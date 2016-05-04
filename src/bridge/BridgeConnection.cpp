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
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkgonintendoIdle()) );
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
  if(msg.startsWith("{") || !AUTHSYSTEM->checkAuth(SockAuthToken) ){
    HandleAPIMessage(msg);
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

void BridgeConnection::HandleAPIMessage(QString msg){
  QJsonObject JM = QJsonDocument::fromJson(msg.toLocal8Bit()).object();
  QJsonObject out;
  if(JM.isEmpty() || !JM.contains("namespace") || !JM.contains("name") || !JM.contains("args") || !JM.contains("id") ){
    //invalid inputs - return 
    out.insert("namespace","error");
    out.insert("name","error");
    out.insert("id", JM.contains("id") ? JM.value("id") : "error");
    out.insert("args", "");
  }else if( JM.value("name").toString()=="response" ){
    // - Return messages first (check ID)
      QString id = JM.value("id").toString();
      if(id=="sysadm_bridge_request_ident"){
        serverconn = (JM.value("args").toObject().value("type").toString() == "server");
      }
     //no response needed
  }else{
    //API Call
    QString name, namesp, id; 
    QJsonObject args = JM.value("args").toObject();
    name = JM.value("name").toString();
    namesp = JM.value("namespace").toString();
    out.insert("id", JM.value("id"));
    out.insert("namespace", namesp);
    out.insert("name","reponse");
    QJsonValue outargs;
    //There is only a short list of API calls the bridge is capable of:
    if(namesp == "rpc" && name=="identify"){
      QJsonObject tmp;
        tmp.insert("type","bridge");
      outargs = tmp;

    }else if(namesp == "rpc" && name=="auth_ssl"){
      if(!args.contains("encrypted_string")){
        //Stage 1 - send a random string to encrypt
        QString key = AUTHSYSTEM->GenerateEncCheckString();
        QJsonObject obj; obj.insert("test_string", key);
	outargs = obj;
      }else{
        //Stage 2 - verify returned encrypted string
        SockAuthToken = AUTHSYSTEM->LoginUC(SOCKET->peerAddress(),args.value("encrypted_string").toString() );
        if(AUTHSYSTEM->checkAuth(SockAuthToken)){
          QJsonArray array;
	    array.append(SockAuthToken);
	    array.append(AUTHSYSTEM->checkAuthTimeoutSecs(SockAuthToken));
	  outargs = array;
        }else{
          out.insert("name","error");
          outargs = "unauthorized";
        }
      }
    }else if(AUTHSYSTEM->checkAuth(SockAuthToken)){
      //Valid auth - a couple more API calls available here

    }else{
      out.insert("name","error"); //unknown API call
    }
    out.insert("args",outargs);
    SOCKET->sendTextMessage( QJsonDocument(out).toJson(QJsonDocument::Compact) );
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
 if(!AUTHSYSTEM->checkAuth(SockAuthToken)){
    //Still not authorized - disconnect
    checkIdle();
  }
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
