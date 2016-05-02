// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#include "BridgeConnection.h"

#include <unistd.h>

#define DEBUG 0
#define IDLETIMEOUTMINS 30

BridgeConnection::BridgeConnection(QWebSocket *sock, QString ID){
  SockID = ID;
  SockAuthToken.clear(); //nothing set initially
  SOCKET = sock;
  SockPeerIP = SOCKET->peerAddress().toString();
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(SOCKET, SIGNAL(textMessageReceived(const QString&)), this, SLOT(EvaluateMessage(const QString&)) );
  connect(SOCKET, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(EvaluateMessage(const QByteArray&)) );
  connect(SOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  idletimer->start();
  QTimer::singleShot(30000, this, SLOT(checkAuth()));
}

BridgeConnection::~BridgeConnection(){
  //qDebug() << "SOCKET Destroyed";
  if(SOCKET!=0 && SOCKET->isValid()){
    SOCKET->close();
    delete SOCKET;
  }
}

QString BridgeConnection::ID(){
  return SockID;
}

void BridgeConnection::forwardMessage(QString msg){
  //qDebug() << "Sending Socket Reply:" << msg;
 if(SOCKET!=0 && SOCKET->isValid()){ SOCKET->sendTextMessage(msg); }
}
//=======================
//             PRIVATE
//=======================
// === GENERAL PURPOSE UTILITY FUNCTIONS ===
QString BridgeConnection::JsonValueToString(QJsonValue val){
  //Note: Do not use this on arrays - only use this on single-value values
  QString out;
  switch(val.type()){
    case QJsonValue::Bool:
	out = (val.toBool() ? "true": "false"); break;
    case QJsonValue::Double:
	out = QString::number(val.toDouble()); break;
    case QJsonValue::String:
	out = val.toString(); break;
    case QJsonValue::Array:
	out = "\""+JsonArrayToStringList(val.toArray()).join("\" \"")+"\"";
    default:
	out.clear();
  }
  return out;
}

QStringList BridgeConnection::JsonArrayToStringList(QJsonArray array){
  //Note: This assumes that the array is only values, not additional objects
  QStringList out;
  if(DEBUG){ qDebug() << "Array to List:" << array.count(); }
  for(int i=0; i<array.count(); i++){
    out << JsonValueToString(array.at(i));
  }
  return out;  
}

void BridgeConnection::InjectMessage(QString msg){
  //See if this message is directed to the bridge itself, or a client
  if(msg.startsWith("{")){

  }else{
    //Need to read the destination off the message first
    int lb = msg.indexOf("\n"); //line break
    QString toID = msg.left(lb);
    if(toID.contains(" ")){ 
      //Return an error message to the calling socket
      if(SOCKET->isValid()){ SOCKET->sendTextMessage("{\"namespace\":\"error\", \"name\":\"error\",\"id\":\"error\",\"args\":\"\" }"); }
    }else{
      //Add the "from" ID to the message
      msg = msg.replace(0,lb,SockID); //replace the "to" ID with the "from" ID
      emit SocketMessage(toID, msg); //forward the message on to the "to" connection
    }
  }
}

// =====================
//       PRIVATE SLOTS
// =====================
void BridgeConnection::checkIdle(){
  if(SOCKET !=0 && SOCKET->isValid()){
    qDebug() << "Connection Idle: "<<SockPeerIP;
    SOCKET->close(); //timeout - close the connection to make way for others
  }
}

void BridgeConnection::checkAuth(){
 //if(!AUTHSYSTEM->checkAuth(SockAuthToken)){
    //Still not authorized - disconnect
    //checkIdle();
  //}
}

void BridgeConnection::SocketClosing(){
  qDebug() << "Connection Closing: " << SockPeerIP;
  if(idletimer->isActive()){ 
    //This means the client deliberately closed the connection - not the idle timer
    //qDebug() << " - Client Closed Connection";
    idletimer->stop(); 
  }else{
    //qDebug() << "idleTimer not running";
  }
  //Stop any current requests

  //Reset the pointer
  if(SOCKET!=0){ SOCKET = 0;  }
  
  emit SocketClosed(SockID);
}

void BridgeConnection::EvaluateMessage(const QByteArray &msg){
  //qDebug() << "New Binary Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  idletimer->start(); 
  InjectMessage( QString(msg) );
  //qDebug() << " - Done with Binary Message";
}

void BridgeConnection::EvaluateMessage(const QString &msg){ 
  //qDebug() << "New Text Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  idletimer->start(); 
  InjectMessage(msg); 
  //qDebug() << " - Done with Text Message";
}

//SSL signal handling
void BridgeConnection::nowEncrypted(){
  //the socket/connection is now encrypted
  //qDebug() << " - Socket now encrypted";
}

void BridgeConnection::peerError(const QSslError&){ //peerVerifyError() signal
  //qDebug() << "Socket Peer Error:";
}

void BridgeConnection::SslError(const QList<QSslError> &err){ //sslErrors() signal
  qDebug() << "Connection SSL Errors ["+SockPeerIP+"]: "+err.length();
}

// ======================
//       PUBLIC SLOTS
// ======================