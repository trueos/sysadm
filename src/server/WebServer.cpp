// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "WebServer.h"

#include <QCoreApplication>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QProcess>

#define DEBUG 0

#define APPCAFEWORKING QString("/var/tmp/appcafe/dispatch-queue.working")

//=======================
//              PUBLIC
//=======================
WebServer::WebServer(){
  //Setup all the various settings
  //Any SSL changes
    /*QSslConfiguration ssl = this->sslConfiguration();
      ssl.setProtocol(QSsl::SecureProtocols);
    this->setSslConfiguration(ssl);*/
  WSServer = 0;
  TCPServer = 0;
  AUTH = new AuthorizationManager();
  watcher = new QFileSystemWatcher(this);
    
  connect(watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(WatcherUpdate(QString)) );
  connect(watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(WatcherUpdate(QString)) );
}

WebServer::~WebServer(){
  delete AUTH;
}

bool WebServer::startServer(quint16 port, bool websocket){
  bool ok = false;
  if(websocket){ ok	= setupWebSocket(port); }
  else{ ok = setupTcp(port); }
  
  if(ok){ 
    QCoreApplication::processEvents();
    qDebug() << "Server Started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
    qDebug() << " Port:" << port;
    if(WSServer!=0){ qDebug() << " URL:" << WSServer->serverUrl().toString(); }
    
    if(!QFile::exists(APPCAFEWORKING)){ QProcess::execute("touch "+APPCAFEWORKING); }
    qDebug() << " Dispatcher Events:" << APPCAFEWORKING;
    watcher->addPath(APPCAFEWORKING);
    WatcherUpdate(APPCAFEWORKING); //load it initially
  }else{ 
    qCritical() << "Could not start server - exiting..."; 
  }
  
  return ok;
}

void WebServer::stopServer(){
  if(WSServer!=0){ WSServer->close(); } //note - this will throw the "closed()" signal when done
  else if(TCPServer!=0){ TCPServer->close(); QCoreApplication::exit(0);} //no corresponding signal
}

//===================
//     PRIVATE
//===================
bool WebServer::setupWebSocket(quint16 port){
  WSServer = new QWebSocketServer("sysadm-server", QWebSocketServer::NonSecureMode, this);
  //Setup Connections
  connect(WSServer, SIGNAL(newConnection()), this, SLOT(NewSocketConnection()) );
  connect(WSServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(NewConnectError(QAbstractSocket::SocketError)) );
  //  -- websocket specific signals
  connect(WSServer, SIGNAL(closed()), this, SLOT(ServerClosed()) );
  connect(WSServer, SIGNAL(serverError(QWebSocketProtocol::CloseCode)), this, SLOT(ServerError(QWebSocketProtocol::CloseCode)) );
  connect(WSServer, SIGNAL(originAuthenticationRequired(QWebSocketCorsAuthenticator*)), this, SLOT(OriginAuthRequired(QWebSocketCorsAuthenticator*)) );
  connect(WSServer, SIGNAL(peerVerifyError(const QSslError&)), this, SLOT(PeerVerifyError(const QSslError&)) );
  connect(WSServer, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(SslErrors(const QList<QSslError>&)) );
  //Now start the server
  return WSServer->listen(QHostAddress::Any, port);
}

bool WebServer::setupTcp(quint16 port){
  TCPServer = new QTcpServer(this);
  //Setup Connections
  connect(TCPServer, SIGNAL(newConnection()), this, SLOT(NewSocketConnection()) );
  connect(TCPServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(NewConnectError(QAbstractSocket::SocketError)) );
  //Now start the server
  return TCPServer->listen(QHostAddress::Any, port);	
}

QString WebServer::generateID(){
  int id = 0;
  for(int i=0; i<OpenSockets.length(); i++){
    if(OpenSockets[i]->ID().toInt()>=id){ id = OpenSockets[i]->ID().toInt()+1; }
  }
  return QString::number(id);
}

QString WebServer::readFile(QString path){
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){ return ""; }
  QTextStream in(&file);
  QString contents = in.readAll();
  file.close();
  if(contents.endsWith("\n")){ contents.chop(1); }
  return contents;  
}

//=======================
//       PRIVATE SLOTS
//=======================
//GENERIC SERVER SIGNALS
// New Connection Signals
void WebServer::NewSocketConnection(){
  WebSocket *sock = 0;
  if(WSServer!=0){
    if(WSServer->hasPendingConnections()){ sock = new WebSocket( WSServer->nextPendingConnection(), generateID(), AUTH); }
  }else if(TCPServer!=0){
    if(TCPServer->hasPendingConnections()){ sock = new WebSocket( TCPServer->nextPendingConnection(), generateID(), AUTH); }
  }
  if(sock==0){ return; } //no new connection
  qDebug() << "New Socket Connection";	
  connect(sock, SIGNAL(SocketClosed(QString)), this, SLOT(SocketClosed(QString)) );
  connect(this, SIGNAL(DispatchStatusUpdate(QString)), sock, SLOT(AppCafeStatusUpdate(QString)) );
  sock->setLastDispatch(lastDispatch); //make sure this socket is aware of the latest notification
  OpenSockets << sock;
}

void WebServer::NewConnectError(QAbstractSocket::SocketError err){         
  qWarning() << "New Connection Error["+QString::number(err)+"]:" << ( (WSServer!=0) ? WSServer->errorString() : TCPServer->errorString());
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
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
  qDebug() << "Origin Auth Required:" << auth->origin();
  //if(auth->origin() == this->serverAddress().toString()){
  // TO-DO: Provide some kind of address filtering routine for which to accept/reject
    qDebug() << " - Allowed";
    auth->setAllowed(true);
  //}else{
    //qDebug() << " - Not Allowed";
    //auth->setAllowed(false);
  //}
	
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
  for(int i=0; i<OpenSockets.length(); i++){
    if(OpenSockets[i]->ID()==ID){ delete OpenSockets.takeAt(i); break; }
  }
  QTimer::singleShot(0,this, SLOT(NewSocketConnection()) ); //check for a new connection
}

void WebServer::WatcherUpdate(QString path){
  if(path==APPCAFEWORKING){
    //Read the file contents
    QString stat = readFile(APPCAFEWORKING);
    if(stat.simplified().isEmpty()){ stat = "idle"; }
    qDebug() << "Dispatcher Update:" << stat;
    lastDispatch = stat; //save for later
    //Forward those contents on to the currently-open sockets
    emit DispatchStatusUpdate(stat);
  }
  //Make sure this file/dir is not removed from the watcher
  if(!watcher->files().contains(path) && !watcher->directories().contains(path)){
    watcher->addPath(path); //re-add it to the watcher. This happens when the file is removed/re-created instead of just overwritten
  }
}