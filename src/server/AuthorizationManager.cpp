// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "AuthorizationManager.h"

#include "globals.h"

// Stuff for PAM to work
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/openpam.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <login_cap.h>

//Internal defines
// -- token management
#define TIMEOUTSECS 900 // (15 minutes) time before a token becomes invalid
#define AUTHCHARS QString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
#define TOKENLENGTH 20
// -- Connection failure limitations
#define AUTHFAILLIMIT 5 //number of sequential failures before IP is blocked for a time
#define FAILOVERMINS 10 //after this many minutes without a new login attempt the failure count will reset

AuthorizationManager::AuthorizationManager() : QObject(){
  HASH.clear();
  IPFAIL.clear();
  //initialize the random number generator (need to generate auth tokens)
  qsrand(QDateTime::currentMSecsSinceEpoch());
}

AuthorizationManager::~AuthorizationManager(){
	
}

// == Token Interaction functions ==
void AuthorizationManager::clearAuth(QString token){
  //clear an authorization token
  QString id = hashID(token);
  if(!id.isEmpty()){ HASH.remove(id); }
}

bool AuthorizationManager::checkAuth(QString token){
	//see if the given token is valid
  bool ok = false;
  QString id = hashID(token);
  if(!id.isEmpty()){
    //Also verify that the token has not timed out
    ok = (HASH[id] > QDateTime::currentDateTime());
    if(ok){ HASH.insert(id, QDateTime::currentDateTime().addSecs(TIMEOUTSECS)); } //valid - bump the timestamp
  }
  return ok;
}

bool AuthorizationManager::hasFullAccess(QString token){
  bool ok = false;
  QString id = hashID(token);
  if(!id.isEmpty()){
    //Also verify that the token has not timed out
    if( HASH[id] > QDateTime::currentDateTime() ){  
      ok = id.section("::::",1,1)=="operator";
    }
  }
  return ok;
}

//SSL Certificate register/revoke/list
bool AuthorizationManager::RegisterCertificate(QString token, QSslCertificate cert){
  if(!checkAuth(token)){ return false; }
  QString user = hashID(token).section("::::",2,2); //get the user name from the currently-valid token
  CONFIG->setValue("RegisteredCerts/"+user+"/"+QString(cert.publicKey().toPem()), cert.toText());
  return true;
}

bool AuthorizationManager::RevokeCertificate(QString token, QString key, QString user){
  //user will be the current user if not empty - cannot touch other user's certs without full perms on current session
  QString cuser = hashID(token).section("::::",2,2);
  if(user.isEmpty()){ user = cuser; } //only probe current user
  if(user !=cuser){
    //Check permissions for this cross-user action
    if(!hasFullAccess(token)){ return false; }
  }
  //Check that the given cert exists first
  if( !CONFIG->contains("RegisteredCerts/"+user+"/"+key) ){ return false; }
  CONFIG->remove("RegisteredCerts/"+user+"/"+key);
  return true;
}

QJsonObject AuthorizationManager::ListCertificates(QString token){
  QJsonObject obj;
  QStringList keys; //Format: "RegisteredCerts/<user>/<key>"
  if( hasFullAccess(token) ){ 
    //Read all user's certs
    keys = CONFIG->allKeys().filter("RegisteredCerts/");
  }else{
    //Only list certs for current user
    QString cuser = hashID(token).section("::::",2,2);
    keys = CONFIG->allKeys().filter("RegisteredCerts/"+cuser+"/");
  }
  keys.sort();
  //Now put the known keys into the output structure arranged by username/key
  QJsonObject user; QString username;
  for(int i=0; i<keys.length(); i++){
    if(username!=keys[i].section("/",1,1)){
      if(!user.isEmpty()){ obj.insert(username, user); user = QJsonObject(); } //save the current info to the output
      username = keys[i].section("/",1,1); //save the new username for later
    }
    user.insert(keys[i].section("/",2,3000), CONFIG->value(keys[i]).toString() ); //just in case the key has additional "/" in it
  }
  if(!user.isEmpty() && !username.isEmpty()){ obj.insert(username, user); }
  
  return obj;
}

//Generic functions
int AuthorizationManager::checkAuthTimeoutSecs(QString token){
	//Return the number of seconds that a token is valid for
  if(!HASH.contains(token)){ return 0; } //invalid token
  return QDateTime::currentDateTime().secsTo( HASH[token] );
}


