// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#include "BridgeServer.h"

#define DEBUG 0

//=======================
//              PUBLIC
//=======================
BridgeServer::BridgeServer() : QWebSocketServer("sysadm-bridge", QWebSocketServer::SecureMode){
  //Setup all the various settings
  connect(AUTHSYSTEM, SIGNAL(BlockHost(QHostAddress)), this, SLOT(BlackListConnection(QHostAddress)) );
}

BridgeServer::~BridgeServer(){

}

bool BridgeServer::startServer(quint16 port){
  if(!QSslSocket::supportsSsl()){ qDebug() << "No SSL Support on this system!!!"; return false; }
  else{
    qDebug() << "Using SSL Library:";
    qDebug() << " - Version:" << QSslSocket::sslLibraryVersionString();
  }
  bool ok = setupWebSocket(port);
  
  if(ok){ 
    //QCoreApplication::processEvents();
    qDebug() << " URL:" << this->serverUrl().toString();
  }else{ 
    qCritical() << "Could not start server - exiting..."; 
  }
  
  return ok;
}

//===================
//    PUBLIC SLOTS
//===================
void BridgeServer::sendMessage(QString toID, QString msg){
  //qDebug() << "Try to forward message:" << toID;
  for(int i=0; i<OpenSockets.length(); i++){
    if(OpenSockets[i]->ID()==toID){ OpenSockets[i]->forwardMessage(msg); }
  }
}

//===================
//     PRIVATE
//===================
bool BridgeServer::setupWebSocket(quint16 port){
  //SSL Configuration
  qDebug() << "SSL Files:" << SSLFILEDIR+"/["+SSLCERTFILE+", "+SSLKEYFILE+"]";
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();
	QFile CF( SSLFILEDIR +"/"+SSLCERTFILE ); 
	  if(CF.open(QIODevice::ReadOnly) ){
	    QSslCertificate CERT(&CF,QSsl::Pem);
	    config.setLocalCertificate( CERT );
	    CF.close();   
	  }else{
	    qWarning() << "Could not read WS certificate file:" << CF.fileName();
	  }
	QFile KF( SSLFILEDIR +"/"+SSLKEYFILE );
	  if(KF.open(QIODevice::ReadOnly) ){
	    QSslKey KEY(&KF, QSsl::Rsa, QSsl::Pem);
	    config.setPrivateKey( KEY );
	    KF.close();	
	  }else{
	    qWarning() << "Could not read WS key file:" << KF.fileName();
	  }
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(SSLVERSION);
  this->setSslConfiguration(config);
  //Setup Connections
  connect(this, SIGNAL(newConnection()), this, SLOT(NewSocketConnection()) );
  connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(NewConnectError(QAbstractSocket::SocketError)) );
  connect(this, SIGNAL(closed()), this, SLOT(ServerClosed()) );
  connect(this, SIGNAL(serverError(QWebSocketProtocol::CloseCode)), this, SLOT(ServerError(QWebSocketProtocol::CloseCode)) );
  connect(this, SIGNAL(originAuthenticationRequired(QWebSocketCorsAuthenticator*)), this, SLOT(OriginAuthRequired(QWebSocketCorsAuthenticator*)) );
  connect(this, SIGNAL(peerVerifyError(const QSslError&)), this, SLOT(PeerVerifyError(const QSslError&)) );
  connect(this, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(SslErrors(const QList<QSslError>&)) );
  connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(ConnectError(QAbstractSocket::SocketError)) );
  //Now start the server
  return this->listen(QHostAddress::Any, port);
}

//Server Blacklist / DDOS mitigator
bool BridgeServer::allowConnection(QHostAddress addr){
  //Check if this addr is on the blacklist
  QString key = "blacklist/"+addr.toString();
  if(!CONFIG->contains(key) ){ return true; } //not in the list
  //Address on the list - see if the timeout has expired 
  QDateTime dt = CONFIG->value(key,QDateTime()).toDateTime();
  if(dt.addSecs(CONFIG->value("blacklist_settings/block_minutes",60).toInt()*60) < QDateTime::currentDateTime()){
    //This entry has timed out - go ahead and allow it
    CONFIG->remove(key); //make the next connection check for this IP faster again
    return true;
  }else{
    return false; //blacklist block is still in effect
  }
  
}

QString BridgeServer::generateID(QString name){
  return name+"::"+QUuid::createUuid().toString();
}

