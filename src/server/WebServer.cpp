// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "WebServer.h"
#include "globals.h"

#define DEBUG 0

//=======================
//              PUBLIC
//=======================
WebServer::WebServer(){
  //Setup all the various settings
  WSServer = 0;
  TCPServer = 0;
  bridgeTimer = new QTimer(this);
    bridgeTimer->setInterval(60000); //1 minute
    connect(bridgeTimer, SIGNAL(timeout()), this, SLOT(checkBridges()) );
  AUTH = new AuthorizationManager();
  connect(AUTH, SIGNAL(BlockHost(QHostAddress)), this, SLOT(BlackListConnection(QHostAddress)) );
}

WebServer::~WebServer(){
  delete AUTH;
}

bool WebServer::startServer(quint16 port, bool websocket){
  if(!QSslSocket::supportsSsl()){ qDebug() << "No SSL Support on this system!!!"; return false; }
  else{
    qDebug() << "Using SSL Library:";
    qDebug() << " - Version:" << QSslSocket::sslLibraryVersionString();
  }
  bool ok = false;
  if(websocket && BRIDGE_ONLY){ ok = true; }
  else if(websocket){ ok = setupWebSocket(port); }
  else{ ok = setupTcp(port); }
  
  if(ok){ 
    QCoreApplication::processEvents();
    qDebug() << "Server Started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
    qDebug() << " Port:" << (BRIDGE_ONLY ? "(Bridge Only)" : QString::number(port));
    if(WSServer!=0){ qDebug() << " URL:" << WSServer->serverUrl().toString(); }
  }else{ 
    qCritical() << "Could not start server - exiting..."; 
  }
  if(ok && websocket){
    bridgeTimer->start();
    QTimer::singleShot(5, this, SLOT(checkBridges()));
  }
  return ok;
}

void WebServer::stopServer(){
  if(bridgeTimer->isActive()){ bridgeTimer->stop(); }
  if(WSServer!=0){ WSServer->close(); } //note - this will throw the "closed()" signal when done
  else if(TCPServer!=0){ TCPServer->close(); QCoreApplication::exit(0);} //no corresponding signal
}

//===================
//     PRIVATE
//===================
bool WebServer::setupWebSocket(quint16 port){
  WSServer = new QWebSocketServer("sysadm-server", QWebSocketServer::SecureMode, this);
  //SSL Configuration
  QSslConfiguration config = QSslConfiguration::defaultConfiguration();
	QFile CF( QStringLiteral(SSLCERTFILEWS) ); 
	  if(CF.open(QIODevice::ReadOnly) ){
	    QSslCertificate CERT(&CF,QSsl::Pem);
	    config.setLocalCertificate( CERT );
	    CF.close();   
	  }else{
	    qWarning() << "Could not read WS certificate file:" << CF.fileName();
	  }
	QFile KF( QStringLiteral(SSLKEYFILEWS));
	  if(KF.open(QIODevice::ReadOnly) ){
	    QSslKey KEY(&KF, QSsl::Rsa, QSsl::Pem);
	    config.setPrivateKey( KEY );
	    KF.close();	
	  }else{
	    qWarning() << "Could not read WS key file:" << KF.fileName();
	  }
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(SSLVERSION);
  WSServer->setSslConfiguration(config);
  //Setup Connections
  connect(WSServer, SIGNAL(newConnection()), this, SLOT(NewSocketConnection()) );
  connect(WSServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(NewConnectError(QAbstractSocket::SocketError)) );
  //  -- websocket specific signals
  connect(WSServer, SIGNAL(closed()), this, SLOT(ServerClosed()) );
  connect(WSServer, SIGNAL(serverError(QWebSocketProtocol::CloseCode)), this, SLOT(ServerError(QWebSocketProtocol::CloseCode)) );
  connect(WSServer, SIGNAL(originAuthenticationRequired(QWebSocketCorsAuthenticator*)), this, SLOT(OriginAuthRequired(QWebSocketCorsAuthenticator*)) );
  connect(WSServer, SIGNAL(peerVerifyError(const QSslError&)), this, SLOT(PeerVerifyError(const QSslError&)) );
  connect(WSServer, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(SslErrors(const QList<QSslError>&)) );
  connect(WSServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(ConnectError(QAbstractSocket::SocketError)) );
  //Now start the server
  return WSServer->listen(QHostAddress::Any, port);
}

bool WebServer::setupTcp(quint16 port){
  TCPServer = new SslServer(this);
  //Setup Connections
  connect(TCPServer, SIGNAL(newConnection()), this, SLOT(NewSocketConnection()) );
  connect(TCPServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(NewConnectError(QAbstractSocket::SocketError)) );
  //Now start the server
  return TCPServer->listen(QHostAddress::Any, port);	
}

//Server Blacklist / DDOS mitigator
bool WebServer::allowConnection(QHostAddress addr){
  //Check if this addr is on the blacklist
  QString key = "blacklist/"+addr.toString();
  if(!CONFIG->contains(key) ){ return true; } //not in the list
  //Address on the list - see if the timeout has expired 
  QDateTime dt = CONFIG->value(key,QDateTime()).toDateTime();
  if(dt.addSecs(BlackList_BlockMinutes*60) < QDateTime::currentDateTime()){
    //This entry has timed out - go ahead and allow it
    CONFIG->remove(key); //make the next connection check for this IP faster again
    return true;
  }else{
    return false; //blacklist block is still in effect
  }
  
}