// == Token Generation functions
QString AuthorizationManager::LoginUP(QHostAddress host, QString user, QString pass){
	//Login w/ username & password
  bool localhost = ( (host== QHostAddress::LocalHost) || (host== QHostAddress::LocalHostIPv6) || (host.toString()=="::ffff:127.0.0.1") );
  bool ok = false;
  //First check that the user is valid on the system and part of the operator group
  bool isOperator = false;
  if(user!="root" && user!="toor"){
    QStringList groups = getUserGroups(user);
    if(groups.contains("wheel")){ isOperator = true; } //full-access user
    else if(!groups.contains("operator")){
      return ""; //user not allowed access if not in either of the wheel/operator groups
    }
  }else{ isOperator = true; }
  qDebug() << "Check username/password" << user << pass << localhost;
  //Need to run the full username/password through PAM
  if(!localhost || user=="root" || user=="toor"){
    ok = pam_checkPW(user,pass);
  }else{
    ok = true; //allow local access for users without password
  }

  qDebug() << "User Login Attempt:" << user << " Success:" << ok << " IP:" << host.toString();
  LogManager::log(LogManager::HOST, QString("User Login Attempt: ")+user+"   Success: "+(ok?"true":"false")+"   IP: "+host.toString() );
  if(!ok){ 
    //invalid login
    //Bump the fail count for this host
    bool overlimit = BumpFailCount(host.toString());
    if(overlimit){ emit BlockHost(host); }
    return (overlimit ? "REFUSED" : "");
  }else{ 
    //valid login - generate a new token for it
    ClearHostFail(host.toString());
    return generateNewToken(isOperator, user);
  } 
}

QString AuthorizationManager::LoginUC(QHostAddress host, QString user, QList<QSslCertificate> certs){
  //Login w/ username & SSL certificate
  bool localhost = ( (host== QHostAddress::LocalHost) || (host== QHostAddress::LocalHostIPv6) || (host.toString()=="::ffff:127.0.0.1") );
  bool ok = false;
  //First check that the user is valid on the system and part of the operator group
  bool isOperator = false;
  if(user!="root" && user!="toor"){
    QStringList groups = getUserGroups(user);
    if(groups.contains("wheel")){ isOperator = true; } //full-access user
    else if(!groups.contains("operator")){
      return ""; //user not allowed access if not in either of the wheel/operator groups
    }
  }else{ isOperator = true; }
  qDebug() << "Check username/certificate combination" << user << localhost;

  //Need to check the registered certificates for the designated user
  if(!localhost || user=="root" || user=="toor"){
    for(int i=0; i<certs.length() && !ok; i++){
      if(CONFIG->contains("RegisteredCerts/"+user+"/"+QString(certs[i].publicKey().toPem()) ) ){
        //Cert was registered - check expiration info
	// TO-DO
	ok = true;
      }
    }
  }else{
    ok = true; //allow local access for users without password
  }

  qDebug() << "User Login Attempt:" << user << " Success:" << ok << " IP:" << host.toString();
  LogManager::log(LogManager::HOST, QString("User Login Attempt: ")+user+"   Success: "+(ok?"true":"false")+"   IP: "+host.toString() );
  if(!ok){
    //invalid login
    //Bump the fail count for this host
    bool overlimit = BumpFailCount(host.toString());
    if(overlimit){ emit BlockHost(host); }
    return (overlimit ? "REFUSED" : "");
  }else{
    //valid login - generate a new token for it
    ClearHostFail(host.toString());
    return generateNewToken(isOperator, user);
  }
}

QString AuthorizationManager::LoginService(QHostAddress host, QString service){
  bool localhost = ( (host== QHostAddress::LocalHost) || (host== QHostAddress::LocalHostIPv6) );
	
  //Login a particular automated service
  qDebug() << "Service Login Attempt:" << service << " Success:" << localhost;
  if(!localhost){ return ""; } //invalid - services must be local for access
  //Check that the service is valid on the system
  bool isok = false;
  if(service!="root" && service!="toor" && localhost){
    QStringList groups = getUserGroups(service);
    isok = (groups.contains(service) && !groups.contains("wheel") && !groups.contains("operator"));
  }
  
  //Now generate a new token and send it back
  if(!isok){ 
    //invalid login
    if(!localhost){
      //Bump the fail count for this host
      bool overlimit = BumpFailCount(host.toString());
      if(overlimit){ emit BlockHost(host); }
      return (overlimit ? "REFUSED" : "");	  
    }else{
      return "";
    }
  }else{ return generateNewToken(false, service); }//services are never given operator privileges
}

// =========================
//               PRIVATE
// =========================
QString AuthorizationManager::generateNewToken(bool isOp, QString user){
  QString tok;
  for(int i=0; i<TOKENLENGTH; i++){
    tok.append( AUTHCHARS.at( qrand() % AUTHCHARS.length() ) );
  }
  
  if( !hashID(tok).isEmpty() ){ 
    //Just in case the randomizer came up with something identical - re-run it
    tok = generateNewToken(isOp, user);
  }else{ 
    //unique token created - add it to the hash with the current time (+timeout)
    QString id = tok + "::::"+(isOp ? "operator" : "user")+"::::"+user; //append operator status to auth key
    HASH.insert(id, QDateTime::currentDateTime().addSecs(TIMEOUTSECS) );
  }
  return tok;
}

