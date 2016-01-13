// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "AuthorizationManager.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>

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
#define TIMEOUTSECS 900 // (15 minutes) time before a token becomes invalid
#define AUTHCHARS QString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
#define TOKENLENGTH 20

AuthorizationManager::AuthorizationManager(){
  HASH.clear();
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

int AuthorizationManager::checkAuthTimeoutSecs(QString token){
	//Return the number of seconds that a token is valid for
  if(!HASH.contains(token)){ return 0; } //invalid token
  return QDateTime::currentDateTime().secsTo( HASH[token] );
}


// == Token Generation functions
QString AuthorizationManager::LoginUP(bool localhost, QString user, QString pass){
	//Login w/ username & password
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
  //qDebug() << "Check username/password" << user << pass;
  //Need to run the full username/password through PAM
  if(!localhost || user=="root" || user=="toor"){
    ok = pam_checkPW(user,pass);
  }else{
    ok = true; //allow local access for users without password
  }
  
  qDebug() << "User Login Attempt:" << user << " Success:" << ok << " Local Login:" << localhost;
  if(!ok){ return ""; } //invalid login
  else{ return generateNewToken(isOperator); } //valid login - generate a new token for it
}

QString AuthorizationManager::LoginService(bool localhost, QString service){
  //Login a particular automated service
  qDebug() << "Service Login Attempt:" << service << " Success:" << localhost;
  if(!localhost){ return ""; } //invalid - services must be local for access
  //Check that the service is valid on the system
  // -- TO-DO
  
  //Now generate a new token and send it back
  return generateNewToken(false); //services are never given operator privileges
}

// =========================
//               PRIVATE
// =========================
QString AuthorizationManager::generateNewToken(bool isOp){
  QString tok;
  for(int i=0; i<TOKENLENGTH; i++){
    tok.append( AUTHCHARS.at( qrand() % AUTHCHARS.length() ) );
  }
  
  if( !hashID(tok).isEmpty() ){ 
    //Just in case the randomizer came up with something identical - re-run it
    tok = generateNewToken(isOp);
  }else{ 
    //unique token created - add it to the hash with the current time (+timeout)
    QString id = tok + "::::"+(isOp ? "operator" : "user"); //append operator status to auth key
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
