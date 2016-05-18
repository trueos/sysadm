// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "WebSocket.h"

#include <QtConcurrent>
#include <unistd.h>

#define DEBUG 0
#define IDLETIMEOUTMINS 30

WebSocket::WebSocket(QWebSocket *sock, QString ID, AuthorizationManager *auth){
  SockID = ID;
  isBridge = false;
  SockAuthToken.clear(); //nothing set initially
  SOCKET = sock;
  TSOCKET = 0;
  AUTHSYSTEM = auth;
  SockPeerIP = SOCKET->peerAddress().toString();
  LogManager::log(LogManager::HOST,"New Connection: "+SockPeerIP);
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(SOCKET, SIGNAL(textMessageReceived(const QString&)), this, SLOT(EvaluateMessage(const QString&)) );
  connect(SOCKET, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(EvaluateMessage(const QByteArray&)) );
  connect(SOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  connect(EVENTS, SIGNAL(NewEvent(EventWatcher::EVENT_TYPE, QJsonValue)), this, SLOT(EventUpdate(EventWatcher::EVENT_TYPE, QJsonValue)) );
  connect(this, SIGNAL(SendMessage(QString)), this, SLOT(sendReply(QString)) );
  idletimer->start();
  QTimer::singleShot(30000, this, SLOT(checkAuth()));
}

WebSocket::WebSocket(QSslSocket *sock, QString ID, AuthorizationManager *auth){
  SockID = ID;
  SockAuthToken.clear(); //nothing set initially
  TSOCKET = sock;
  SOCKET = 0;
  SockPeerIP = TSOCKET->peerAddress().toString();
  LogManager::log(LogManager::HOST,"New Connection: "+SockPeerIP);
  AUTHSYSTEM = auth;
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(TSOCKET, SIGNAL(readyRead()), this, SLOT(EvaluateTcpMessage()) );
  connect(TSOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  connect(TSOCKET, SIGNAL(encrypted()), this, SLOT(nowEncrypted()) );
  connect(TSOCKET, SIGNAL(peerVerifyError(const QSslError &)), this, SLOT(peerError(const QSslError &)) );
  connect(TSOCKET, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(SslError(const QList<QSslError> &)) );
  connect(EVENTS, SIGNAL(NewEvent(EventWatcher::EVENT_TYPE, QJsonValue)), this, SLOT(EventUpdate(EventWatcher::EVENT_TYPE, QJsonValue)) );
  connect(this, SIGNAL(SendMessage(QString)), this, SLOT(sendReply(QString)) );
  //qDebug() << " - Starting Server Encryption Handshake";
   TSOCKET->startServerEncryption();
  //qDebug() << " - Socket Encrypted:" << TSOCKET->isEncrypted();
  idletimer->start();
  QTimer::singleShot(30000, this, SLOT(checkAuth()));
}

WebSocket::WebSocket(QString url, QString ID, AuthorizationManager *auth){
  //sets up a bridge connection (websocket only)
  SockID = ID;
  isBridge = true;
  SockAuthToken.clear(); //nothing set initially
  SOCKET = new QWebSocket("sysadm-server", QWebSocketProtocol::VersionLatest, this);
  TSOCKET = 0;
  AUTHSYSTEM = auth;
  SockPeerIP = SOCKET->peerAddress().toString();
  LogManager::log(LogManager::HOST,"New Connection: "+SockPeerIP);
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(SOCKET, SIGNAL(textMessageReceived(const QString&)), this, SLOT(EvaluateMessage(const QString&)) );
  connect(SOCKET, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(EvaluateMessage(const QByteArray&)) );
  connect(SOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  connect(EVENTS, SIGNAL(NewEvent(EventWatcher::EVENT_TYPE, QJsonValue)), this, SLOT(EventUpdate(EventWatcher::EVENT_TYPE, QJsonValue)) );
  connect(this, SIGNAL(SendMessage(QString)), this, SLOT(sendReply(QString)) );
  connect(SOCKET, SIGNAL(connected()), this, SLOT(startBridgeAuth()) );
  //idletimer->start(); //do not idle out on a bridge connection
  /*QTimer::singleShot(30000, this, SLOT(checkAuth()));*/
  //Assemble the URL as needed
  if(!url.startsWith("wss://")){ url.prepend("wss://"); }
  bool hasport = false;
  url.section(":",-1).toInt(&hasport); //check if the last piece of the url is a valid number
  if(!hasport){ url.append(":"+QString::number(BRIDGEPORTNUMBER)); }
  SOCKET->open(QUrl(url));
}

WebSocket::~WebSocket(){
  //qDebug() << "SOCKET Destroyed";
  if(SOCKET!=0 && SOCKET->isValid()){
    SOCKET->close();
    delete SOCKET;
  }
  if(TSOCKET!=0 && TSOCKET->isValid()){
    TSOCKET->close();
    delete TSOCKET;
  }
}


QString WebSocket::ID(){
  return SockID;
}

void WebSocket::closeConnection(){
  if(SOCKET!=0 && SOCKET->isValid()){
    SOCKET->close();
  }
  if(TSOCKET!=0 && TSOCKET->isValid()){
    TSOCKET->close();
  }
}

//=======================
//             PRIVATE
//=======================
void WebSocket::sendReply(QString msg){
  //qDebug() << "Sending Socket Reply:" << msg;
 if(SOCKET!=0 && SOCKET->isValid()){ SOCKET->sendTextMessage(msg); } //Websocket connection
 else if(TSOCKET!=0 && TSOCKET->isValid()){ 
    //TCP Socket connection
    TSOCKET->write(msg.toUtf8().data()); 
    TSOCKET->disconnectFromHost(); //TCP/REST connections are 1 connection per message.
 }
}

void WebSocket::EvaluateREST(QString msg){
  //Parse the message into it's elements and proceed to the main data evaluation
  RestInputStruct IN(msg, TSOCKET!=0);	
  if(SOCKET!=0 && !IN.Header.isEmpty()){
    //Bridge-relay message - need to decrypt the message body before it can be parsed
    //IN.Body = AUTHSYSTEM->decryptString(IN.Body, key); //TO-DO
    IN.ParseBodyIntoJson();
  }
  if(DEBUG){
    qDebug() << "New REST Message:";
    qDebug() << "  VERB:" << IN.VERB << "URI:" << IN.URI;
    qDebug() << "  HEADERS:" << IN.Header;
    qDebug() << "  BODY:" << IN.Body;
    //qDebug() << " Auth:" << IN.auth;
    qDebug() << "JSON Values:";
    qDebug() << " - Name:" << IN.name;
    qDebug() << " - Namespace:" << IN.namesp;
    qDebug() << " - ID:" << IN.id;
    qDebug() << " - Has Args:" << !IN.args.isNull();
  }
  //Now check for the REST-specific verbs/actions
  if(IN.VERB == "OPTIONS" || IN.VERB == "HEAD"){
    RestOutputStruct out;	  
      out.in_struct = IN;
      out.CODE = RestOutputStruct::OK;
      if(IN.VERB=="HEAD"){
	
      }else{ //OPTIONS
	out.Header << "Allow: HEAD, GET";
	out.Header << "Hosts: /syscache";	      
      }
      out.Header << "Accept: text/json";
      out.Header << "Content-Type: text/json; charset=utf-8";
    this->sendReply(out.assembleMessage());
  }else{
    if(IN.name.startsWith("auth") || (IN.namesp.toLower()=="rpc" && IN.name.toLower()=="identify") ){
      //Keep auth/pre-auth system requests in order
      EvaluateRequest(IN);
    }else if(isBridge && (IN.name=="response" || (IN.namesp=="events" && IN.name=="bridge") ) ){
      EvaluateResponse(IN);
    }else{
      QtConcurrent::run(this, &WebSocket::EvaluateRequest, IN);
    }
  }
}

void WebSocket::EvaluateRequest(const RestInputStruct &REQ){
  qDebug() << "Evaluate Request:" << REQ.namesp << REQ.name << REQ.args;
  RestOutputStruct out;
    out.in_struct = REQ;
  QHostAddress host;
    if(SOCKET!=0 && SOCKET->isValid()){ host = SOCKET->peerAddress(); }
    else if(TSOCKET!=0 && TSOCKET->isValid()){ host = TSOCKET->peerAddress(); }
  QString cur_auth_tok = SockAuthToken;
  if(!REQ.bridgeID.isEmpty() || isBridge){ //never clear/check the SockAuthToken itself on a bridge - this was assigned by the bridge and not created here
    cur_auth_tok.clear();
    if(BRIDGE.contains(REQ.bridgeID)){ cur_auth_tok = BRIDGE[REQ.bridgeID].auth_tok; }
  }
  if(!REQ.VERB.isEmpty() && REQ.VERB != "GET" && REQ.VERB!="POST" && REQ.VERB!="PUT"){
    //Non-supported request (at the moment) - return an error message
    out.CODE = RestOutputStruct::BADREQUEST;
  }else if(out.in_struct.name.isEmpty() || out.in_struct.namesp.isEmpty() ){
    //Invalid JSON structure validity
    //Note: id and args are optional at this stage - let the subsystems handle those inputs
    out.CODE = RestOutputStruct::BADREQUEST;
  }else{
    //First check for a REST authorization (not stand-alone request)
    if(!out.in_struct.auth.isEmpty()){
      AUTHSYSTEM->clearAuth(SockAuthToken); //new auth requested - clear any old token
      SockAuthToken = AUTHSYSTEM->LoginUP(host, out.in_struct.auth.section(":",0,0), out.in_struct.auth.section(":",1,1));
    }
  //qDebug() << "Auth Token:" << cur_auth_tok;
    //Now check the body of the message and do what it needs
if(out.in_struct.namesp.toLower() == "rpc"){
  if(out.in_struct.name == "identify"){
    QJsonObject obj;
      obj.insert("type", "server");
    out.out_args = obj;
    out.CODE = RestOutputStruct::OK;
  }else if(out.in_struct.name.startsWith("auth")){
    //Now perform authentication based on type of auth given
    //Note: This sets/changes the current SockAuthToken
   AUTHSYSTEM->clearAuth(cur_auth_tok);  //new auth requested - clear any old token

    if(DEBUG){ qDebug() << "Authenticate Peer:" << host; }
    //Now do the auth
    if(out.in_struct.name=="auth" && out.in_struct.args.isObject() && !isBridge ){
	    //username/[password/cert] authentication
	    QString user, pass;
	    if(out.in_struct.args.toObject().contains("username")){ user = JsonValueToString(out.in_struct.args.toObject().value("username"));  }
	    if(out.in_struct.args.toObject().contains("password")){ pass = JsonValueToString(out.in_struct.args.toObject().value("password"));  }
	    
	      //Use the given password
	      cur_auth_tok = AUTHSYSTEM->LoginUP(host, user, pass);
    }else if(out.in_struct.name=="auth_ssl"){
      if(out.in_struct.args.isObject() && out.in_struct.args.toObject().contains("encrypted_string")){
        //Stage 2: Check the returned encrypted/string
	cur_auth_tok = AUTHSYSTEM->LoginUC(host, JsonValueToString(out.in_struct.args.toObject().value("encrypted_string")) );
      }else{
        //Stage 1: Send the client a random string to encrypt with their SSL key
        QString key = AUTHSYSTEM->GenerateEncCheckString();
        QJsonObject obj; obj.insert("test_string", key);
	out.out_args = obj;
        out.CODE = RestOutputStruct::OK;
        QString msg = out.assembleMessage();
        if(SOCKET!=0 && !REQ.bridgeID.isEmpty()){
          //BRIDGE RELAY - alternate format
          //Need to encrypt the message for output (TO-DO)
         QString key = BRIDGE[REQ.bridgeID].enc_key;
         if(!key.isEmpty()){ msg = AUTHSYSTEM->encryptString(msg, key); }
          //Now add the destination ID
          msg.prepend( REQ.bridgeID+"\n");
        }
	this->sendReply(msg);
	return;
      }
    }else if(out.in_struct.name == "auth_token" && out.in_struct.args.isObject()  && !isBridge){
       cur_auth_tok = JsonValueToString(out.in_struct.args.toObject().value("token"));
    }else if(out.in_struct.name == "auth_clear"){
       return; //don't send a return message after clearing an auth (already done)
    }
	  
	  //Now check the auth and respond appropriately
	  if(AUTHSYSTEM->checkAuth(cur_auth_tok)){
	    //Good Authentication - return the new token 
	    QJsonArray array;
	      array.append(cur_auth_tok);
	      array.append(AUTHSYSTEM->checkAuthTimeoutSecs(cur_auth_tok));
	    out.out_args = array;
	    out.CODE = RestOutputStruct::OK;
	    if(REQ.bridgeID.isEmpty()){ SockAuthToken = cur_auth_tok; }
            else{ BRIDGE[REQ.bridgeID].auth_tok = cur_auth_tok; }
	  }else{
	    if(SockAuthToken=="REFUSED"){
	      out.CODE = RestOutputStruct::FORBIDDEN;
	    }
	    //Bad Authentication - return error
	      out.CODE = RestOutputStruct::UNAUTHORIZED;
	  }
		
	}else if( AUTHSYSTEM->checkAuth(cur_auth_tok) ){ //validate current Authentication token	 
	  //Now provide access to the various subsystems
	  // First get/set the permissions flag into the input structure
	  out.in_struct.fullaccess = AUTHSYSTEM->hasFullAccess(cur_auth_tok);
	  //Pre-set any output fields
    QJsonObject outargs;	
	    out.CODE = EvaluateBackendRequest(out.in_struct, &outargs);
      out.out_args = outargs;	  
  }else{
	  //Bad/No authentication
	  out.CODE = RestOutputStruct::UNAUTHORIZED;
	}
	    	
}else if(out.in_struct.namesp.toLower() == "events"){
	//qDebug() << "Got Event subsytem request" << out.in_struct.args;
          if( AUTHSYSTEM->checkAuth(cur_auth_tok) ){ //validate current Authentication token	 
	    //Pre-set any output fields
            QJsonObject outargs;	
	    //Assemble the list of input events
	    QStringList evlist;
	    if(out.in_struct.args.isString()){ evlist << JsonValueToString(out.in_struct.args); }
	    else if(out.in_struct.args.isArray()){ evlist = JsonArrayToStringList(out.in_struct.args.toArray()); }
	    //Now subscribe/unsubscribe to these events
	    int sub = -1; //bad input
	    if(out.in_struct.name=="subscribe"){ sub = 1; }
	    else if(out.in_struct.name=="unsubscribe"){ sub = 0; }
	    //qDebug() << "Got Client Event Modification:" << sub << evlist;
	    if(sub>=0 && !evlist.isEmpty() ){
	      for(int i=0; i<evlist.length(); i++){
	        EventWatcher::EVENT_TYPE type = EventWatcher::typeFromString(evlist[i]);
		//qDebug() << " - type:" << type;
		if(isBridge){ 
                  ForwardEvents.clear();
                  if(!REQ.bridgeID.isEmpty()){ ForwardEvents = BRIDGE[REQ.bridgeID].sendEvents; }
                }
		if(type==EventWatcher::BADEVENT){ continue; }
		outargs.insert(out.in_struct.name,QJsonValue(evlist[i]));
		if(sub==1){ 
		  ForwardEvents << type; 
		  EventUpdate(type);
		}else{
		  ForwardEvents.removeAll(type);
		}
                if(isBridge && !REQ.bridgeID.isEmpty()){ BRIDGE[REQ.bridgeID].sendEvents = ForwardEvents; }
	      }
	      out.out_args = outargs;
	      out.CODE = RestOutputStruct::OK;
	    }else{
	      //Bad/No authentication
	      out.CODE = RestOutputStruct::BADREQUEST;		    
	    }
          }else{
	    //Bad/No authentication
	    out.CODE = RestOutputStruct::UNAUTHORIZED;
	  }
	//Other namespace - check whether auth has already been established before continuing
}else if( AUTHSYSTEM->checkAuth(cur_auth_tok) ){ //validate current Authentication token	 
	  //Now provide access to the various subsystems
	  // First get/set the permissions flag into the input structure
	  out.in_struct.fullaccess = AUTHSYSTEM->hasFullAccess(cur_auth_tok);
	  //Pre-set any output fields
          QJsonObject outargs;	
	    out.CODE = EvaluateBackendRequest(out.in_struct, &outargs);
            out.out_args = outargs;
}else{
	  //Error in inputs - assemble the return error message
	  out.CODE = RestOutputStruct::UNAUTHORIZED;
}

    //If this is a REST input - go ahead and format the output header
    if(out.CODE == RestOutputStruct::OK){
      out.Header << "Content-Type: text/json; charset=utf-8";
    }
  }
  //Return any information
  QString msg = out.assembleMessage();
  if(SOCKET!=0 && !REQ.bridgeID.isEmpty()){
    //BRIDGE RELAY - alternate format
    //Need to encrypt the message for output (TO-DO)
    QString key = BRIDGE[REQ.bridgeID].enc_key;
    if(!key.isEmpty()){ msg = AUTHSYSTEM->encryptString(msg, key); }
    //Now add the destination ID
    msg.prepend( REQ.bridgeID+"\n");
  }
  if(out.CODE == RestOutputStruct::FORBIDDEN && SOCKET!=0 && SOCKET->isValid()){
    this->sendReply(msg);
    SOCKET->close(QWebSocketProtocol::CloseCodeNormal, "Too Many Authorization Failures - Try again later");
  }else{
    this->emit SendMessage(msg);	  
  }
}

void WebSocket::EvaluateResponse(const RestInputStruct& IN){
  if(!isBridge){ return; } //this is only valid for bridge connections
  if(IN.namesp=="events" && IN.name=="bridge"){
    QStringList bids = JsonArrayToStringList(IN.args.toObject().value("available_connections").toArray());
    QStringList keys = BRIDGE.keys();
    for(int i=0; i<keys.length(); i++){
      if(bids.contains(keys[i])){ bids.removeAll(keys[i]);  } //already handled
      else{ AUTHSYSTEM->clearAuth(BRIDGE[keys[i]].auth_tok); BRIDGE.remove(keys[i]); } //no longer available
    }
    //Now add any new bridge ID's to the hash
    for(int i=0; i<bids.length(); i++){
      BRIDGE.insert(bids[i], bridge_data());
    }

  }else if(IN.namesp=="rpc" && IN.id=="server_to_bridge_auth"){
    if(IN.args.isArray()){
      //Auth Successful - save auth token
      QStringList arr = JsonArrayToStringList(IN.args.toArray());
      if(arr.length()==2){
        SockAuthToken = arr.first();
      }
    }else if(IN.args.isObject()){
      //Stage 2 auth - encrypt string and return it
      //Get the test string and encode it
      QString test = IN.args.toObject().value("test_string").toString();
      QString enc_test = AUTHSYSTEM->GenerateEncString_bridge(test);
      //Now send the string back
      QJsonObject obj, args;
        obj.insert("id","server_to_bridge_auth");
        obj.insert("namespace","rpc");
        obj.insert("name","auth_ssl");
        args.insert("encrypted_string", enc_test);
        obj.insert("args", args);
      this->sendReply( QJsonDocument(obj).toJson(QJsonDocument::Compact) );
    }
  }
}
// === GENERAL PURPOSE UTILITY FUNCTIONS ===
QString WebSocket::JsonValueToString(QJsonValue val){
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

QStringList WebSocket::JsonArrayToStringList(QJsonArray array){
  //Note: This assumes that the array is only values, not additional objects
  QStringList out;
  if(DEBUG){ qDebug() << "Array to List:" << array.count(); }
  for(int i=0; i<array.count(); i++){
    out << JsonValueToString(array.at(i));
  }
  return out;  
}

// =====================
//       PRIVATE SLOTS
// =====================
void WebSocket::checkIdle(){
  if(SOCKET !=0 && SOCKET->isValid()){
    LogManager::log(LogManager::HOST,"Connection Idle: "+SockPeerIP);
    SOCKET->close(); //timeout - close the connection to make way for others
  }
  else if(TSOCKET !=0 && TSOCKET->isValid() ){
    LogManager::log(LogManager::HOST,"Connection Idle: "+SockPeerIP);
    TSOCKET->close(); //timeout - close the connection to make way for others
  }
}

void WebSocket::checkAuth(){
  if(!AUTHSYSTEM->checkAuth(SockAuthToken)){
    //Still not authorized - disconnect
    checkIdle();
  }
}

void WebSocket::SocketClosing(){
  LogManager::log(LogManager::HOST,"Connection Closing: "+SockPeerIP);
  if(idletimer->isActive()){ 
    //This means the client deliberately closed the connection - not the idle timer
    //qDebug() << " - Client Closed Connection";
    idletimer->stop(); 
  }else{
    //qDebug() << "idleTimer not running";
  }
  //Stop any current requests

  //Reset the pointer
  if(SOCKET!=0){ SOCKET = 0;	 }
  if(TSOCKET!=0){ TSOCKET = 0; }
  
  emit SocketClosed(SockID);
}

void WebSocket::EvaluateMessage(const QByteArray &msg){
  //qDebug() << "New Binary Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  idletimer->start(); 
  EvaluateREST( QString(msg) );
  //qDebug() << " - Done with Binary Message";
}

void WebSocket::EvaluateMessage(const QString &msg){ 
  //qDebug() << "New Text Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  idletimer->start(); 
  EvaluateREST(msg); 
  //qDebug() << " - Done with Text Message";
}

void WebSocket::ParseIncoming(){
  bool found = false;

  // Check if we have a complete JSON request waiting to be parsed
  QString JsonRequest;
  for ( int i = 0; i<incomingbuffer.size(); i++) {
    if(incomingbuffer[i]!='}'){ continue; } //only look for the end of a JSON block
    JsonRequest = incomingbuffer.left(i+1);
    //REST requests will have non-JSON at the top - trim that off
    JsonRequest = "{"+JsonRequest.section("{",1,-1);
    //JsonRequest.truncate(i);
    //qDebug() << "Check JSON:" << JsonRequest;
    if ( !QJsonDocument::fromJson(JsonRequest.toLocal8Bit()).isNull() )
    {
      // We have what looks like a valid JSON request, lets parse it now
      //qDebug() << "TCP Message" << JsonRequest;
      EvaluateREST(incomingbuffer.left(i+1)); //send the full message
      incomingbuffer.remove(0, i);
      found = true;
      break;
    }
  }
  //if(!found){ qDebug() << "Bad Message:" << incomingbuffer; }
  // If the buffer is larger than 128000 chars and no valid JSON somebody
  // is screwing with us, lets clear the buffer
  if ( ! found && incomingbuffer.size() > 128000 ) {
    incomingbuffer="";
    return;
  }

  // If we found a valid JSON request, but still have data, check for
  // a second request waiting in the buffer
  if ( found && incomingbuffer.size() > 2 )
    ParseIncoming();
}

void WebSocket::EvaluateTcpMessage(){
  //Need to read the data from the Tcp socket and turn it into a string
  //qDebug() << "New TCP Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  incomingbuffer.append(QString(TSOCKET->read(128000)));

  // Check for JSON in this incoming data
  ParseIncoming();

  idletimer->start(); 
  //qDebug() << " - Done with TCP Message";	
}

//SSL signal handling
void WebSocket::nowEncrypted(){
  //the socket/connection is now encrypted
  //qDebug() << " - Socket now encrypted";
}

void WebSocket::peerError(const QSslError&){ //peerVerifyError() signal
  //qDebug() << "Socket Peer Error:";
}

void WebSocket::SslError(const QList<QSslError> &err){ //sslErrors() signal
  LogManager::log(LogManager::HOST,"Connection SSL Errors ["+SockPeerIP+"]: "+err.length());
}

void WebSocket::startBridgeAuth(){
  QJsonObject obj;
  obj.insert("id", "server_to_bridge_auth");
  obj.insert("namespace","rpc");
  obj.insert("name","auth_ssl");
  obj.insert("args","");

  this->sendReply( QJsonDocument(obj).toJson(QJsonDocument::Compact) );
}

// ======================
//       PUBLIC SLOTS
// ======================
void WebSocket::EventUpdate(EventWatcher::EVENT_TYPE evtype, QJsonValue msg){
  //qDebug() << "Got Socket Event Update:" << msg;
  if(msg.isNull()){ msg = EVENTS->lastEvent(evtype); }
  if(msg.isNull()){ return; } //nothing to send
  if( !ForwardEvents.contains(evtype) && !isBridge ){ return; }
  RestOutputStruct out;
    out.CODE = RestOutputStruct::OK;
    out.in_struct.namesp = "events";
    out.out_args = msg;
    //out.Header << "Content-Type: text/json; charset=utf-8"; //REST header info
    out.in_struct.name = EventWatcher::typeToString(evtype);
  //qDebug() << "Send Event:" << out.assembleMessage();
  if(isBridge){
    QString raw = out.assembleMessage();
    QStringList conns = BRIDGE.keys();
    for(int i=0; i<conns.length(); i++){
      if( !BRIDGE[conns[i]].sendEvents.contains(evtype) ){ continue; }
      //Encrypt the data with the proper key
      QString key = BRIDGE[conns[i]].enc_key;
      QString enc_data = raw;
      if(!key.isEmpty()){ enc_data = AUTHSYSTEM->encryptString(raw, key); }
      //Now add the destination ID
      enc_data.prepend( conns[i]+"\n");
      this->emit SendMessage(enc_data);
    }
  }else{
    //NON-BRIDGE: Now send the message back through the socket
    this->emit SendMessage(out.assembleMessage());
  }
}