QStringList AuthorizationManager::getUserGroups(QString user){
  QProcess proc;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("LANG", "C");
  env.insert("LC_All", "C");
  proc.setProcessEnvironment(env);
  proc.setProcessChannelMode(QProcess::MergedChannels);
  proc.start("id", QStringList() << "-nG" << user);
  if(!proc.waitForStarted(30000)){ return QStringList(); } //process never started - max wait of 30 seconds
  while(!proc.waitForFinished(500)){
    if(proc.state() != QProcess::Running){ break; } //somehow missed the finished signal
    QCoreApplication::processEvents();
  }
  QStringList out = QString(proc.readAllStandardOutput()).remove("\n").split(" ");
  //qDebug() << "Found Groups for user:" << user << out;
  return out;	
}

bool AuthorizationManager::BumpFailCount(QString host){
  //Returns: true if the failure count is over the limit
  //key: "<IP>::::<failnum>"
  QStringList keys = QStringList(IPFAIL.keys()).filter(host+"::::");
  int fails = 0;
  if(!keys.isEmpty()){
    //Take the existing key/value and put a new one in (this limits the filter to 1 value maximum)
    QDateTime last = IPFAIL.take(keys[0]);
    if(last.addSecs(FAILOVERMINS*60) > QDateTime::currentDateTime() ){
     fails = keys[0].section("::::",1,1).toInt();
    }
  }
  fails++;
  IPFAIL.insert(host+"::::"+QString::number(fails), QDateTime::currentDateTime() );
  return (fails>=AUTHFAILLIMIT);	
}

void AuthorizationManager::ClearHostFail(QString host){
  QStringList keys = QStringList(IPFAIL.keys()).filter(host+"::::");
  for(int i=0; i<keys.length(); i++){ IPFAIL.remove(keys[i]); }
}

/*
 ========== PAM FUNCTIONS ==========
*/
static struct pam_conv pamc = { openpam_nullconv, NULL };
pam_handle_t *pamh;

bool AuthorizationManager::pam_checkPW(QString user, QString pass){
  //Convert the inputs to C character arrays for use in PAM
  QByteArray tmp = user.toUtf8();
  char* cUser = tmp.data();
  QByteArray tmp2 = pass.toUtf8();
  char* cPassword = tmp2.data();
  //initialize variables
  bool result = false;
  int ret;
  //Initialize PAM
  ret = pam_start( user=="root" ? "system": "login", cUser, &pamc, &pamh);
  if( ret == PAM_SUCCESS ){
    //Place the user-supplied password into the structure 
    ret = pam_set_item(pamh, PAM_AUTHTOK, cPassword);
    //Set the TTY 
    //ret = pam_set_item(pamh, PAM_TTY, "pcdm-terminal");
    //Authenticate with PAM
    ret = pam_authenticate(pamh,0);
    if( ret == PAM_SUCCESS ){
      //Check for valid, unexpired account and verify access restrictions
      ret = pam_acct_mgmt(pamh,0);
      if( ret == PAM_SUCCESS ){ result = true; }
    
    }else{
      pam_logFailure(ret);
    }
  }
  //return verification result
  return result;	
}

void AuthorizationManager::pam_logFailure(int ret){
  //Interpret a PAM error message and log it
  qWarning() << "PAM Error: " << ret;
  switch( ret ){
  case PAM_ABORT:
    qWarning() << " - PAM abort error";
    break;
  case PAM_AUTHINFO_UNAVAIL:
    qWarning() << " - Authentication info unavailable";
    break;
  case PAM_AUTH_ERR:
    qWarning() << " - Authentication error";
    break;
  case PAM_BUF_ERR:
    qWarning() << " - Buffer error";
    break;
  case PAM_CONV_ERR:
    qWarning() << " - Conversion error";
    break;
  case PAM_CRED_INSUFFICIENT:
    qWarning() << " - Credentials insufficient";
    break;
  case PAM_MAXTRIES:
    qWarning() << " - Maximum number of tries exceeded";
    break;
  case PAM_PERM_DENIED:
    qWarning() << " - Permission denied";
    break;
  case PAM_SERVICE_ERR:
    qWarning() << " - Service error";
    break;
  case PAM_SYMBOL_ERR:
    qWarning() << " - Symbol error";
    break;
  case PAM_SYSTEM_ERR:
    qWarning() << " - System error";
    break;
  case PAM_USER_UNKNOWN:
    qWarning() << " - Unknown user";
    break;
  default:
    qWarning() << " - Unrecognized authentication error";
  }
	
}
