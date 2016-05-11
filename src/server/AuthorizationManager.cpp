// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "AuthorizationManager.h"

#include "globals.h"

#include <QCryptographicHash>
#include "library/sysadm-general.h" //simplification functions

// Stuff for PAM to work
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/openpam.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <login_cap.h>

//Stuff for OpenSSL to work
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

//Internal defines
// -- token management
#define TIMEOUTSECS 900 // (15 minutes) time before a token becomes invalid
#define AUTHCHARS QString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
#define TOKENLENGTH 40

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
  if(token.isEmpty() || token.length() < TOKENLENGTH){ return; } //not a valid token
  //clear an authorization token
  QString id = hashID(token);
  //qDebug() << "Clear Auth:" << id;
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
bool AuthorizationManager::RegisterCertificate(QString token, QString pubkey, QString nickname, QString email){
  if(!checkAuth(token)){ return false; }
  QString user = hashID(token).section("::::",2,2); //get the user name from the currently-valid token
  //NOTE: The public key should be a base64 encoded string
  CONFIG->setValue("RegisteredCerts/"+user+"/"+pubkey, "Nickname: "+nickname+"\nEmail: "+email+"\nDate Registered: "+QDateTime::currentDateTime().toString(Qt::ISODate) );
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

void AuthorizationManager::ListCertificates(QString token, QJsonObject *out){
  QStringList keys; //Format: "RegisteredCerts/<user>/<key>"
  if( hasFullAccess(token) ){ 
    //Read all user's certs
    keys = CONFIG->allKeys().filter("RegisteredCerts/");
    //qDebug() << "Found SSL Keys to List:" << keys;
  }else{
    //Only list certs for current user
    QString cuser = hashID(token).section("::::",2,2);
    keys = CONFIG->allKeys().filter("RegisteredCerts/"+cuser+"/");
    //qDebug() << "Found SSL Keys to List:" << keys;
  }
  keys.sort();
  //Now put the known keys into the output structure arranged by username/key
  QJsonObject user; QString username;
  for(int i=0; i<keys.length(); i++){
    if(username!=keys[i].section("/",1,1)){
      if(!user.isEmpty()){ out->insert(username, user); user = QJsonObject(); } //save the current info to the output
      username = keys[i].section("/",1,1); //save the new username for later
    }
    QString info = CONFIG->value(keys[i]).toString();
    QString key = keys[i].section("/",2,-1);//just in case the key has additional "/" in it
    user.insert(key,info);
  }
  if(!user.isEmpty() && !username.isEmpty()){ out->insert(username, user); }
}

void AuthorizationManager::ListCertificateChecksums(QJsonObject *out){
 QStringList keys; //Format: "RegisteredCerts/<user>/<key>" (value is full text)
    //Read all user's certs (since we only need checksums)
    keys = CONFIG->allKeys().filter("RegisteredCerts/");
  keys.sort();
  QJsonArray arr;
  QCryptographicHash chash(QCryptographicHash::Md5);
  for(int i=0; i<keys.length(); i++){
    chash.addData( keys[i].section("/",2,-1).toLocal8Bit() );
    QByteArray res = chash.result();
    chash.reset();
    arr << QString(res);
  }
  out->insert("md5_keys", arr);
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
  bool isOperator = false;
  //First check that the user is valid on the system and part of the operator group
  if( CONFIG->value("auth/allowUserPassAuth",true).toBool() ){
    if(user!="root" && user!="toor"){
      QStringList groups = getUserGroups(user);
      if(groups.contains("wheel")){ isOperator = true; } //full-access user
      else if(!groups.contains("operator")){
        return ""; //user not allowed access if not in either of the wheel/operator groups
      }
    }else{ isOperator = true; }
    //qDebug() << "Check username/password" << user << pass << localhost;
    //Need to run the full username/password through PAM
    if(!localhost || user=="root" || user=="toor"){
      ok = pam_checkPW(user,pass);
    }else{
      ok = local_checkActive(user); //allow local access for users without password
    }
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
  bool localhost = ( (host== QHostAddress::LocalHost) || (host== QHostAddress::LocalHostIPv6) || (host.toString()=="::ffff:127.0.0.1") );
  
  //Login a particular automated service
  qDebug() << "Service Login Attempt:" << service << " Success:" << localhost;
  if(!localhost){ return ""; } //invalid - services must be local for access
  //Check that the service is valid on the system
  bool isok = localhost && CONFIG->value("auth/allowServiceAuth",false).toBool( );
  if(service!="root" && service!="toor" && isok){
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

//Stage 1 SSL Login Check: Generation of random string for this user
QString AuthorizationManager::GenerateEncCheckString(){
  QString key;
  for(int i=0; i<TOKENLENGTH; i++){
    key.append( AUTHCHARS.at( qrand() % AUTHCHARS.length() ) );
  }
  if(HASH.contains("SSL_CHECK_STRING/"+key)){ key = GenerateEncCheckString(); } //get a different one
  else{
    //insert this new key into the hash for later
    HASH.insert("SSL_CHECK_STRING/"+key, QDateTime::currentDateTime().addSecs(30) ); //only keep a key "alive" for 30 seconds
  }
  return key;
}

QString AuthorizationManager::GenerateEncString_bridge(QString str){
  //Get the private key
  return str; //NOT IMPLEMENTED YET
  QByteArray privkey = "";//SSL_cfg.privateKey().toPem();
  
  //Now use this private key to encode the given string
  unsigned char encode[4098] = {};
  RSA *rsa= NULL;
  BIO *keybio = NULL;
  keybio = BIO_new_mem_buf(privkey.data(), -1);
  if(keybio==NULL){ return ""; }
  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
  if(rsa==NULL){ return ""; }
  int len = RSA_private_encrypt(str.length(), (unsigned char*)(str.toLatin1().data()), encode, rsa, RSA_PKCS1_PADDING);
  if(len <0){ return ""; }
  else{ 
    //Now return this as a base64 encoded string
    QByteArray str_encode( (char*)(encode), len);
    /*qDebug() << "Encoded String Info";
    qDebug() << " - Raw string:" << str << "Length:" << str.length();
    qDebug() << " - Encoded string:" << str_encode << "Length:" << str_encode.length();*/
    str_encode = str_encode.toBase64();
    /*qDebug() << " - Enc string (base64):" << str_encode << "Length:" << str_encode.length();
    qDebug() << " - Enc string (QString):" << QString(str_encode);*/
    return QString( str_encode ); 
  }

}

//Stage 2 SSL Login Check: Verify that the returned/encrypted string can be decoded and matches the initial random string
QString AuthorizationManager::LoginUC(QHostAddress host, QString encstring){
  //Login w/  SSL certificate
  bool ok = false;
  //qDebug() << "SSL Auth Attempt";
    //First clean out any old strings/keys
    QStringList pubkeys = QStringList(HASH.keys()).filter("SSL_CHECK_STRING/"); //temporary, re-use variable below
    for(int i=0; i<pubkeys.length(); i++){ 
      //Check expiration time on each initial string
      if(QDateTime::currentDateTime() > HASH[pubkeys[i]]){ 
        //Note: normally only 1 request per user at a time, but it is possible for a couple different clients to try 
        // and authenticate as the same user (but different keys) at nearly the same time - so keep a short valid-string time frame (<30 seconds)
        // to mitigate this possibility (need to prevent the second user-auth request from invalidating the first before the first auth handshake is finished)
        HASH.remove(pubkeys[i]); //initstring expired - go ahead and remove it to reduce calc time later
      }
    }
    //Now re-use the "pubkeys" variable for the public SSL keys
    QString user;
    pubkeys = CONFIG->allKeys().filter("RegisteredCerts/"); //Format: "RegisteredCerts/<user>/<key>"
    //qDebug() << " - Check pubkeys";// << pubkeys;
    for(int i=0; i<pubkeys.length() && !ok; i++){
      //Decrypt the string with this pubkey - and compare to the outstanding initstrings
      QString key = DecryptSSLString(encstring, pubkeys[i].section("/",2,-1));
      if(HASH.contains("SSL_CHECK_STRING/"+key)){
        //Valid reponse found
	//qDebug() << " - Found Valid Key";
        ok = true;
        //Remove the initstring from the hash (already used)
        HASH.remove("SSL_CHECK_STRING/"+key);
        user = pubkeys[i].section("/",1,1);
      }
    }
  bool isOperator = false;    
  if(ok){
    //qDebug() << "Check user groups";
    //First check that the user is valid on the system and part of the operator group
    if(user!="root" && user!="toor"){
      QStringList groups = getUserGroups(user);
      if(groups.contains("wheel")){ isOperator = true; } //full-access user
      else if(!groups.contains("operator")){
        return ""; //user not allowed access if not in either of the wheel/operator groups
      }
    }else{ isOperator = true; }

  }
  if(user.isEmpty()){ ok = false; }

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

QString AuthorizationManager::encryptString(QString msg, QString key){
  //do nothing yet
  return msg;
}

QString AuthorizationManager::decryptString(QString msg, QString key){
  return msg; //do nothing yet
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
    //qDebug() << "Current HASH Contents:" << HASH.keys();
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

bool AuthorizationManager::local_checkActive(QString user){
  //Check for X Sessions first (don't show up with normal login verification tools)
  QDir xdir("/tmp/.X11-unix");
  qDebug() << "Check local user activity:" << user;
  if(xdir.exists() && !xdir.entryList(QDir::System | QDir::NoDotAndDotDot, QDir::Name).isEmpty() ){
    //Found an active graphical session - check for active processes associated with the user
    QString res = sysadm::General::RunCommand("ps",QStringList() << "-U" << user << "-x");
    //qDebug() << "PS list length:" << res.split("\n").length();
    return (res.split("\n").length()>2); //more than 1 active process for this user (labels + shell/desktop + tool used to communicate with sysadm)
  }else{
    //No X sessions - look for normal login sessions
   QStringList active =  sysadm::General::RunCommand("users").section("\n",0,0).split(" ");
   //qDebug() << "active users" << active;
   return active.contains(user);
  }
}

bool AuthorizationManager::BumpFailCount(QString host){
  //Returns: true if the failure count is over the limit
  //key: "<IP>::::<failnum>"
  QStringList keys = QStringList(IPFAIL.keys()).filter(host+"::::");
  int fails = 0;
  if(!keys.isEmpty()){
    //Take the existing key/value and put a new one in (this limits the filter to 1 value maximum)
    QDateTime last = IPFAIL.take(keys[0]);
    if(last.addSecs(BlackList_AuthFailResetMinutes*60) > QDateTime::currentDateTime() ){
     fails = keys[0].section("::::",1,1).toInt();
    }
  }
  fails++;
  IPFAIL.insert(host+"::::"+QString::number(fails), QDateTime::currentDateTime() );
  return (fails>=BlackList_AuthFailsToBlock);	
}

void AuthorizationManager::ClearHostFail(QString host){
  QStringList keys = QStringList(IPFAIL.keys()).filter(host+"::::");
  for(int i=0; i<keys.length(); i++){ IPFAIL.remove(keys[i]); }
}

QString AuthorizationManager::DecryptSSLString(QString encstring, QString pubkey){
  //Convert from the base64 string back into byte array
  QByteArray enc;
    enc.append(encstring);
  enc = QByteArray::fromBase64(enc);
  QByteArray pkey;
    pkey.append(pubkey);
  pkey = QByteArray::fromBase64(pkey);
  //Now start the SSL routine
  /*qDebug() << "Decrypt String:" << "Length:" << enc.length() << enc;
  qDebug() << " - Base64:" << encstring << "Length:" << encstring.length();
  qDebug() << " - pubkey (base64):" << pubkey << "Length:" << pubkey.length();
  qDebug() << " - pubkey:" << pkey << "Length:" << pkey.length();*/
  unsigned char decode[4098] = {};
  RSA *rsa= NULL;
  BIO *keybio = NULL;
  //qDebug() << " - Generate keybio";
  keybio = BIO_new_mem_buf(pkey.data(), -1);
  if(keybio==NULL){ return ""; }
  //qDebug() << " - Read pubkey";
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  if(rsa==NULL){ qDebug() << " - Invalid RSA key!!"; return ""; }
  //qDebug() << " - Decrypt string";
  int len = RSA_public_decrypt(enc.length(), (unsigned char*)(enc.data()), decode, rsa, RSA_PKCS1_PADDING);
  if(len<0){ return ""; }
  else{ return QString( QByteArray( (char*)(decode), len) ); }
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
