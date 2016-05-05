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

  //Check that the given cert exists first
  if( !CONFIG->contains("RegisteredCerts/"+cuser+"/"+key) ){ return false; }
  CONFIG->remove("RegisteredCerts/"+cuser+"/"+key);
  return true;
}

void AuthorizationManager::ListCertificates(QString token, QJsonObject *out){
  QStringList keys; //Format: "RegisteredCerts/<user>/<key>"
    //Read all user's certs
    keys = CONFIG->allKeys().filter("RegisteredCerts/");
    //qDebug() << "Found SSL Keys to List:" << keys;

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

//Generic functions
int AuthorizationManager::checkAuthTimeoutSecs(QString token){
	//Return the number of seconds that a token is valid for
  if(!HASH.contains(token)){ return 0; } //invalid token
  return QDateTime::currentDateTime().secsTo( HASH[token] );
}


// == Token Generation functions

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

  //qDebug() << "User Login Attempt:" << user << " Success:" << ok << " IP:" << host.toString();

  if(!ok){
    //invalid login
    //Bump the fail count for this host
    bool overlimit = BumpFailCount(host.toString());
    if(overlimit){ emit BlockHost(host); }
    return (overlimit ? "REFUSED" : "");
  }else{
    //valid login - generate a new token for it
    ClearHostFail(host.toString());
    return generateNewToken(false, "none");
  }
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

bool AuthorizationManager::BumpFailCount(QString host){
  //Returns: true if the failure count is over the limit
  //key: "<IP>::::<failnum>"
  QStringList keys = QStringList(IPFAIL.keys()).filter(host+"::::");
  int fails = 0;
  if(!keys.isEmpty()){
    //Take the existing key/value and put a new one in (this limits the filter to 1 value maximum)
    QDateTime last = IPFAIL.take(keys[0]);
    if(last.addSecs(CONFIG->value("blacklist_settings/block_minutes",60).toInt()*60) > QDateTime::currentDateTime() ){
     fails = keys[0].section("::::",1,1).toInt();
    }
  }
  fails++;
  IPFAIL.insert(host+"::::"+QString::number(fails), QDateTime::currentDateTime() );
  return (fails>=CONFIG->value("blacklist_settings/fails_to_block",2).toInt());
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
