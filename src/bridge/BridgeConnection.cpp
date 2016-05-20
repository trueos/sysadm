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
  qDebug() << "New Connection:" << SockPeerIP;
  idletimer = new QTimer(this);
    idletimer->setInterval(30000); //connection timout for idle sockets
    //idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkAuth()) );
  connect(SOCKET, SIGNAL(textMessageReceived(const QString&)), this, SLOT(EvaluateMessage(const QString&)) );
  connect(SOCKET, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(EvaluateMessage(const QByteArray&)) );
  connect(SOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  idletimer->start();
  requestIdentify();
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

bool BridgeConnection::isServer(){
  return serverconn;
}

QStringList BridgeConnection::validKeySums(){
  return knownkeys;
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
  qDebug() << "Got Message:" << msg;
  if(msg.startsWith("{") || !AUTHSYSTEM->checkAuth(SockAuthToken) ){
    HandleAPIMessage(msg);
  }else{
    //Need to read the destination off the message first
    int lb = msg.indexOf("\n"); //line break
    QString toID = msg.left(lb);
    if(toID.contains(" ")){ 
      //Return an error message to the calling socket
      qDebug() << "Invalid Destination ID:" << toID;
      //if(SOCKET->isValid()){ SOCKET->sendTextMessage("{\"namespace\":\"error\", \"name\":\"error\",\"id\":\"error\",\"args\":\"\" }"); }
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
    /*out.insert("namespace","error");
    out.insert("name","error");
    out.insert("id", JM.contains("id") ? JM.value("id") : "error");
    out.insert("args", "");*/
  }else if( JM.value("name").toString()=="response" ){
    // - Return messages first (check ID)
      QString id = JM.value("id").toString();
      if(id=="sysadm_bridge_request_ident"){
        qDebug() << "Got ident reply:" << JM;
        serverconn = (JM.value("args").toObject().value("type").toString() == "server");
      }else if("bridge_request_list_keys"){
        QStringList keys = JsonArrayToStringList(JM.value("args").toObject().value("md5_keys").toArray());
        //Now see what has changed (if anything)
        if(keys!=knownkeys){
          knownkeys = keys;
          emit keysChanged(SockID, serverconn, knownkeys);
        }
      }
     //no response needed
  }else if(JM.value("name").toString()!="error"){
    //API Call
    QString name, namesp, id; 
    QJsonObject args = JM.value("args").toObject();
    name = JM.value("name").toString();
    namesp = JM.value("namespace").toString();
    out.insert("id", JM.value("id"));
    out.insert("namespace", namesp);
    out.insert("name","response");
    QJsonValue outargs;
    //There is only a short list of API calls the bridge is capable of:
    if(namesp == "rpc" && name=="identify"){
      QJsonObject tmp;
        tmp.insert("type","bridge");
      outargs = tmp;

    }else if(namesp == "rpc" && name=="auth_ssl"){
      if(!args.contains("encrypted_string")){
        //Stage 1 - send a random string to encrypt
        qDebug() << "Connection Auth Init:" << SockID;
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
          qDebug() << "Connection Authorized:" << SockPeerIP << SockID << "Type:" << (serverconn ? "server" : "client");
          QTimer::singleShot(10 ,this, SLOT(requestKeyList()) );
        }else{
          out.insert("name","error");
          outargs = "unauthorized";
          qDebug() << "Connection Not Authorized:" << SockPeerIP << SockID;
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
  if(SOCKET !=0){
    if(SOCKET->isValid()){
      qDebug() << "Connection Idle: "<< SockID;
      SOCKET->close(); //timeout - close the connection to make way for others
    }else{
      emit SocketClosed(SockID); //disconnect signal did not get picked up for some reason
    }
  } 
}

void BridgeConnection::checkAuth(){
 if(!AUTHSYSTEM->checkAuth(SockAuthToken) || !SOCKET->isValid() ){
    //Still not authorized - disconnect
    checkIdle();
  }
}

void BridgeConnection::SocketClosing(){
  qDebug() << "Connection Closing: " << SockID;
  if(idletimer->isActive()){ idletimer->stop(); }
  //Stop any current requests

  //Reset the pointer
  if(SOCKET!=0){ SOCKET = 0;  }
  
  emit SocketClosed(SockID);
}

void BridgeConnection::EvaluateMessage(const QByteArray &msg){
  //qDebug() << "New Binary Message:";
  InjectMessage( QString(msg) );
  //qDebug() << " - Done with Binary Message";
}

void BridgeConnection::EvaluateMessage(const QString &msg){ 
  //qDebug() << "New Text Message:";
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
void BridgeConnection::requestIdentify(){
 QJsonObject obj;
    obj.insert("id","sysadm_bridge_request_ident");
    obj.insert("namespace","rpc");
    obj.insert("name","identify");
    obj.insert("args","");

  SOCKET->sendTextMessage( QJsonDocument(obj).toJson(QJsonDocument::Compact) );
}

void BridgeConnection::requestKeyList(){
  if(!AUTHSYSTEM->checkAuth(SockAuthToken)){ return; } //not authorized yet
  QJsonObject obj;
    obj.insert("id","bridge_request_list_keys");
    obj.insert("namespace","rpc");
    obj.insert("name","settings");
    QJsonObject args;
      args.insert("action","list_ssl_checksums");
    obj.insert("args",args);
  qDebug() << "Request Key List";
  SOCKET->sendTextMessage( QJsonDocument(obj).toJson(QJsonDocument::Compact) );
}

void BridgeConnection::announceIDAvailability(QStringList IDs){
  if(!AUTHSYSTEM->checkAuth(SockAuthToken)){ return; } //not authorized yet
  if(lastKnownConnections == IDs){ return; } //don't announce changes when nothing changed
  lastKnownConnections = IDs; //save for comparison later
  QJsonObject obj;
    obj.insert("id","");
    obj.insert("namespace","events");
    obj.insert("name","bridge");
    QJsonObject args;
      args.insert("available_connections",QJsonArray::fromStringList(IDs));
    obj.insert("args",args);

  SOCKET->sendTextMessage( QJsonDocument(obj).toJson(QJsonDocument::Compact) );
}