//=======================
//       PRIVATE SLOTS
//=======================
//GENERIC SERVER SIGNALS
// New Connection Signals
void BridgeServer::NewSocketConnection(){
  BridgeConnection *sock = 0;
  if(this->hasPendingConnections()){ 
    qDebug() << "New incoming connection..";
    QWebSocket *ws = this->nextPendingConnection();
    if(allowConnection(ws->peerAddress()) ){
      QString name = ws->peerName();
      if(name.isEmpty()){ name = ws->peerAddress().toString(); }
      sock = new BridgeConnection( this, ws, generateID(name) );
    }else{
      ws->abort();
    }
  }
  if(sock==0){ return; } //no new connection
  //qDebug() << "New Socket Connection";	
  connect(sock, SIGNAL(SocketClosed(QString)), this, SLOT(SocketClosed(QString)) );
  connect(sock, SIGNAL(SocketMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)) );
  connect(sock, SIGNAL(keysChanged(QString, bool, QStringList)), this, SLOT(announceKeyChange(QString, bool, QStringList)) );
  OpenSockets << sock;
}

void BridgeServer::NewConnectError(QAbstractSocket::SocketError err){         
  qWarning() << "New Connection Error["+QString::number(err)+"]:" << this->errorString();
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
}

//Socket Blacklist function
void BridgeServer::BlackListConnection(QHostAddress addr){
  //Make sure this is not the localhost (never block that)
  if(addr!=QHostAddress(QHostAddress::LocalHost) && addr!=QHostAddress(QHostAddress::LocalHostIPv6)  && addr.toString()!="::ffff:127.0.0.1" ){
    //Block this remote host
    qDebug() << "Blacklisting IP Temporarily: " << addr.toString();
    CONFIG->setValue("blacklist/"+addr.toString(), QDateTime::currentDateTime());
  }
}

//WEBSOCKET SERVER SIGNALS
// Overall Server signals
void BridgeServer::ServerClosed(){
  QCoreApplication::exit(0);
}

void BridgeServer::ServerError(QWebSocketProtocol::CloseCode code){
  qWarning() << "Server Error["+QString::number(code)+"]:" << this->errorString();
}

// -  SSL/Authentication Signals (still websocket only)
void BridgeServer::OriginAuthRequired(QWebSocketCorsAuthenticator *auth){
  //This just provides the ability to check the URL/app which is trying to connect from
  //	- this is not really useful right now since anything could be set there (accurate or not)
  auth->setAllowed(true);
}

void BridgeServer::ConnectError(QAbstractSocket::SocketError err){
  qDebug() << "Connection Error" << err;
}

void BridgeServer::PeerVerifyError(const QSslError &err){
  qDebug() << "Peer Verification Error:" << err.errorString();
	
}

void BridgeServer::SslErrors(const QList<QSslError> &list){
  qWarning() << "SSL Errors:";
  for(int i=0; i<list.length(); i++){
    qWarning() << " - " << list[i].errorString();
  }
}

// - More Functions for all socket interactions
void BridgeServer::SocketClosed(QString ID){
  qDebug() << "Socket Closed:" << ID;
  for(int i=0; i<OpenSockets.length(); i++){
    if(OpenSockets[i]->ID()==ID){ delete OpenSockets.takeAt(i); break; }
  }
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
}

// Connection Keys Changed
void BridgeServer::announceKeyChange(QString ID, bool isServer, QStringList keys){
  //qDebug() << "Key Change:" << ID << isServer << keys;
  for(int c = 0; c<OpenSockets.length(); c++){
    bool server = OpenSockets[c]->isServer();
    QStringList keys = OpenSockets[c]->validKeySums();
    keys.removeDuplicates();
    QStringList IDs;
    for(int i=0; i<OpenSockets.length(); i++){
      if(i==c){ continue; } //current socket
      else if(OpenSockets[i]->isServer() != server){ //look for a server/client pair
        //compare keys to look for matches
        /*QStringList chkkeys = OpenSockets[i ]->validKeySums();
        chkkeys.removeDuplicates();
        qDebug() << "Known Keys for ID:" << OpenSockets[i]->ID() << chkkeys;
        chkkeys << keys;
        if(chkkeys.removeDuplicates() > 0){ */
          IDs << OpenSockets[i]->ID(); 
        //}
      }
    }//end inner loop of sockets
    OpenSockets[c]->announceIDAvailability(IDs);
  } //end loop over sockets to check
}