QString WebServer::generateID(){
  int id = 1; //start integer ID's with 1
  for(int i=0; i<OpenSockets.length(); i++){
    bool ok=false;
    int num = OpenSockets[i]->ID().toInt(&ok); //some ID's will not be a simple number (bridge connections)
    if(ok && num>=id){ id = OpenSockets[i]->ID().toInt()+1; }
  }
  return QString::number(id);
}

//=======================
//       PRIVATE SLOTS
//=======================
//GENERIC SERVER SIGNALS
// New Connection Signals
void WebServer::NewSocketConnection(){
  WebSocket *sock = 0;
  if(WSServer!=0){
    if(WSServer->hasPendingConnections()){ 
      QWebSocket *ws = WSServer->nextPendingConnection();
      if( !allowConnection(ws->peerAddress()) ){ ws->close(); }
      else{ sock = new WebSocket( this, ws, generateID(), AUTH); }
    }
  }else if(TCPServer!=0){
    if(TCPServer->hasPendingConnections()){ 
	QSslSocket *ss = TCPServer->nextPendingConnection();
	if( !allowConnection(ss->peerAddress()) ){ ss->close(); }    
	else{ sock = new WebSocket( this, ss, generateID(), AUTH); }
    }
  }
  if(sock==0){ return; } //no new connection
  //qDebug() << "New Socket Connection";	
  connect(sock, SIGNAL(SocketClosed(QString)), this, SLOT(SocketClosed(QString)) );
  //connect(EVENTS, SIGNAL(NewEvent(EventWatcher::EVENT_TYPE, QJsonValue)), sock, SLOT(EventUpdate(EventWatcher::EVENT_TYPE, QJsonValue)) );
  OpenSockets << sock;
}

void WebServer::NewConnectError(QAbstractSocket::SocketError err){         
  qWarning() << "New Connection Error["+QString::number(err)+"]:" << ( (WSServer!=0) ? WSServer->errorString() : TCPServer->errorString());
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
}

//Socket Blacklist function
void WebServer::BlackListConnection(QHostAddress addr){
  //Make sure this is not the localhost (never block that)
  if(addr!=QHostAddress(QHostAddress::LocalHost) && addr!=QHostAddress(QHostAddress::LocalHostIPv6)  && addr.toString()!="::ffff:127.0.0.1" ){
    //Block this remote host
    LogManager::log(LogManager::HOST,"Blacklisting IP Temporarily: "+addr.toString());
    CONFIG->setValue("blacklist/"+addr.toString(), QDateTime::currentDateTime());
  }
}

//WEBSOCKET SERVER SIGNALS
// Overall Server signals
void WebServer::ServerClosed(){
  QCoreApplication::exit(0);
}

void WebServer::ServerError(QWebSocketProtocol::CloseCode code){
  qWarning() << "Server Error["+QString::number(code)+"]:" << ( (WSServer!=0) ? WSServer->errorString() : TCPServer->errorString());
}

// -  SSL/Authentication Signals (still websocket only)
void WebServer::OriginAuthRequired(QWebSocketCorsAuthenticator *auth){
  //This just provides the ability to check the URL/app which is trying to connect from
  //	- this is not really useful right now since anything could be set there (accurate or not)
  auth->setAllowed(true);
}

void WebServer::ConnectError(QAbstractSocket::SocketError err){
  qDebug() << "Connection Error" << err;
}

void WebServer::PeerVerifyError(const QSslError &err){
  qDebug() << "Peer Verification Error:" << err.errorString();
	
}

void WebServer::SslErrors(const QList<QSslError> &list){
  qWarning() << "SSL Errors:";
  for(int i=0; i<list.length(); i++){
    qWarning() << " - " << list[i].errorString();
  }
}

// - More Functions for all socket interactions
void WebServer::SocketClosed(QString ID){
  qDebug() << "Socket Closed:" << ID << QDateTime::currentDateTime().toString(Qt::ISODate);
  for(int i=0; i<OpenSockets.length(); i++){
    if(OpenSockets[i]->ID()==ID){ delete OpenSockets.takeAt(i); break; }
  }
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
}

// BRIDGE Connection checks
void WebServer::checkBridges(){
  //qDebug() << "Check Bridges:" << WS_MODE << QDateTime::currentDateTime().toString(Qt::ISODate);
  if(!WS_MODE){ return; }

  //Get all the unique bridge URL's we need connections to
  QStringList bridgeKeys = CONFIG->allKeys().filter("bridge_connections/");
  for(int i=0; i<bridgeKeys.length(); i++){
    bridgeKeys[i] = CONFIG->value(bridgeKeys[i]).toString(); //turn the key into the URL for the bridge  (unique ID)
  }
  //Now browse through all the current connections and see if any are already active
  for(int i=0; i<OpenSockets.length(); i++){
    bool active = OpenSockets[i]->isActive();
    if( bridgeKeys.contains( OpenSockets[i]->ID() ) && active){
      bridgeKeys.removeAll( OpenSockets[i]->ID() ); //already running - remove from the temporary list
    }else if( OpenSockets[i]->ID().toInt()==0 || !active){
      //non-integer ID - another bridge connection which must have been removed from the server settings
      OpenSockets[i]->closeConnection();
    }
  }
  //Now startup any connections which are missing
  for(int i=0; i<bridgeKeys.length(); i++){
    //qDebug() << "Try to connect to bridge:" << bridgeKeys[i];
    WebSocket *sock = new WebSocket(this, bridgeKeys[i], bridgeKeys[i], AUTH);
    connect(sock, SIGNAL(SocketClosed(QString)), this, SLOT(SocketClosed(QString)) );
    OpenSockets << sock;
  }
}
