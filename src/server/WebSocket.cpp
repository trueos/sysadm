// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "WebSocket.h"

#define DEBUG 1
#define IDLETIMEOUTMINS 30

WebSocket::WebSocket(QWebSocket *sock, QString ID, AuthorizationManager *auth){
  SockID = ID;
  SockAuthToken.clear(); //nothing set initially
  SOCKET = sock;
  TSOCKET = 0;
  SendAppCafeEvents = false;
  AUTHSYSTEM = auth;
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(SOCKET, SIGNAL(textMessageReceived(const QString&)), this, SLOT(EvaluateMessage(const QString&)) );
  connect(SOCKET, SIGNAL(binaryMessageReceived(const QByteArray&)), this, SLOT(EvaluateMessage(const QByteArray&)) );
  connect(SOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  idletimer->start();
}

WebSocket::WebSocket(QTcpSocket *sock, QString ID, AuthorizationManager *auth){
  SockID = ID;
  SockAuthToken.clear(); //nothing set initially
  TSOCKET = sock;
  SOCKET = 0;
  SendAppCafeEvents = false;
  AUTHSYSTEM = auth;
  idletimer = new QTimer(this);
    idletimer->setInterval(IDLETIMEOUTMINS*60000); //connection timout for idle sockets
    idletimer->setSingleShot(true);
  connect(idletimer, SIGNAL(timeout()), this, SLOT(checkIdle()) );
  connect(TSOCKET, SIGNAL(readyRead()), this, SLOT(EvaluateTCPMessage()) );
  connect(TSOCKET, SIGNAL(aboutToClose()), this, SLOT(SocketClosing()) );
  idletimer->start();
}

WebSocket::~WebSocket(){
  if(SOCKET!=0){
    SOCKET->close();
    delete SOCKET;
  }
  if(TSOCKET!=0){
    TSOCKET->close();
    delete TSOCKET;
  }
}


QString WebSocket::ID(){
  return SockID;
}

void WebSocket::setLastDispatch(QString msg){ 
  //used on initialization only
  lastDispatchEvent = msg;
}

//=======================
//             PRIVATE
//=======================
void WebSocket::EvaluateREST(QString msg){
  //Parse the message into it's elements and proceed to the main data evaluation
  RestInputStruct IN(msg);
  //NOTE: All the REST functionality is disabled for the moment, until we decide to turn it on again at a later time (just need websockets right now - not full REST)	

  if(DEBUG){
    qDebug() << "New REST Message:";
    qDebug() << "  VERB:" << IN.VERB << "URI:" << IN.URI;
    qDebug() << "  HEADERS:" << IN.Header;
    qDebug() << "  BODY:" << IN.Body;
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
    if(SOCKET!=0){ SOCKET->sendTextMessage(out.assembleMessage()); }
    else if(TSOCKET!=0){ TSOCKET->write(out.assembleMessage().toUtf8().data()); }
  }else{
    EvaluateRequest(IN);
  }
}

void WebSocket::EvaluateRequest(const RestInputStruct &REQ){
  RestOutputStruct out;
    out.in_struct = REQ;
  if(!REQ.VERB.isEmpty() && REQ.VERB != "GET"){
    //Non-supported request (at the moment) - return an error message
    out.CODE = RestOutputStruct::BADREQUEST;
  }else if(out.in_struct.name.isEmpty() || out.in_struct.namesp.isEmpty() ){
    //Invalid JSON structure validity
    //Note: id and args are optional at this stage - let the subsystems handle those inputs
    out.CODE = RestOutputStruct::BADREQUEST;
  }else{
    //Now check the body of the message and do what it needs
    /*QJsonDocument doc = QJsonDocument::fromJson(REQ.Body.toUtf8());
    if(doc.isNull()){ qWarning() << "Empty JSON Message Body!!" << REQ.Body.toUtf8(); }
    //Define the output structures
    QJsonObject ret; //return message

    //Objects contain other key/value pairs - this is 99% of cases
    if(doc.isObject()){
      //First check/set all the various required fields (both in and out)
      bool good = doc.object().contains("namespace") \
	    && doc.object().contains("name") \
	    && doc.object().contains("id") \
	    && doc.object().contains("args");
      //Can add some fallbacks for missing fields here - but not implemented yet
      */
      //parse the message and do something
      //if(good && (JsonValueToString(doc.object().value("namespace"))=="rpc") ){
      if(out.in_struct.namesp.toLower() == "rpc"){
	//Now fetch the outputs from the appropriate subsection
	//Note: Each subsection needs to set the "name", "namespace", and "args" output objects
	//QString name = JsonValueToString(doc.object().value("name")).toLower();
	//QJsonValue args = doc.object().value("args");
	if(out.in_struct.name.startsWith("auth")){
	  //Now perform authentication based on type of auth given
	  //Note: This sets/changes the current SockAuthToken
	  AUTHSYSTEM->clearAuth(SockAuthToken); //new auth requested - clear any old token
	  if(DEBUG){ qDebug() << "Authenticate Peer:" << SOCKET->peerAddress().toString(); }
	  bool localhost = (SOCKET->peerAddress() == QHostAddress::LocalHost) || (SOCKET->peerAddress() == QHostAddress::LocalHostIPv6);
	  //Now do the auth
	  if(out.in_struct.name=="auth" && out.in_struct.args.isObject() ){
	    //username/password authentication
	    QString user, pass;
	    if(out.in_struct.args.toObject().contains("username")){ user = JsonValueToString(out.in_struct.args.toObject().value("username"));  }
	    if(out.in_struct.args.toObject().contains("password")){ pass = JsonValueToString(out.in_struct.args.toObject().value("password"));  }
	    SockAuthToken = AUTHSYSTEM->LoginUP(localhost, user, pass);
	  }else if(out.in_struct.name == "auth_token" && out.in_struct.args.isObject()){
	    SockAuthToken = JsonValueToString(out.in_struct.args.toObject().value("token"));
	  }else if(out.in_struct.name == "auth_clear"){
	    return; //don't send a return message after clearing an auth (already done)
	  }
	  
	  //Now check the auth and respond appropriately
	  if(AUTHSYSTEM->checkAuth(SockAuthToken)){
	    //Good Authentication - return the new token 
	    //ret.insert("namespace", QJsonValue("rpc"));
	    //ret.insert("name", QJsonValue("response"));
	    //ret.insert("id", doc.object().value("id")); //use the same ID for the return message
	    QJsonArray array;
	      array.append(SockAuthToken);
	      array.append(AUTHSYSTEM->checkAuthTimeoutSecs(SockAuthToken));
	    out.out_args = array;
	    out.CODE = RestOutputStruct::OK;
	  }else{
	    SockAuthToken.clear(); //invalid token
	    //Bad Authentication - return error
	    out.CODE = RestOutputStruct::UNAUTHORIZED;
	    //SetOutputError(&ret, JsonValueToString(out.in_struct.id), 401, "Unauthorized");
	  }
		
	}else if( AUTHSYSTEM->checkAuth(SockAuthToken) ){ //validate current Authentication token	 
	  //Now provide access to the various subsystems
	  //Pre-set any output fields
          QJsonObject outargs;	
	    //ret.insert("namespace", QJsonValue("rpc"));
	    //ret.insert("name", QJsonValue("response"));
	    //ret.insert("id", doc.object().value("id")); //use the same ID for the return message
	  out.CODE = EvaluateBackendRequest(out.in_struct.namesp, out.in_struct.name, out.in_struct.args, &outargs);
            out.out_args = outargs; //ret.insert("args",outargs);	  
        }else{
	  out.CODE = RestOutputStruct::UNAUTHORIZED;
	  //Bad/No authentication
	  //SetOutputError(&ret, JsonValueToString(doc.object().value("id")), 401, "Unauthorized");
	}
	    	
      //}else if(good && (JsonValueToString(doc.object().value("namespace"))=="events") ){
      }else if(out.in_struct.namesp.toLower() == "events"){
          if( AUTHSYSTEM->checkAuth(SockAuthToken) ){ //validate current Authentication token	 
	    //Pre-set any output fields
            QJsonObject outargs;	
	      //ret.insert("namespace", QJsonValue("events"));
	      //ret.insert("name", QJsonValue("response"));
	      //ret.insert("id", doc.object().value("id")); //use the same ID for the return message
	    //Assemble the list of input events
	    QStringList evlist;
	    if(out.in_struct.args.isObject()){ evlist << JsonValueToString(out.in_struct.args); }
	    else if(out.in_struct.args.isArray()){ evlist = JsonArrayToStringList(out.in_struct.args.toArray()); }
	    //Now subscribe/unsubscribe to these events
	    if(out.in_struct.name=="subscribe"){
	      if(evlist.contains("dispatcher")){ 
	        SendAppCafeEvents = true; 
	        outargs.insert("subscribe",QJsonValue("dispatcher"));  
		QTimer::singleShot(100, this, SLOT(AppCafeStatusUpdate()) );
	      }
	    }else if(out.in_struct.name=="unsubscribe"){
	      if(evlist.contains("dispatcher")){ 
		SendAppCafeEvents = false; 
		outargs.insert("unsubscribe",QJsonValue("dispatcher"));
	      }
	    }else{
	      outargs.insert("unknown",QJsonValue("unknown"));
	    }
            //ret.insert("args",outargs);
	    out.out_args = outargs;
          }else{
	    //Bad/No authentication
	    out.CODE = RestOutputStruct::UNAUTHORIZED;
	  }
	//Other namespace - check whether auth has already been established before continuing
        }else if( AUTHSYSTEM->checkAuth(SockAuthToken) ){ //validate current Authentication token	 
	  //Now provide access to the various subsystems
	  //Pre-set any output fields
          QJsonObject outargs;	
	    out.CODE = EvaluateBackendRequest(out.in_struct.namesp, out.in_struct.name, out.in_struct.args, &outargs);
            out.out_args = outargs; //ret.insert("args",outargs);	
	}else{
	  //Error in inputs - assemble the return error message
	  out.CODE = RestOutputStruct::BADREQUEST;
	}
    //If this is a REST input - go ahead and format the output header
    if(out.CODE == RestOutputStruct::OK){
      out.Header << "Content-Type: text/json; charset=utf-8";
    }
  }
  //Return any information
   if(SOCKET!=0){ SOCKET->sendTextMessage(out.assembleMessage()); }
  else if(TSOCKET!=0){ TSOCKET->write(out.assembleMessage().toUtf8().data()); }
}

// === SYSCACHE REQUEST INTERACTION ===
/*void WebSocket::EvaluateBackendRequest(QString name, const QJsonValue args, QJsonObject *out){
  QJsonObject obj; //output object
  if(args.isObject()){
    //For the moment: all arguments are full syscache DB calls - no special ones
    QStringList reqs = args.toObject().keys();
    if(!reqs.isEmpty()){
      if(DEBUG){ qDebug() << "Parsing Inputs:" << reqs; }
      for(int r=0; r<reqs.length(); r++){
        QString req =  JsonValueToString(args.toObject().value(reqs[r]));
        if(DEBUG){ qDebug() << "  ["+reqs[r]+"]="+req; }
        QStringList values;
	if(name.toLower()=="syscache"){values = SysCacheClient::parseInputs( QStringList() << req ); }
	else if(name.toLower()=="dispatcher"){values = DispatcherClient::parseInputs( QStringList() << req, AUTHSYSTEM); }
        values.removeAll("");
        //Quick check if a list of outputs was returned
        if(values.length()==1 && name.toLower()=="syscache"){
          values = values[0].split(SCLISTDELIM); //split up the return list (if necessary)
          values.removeAll("");
        }
        if(DEBUG){ qDebug() << " - Returns:" << values; }
        if(values.length()<2){ out->insert(req, QJsonValue(values.join("")) ); }
        else{
          //This is an array of outputs
          QJsonArray arr;
          for(int i=0; i<values.length(); i++){ arr.append(values[i]); }
          out->insert(req,arr);
        }
      }
    } //end of special "request" objects
  }else if(args.isArray()){
    QStringList inputs = JsonArrayToStringList(args.toArray());
    if(DEBUG){ qDebug() << "Parsing Array inputs:" << inputs; }
    QStringList values;
      if(name.toLower()=="syscache"){values = SysCacheClient::parseInputs( inputs ); }
      else if(name.toLower()=="dispatcher"){values = DispatcherClient::parseInputs( inputs , AUTHSYSTEM); }
    if(DEBUG){ qDebug() << " - Returns:" << values; }
    for(int i=0; i<values.length(); i++){
      if(name.toLower()=="syscache" && values[i].contains(SCLISTDELIM)){
	  //This is an array of values from syscache
	  QStringList vals = values[i].split(SCLISTDELIM);
	  vals.removeAll("");
	  QJsonArray arr;
	    for(int j=0; j<vals.length(); j++){ arr.append(vals[j]); }
	    out->insert(inputs[i],arr);
      }else{
          out->insert(inputs[i],values[i]);
      }
    }
  } //end array of inputs

}*/

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
  qDebug() << "Array to List:" << array.count();
  for(int i=0; i<array.count(); i++){
    out << JsonValueToString(array.at(i));
  }
  return out;  
}

void WebSocket::SetOutputError(QJsonObject *ret, QString id, int err, QString msg){
  ret->insert("namespace", QJsonValue("rpc"));
  ret->insert("name", QJsonValue("error"));
  ret->insert("id",QJsonValue(id));
  QJsonObject obj;
  	obj.insert("code", err);
	obj.insert("message", QJsonValue(msg));
  ret->insert("args",obj);	
}

// =====================
//       PRIVATE SLOTS
// =====================
void WebSocket::checkIdle(){
  //This function is called automatically every few seconds that a client is connected
  if(SOCKET !=0){
    qDebug() << " - Client Timeout: Closing connection...";
    SOCKET->close(); //timeout - close the connection to make way for others
  }
  if(TSOCKET !=0){
    qDebug() << " - Client Timeout: Closing connection...";
    TSOCKET->close(); //timeout - close the connection to make way for others
  }
}

void WebSocket::SocketClosing(){
  qDebug() << "Socket Closing...";
  if(idletimer->isActive()){ 
    //This means the client deliberately closed the connection - not the idle timer
    idletimer->stop(); 
  }
  //Stop any current requests

  //Reset the pointer
  if(SOCKET!=0){ SOCKET = 0;	 }
  if(TSOCKET!=0){ TSOCKET = 0; }
  
  emit SocketClosed(SockID);
}

void WebSocket::EvaluateMessage(const QByteArray &msg){
  qDebug() << "New Binary Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  EvaluateREST( QString(msg) );
  idletimer->start(); 
  qDebug() << "Done with Message";
}

void WebSocket::EvaluateMessage(const QString &msg){ 
  qDebug() << "New Text Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  EvaluateREST(msg);
  idletimer->start(); 
  qDebug() << "Done with Message";
}

void WebSocket::EvaluateTcpMessage(){
  //Need to read the data from the Tcp socket and turn it into a string
  qDebug() << "New TCP Message:";
  if(idletimer->isActive()){ idletimer->stop(); }
  EvaluateREST( QString(TSOCKET->readAll()) );
  idletimer->start(); 
  qDebug() << "Done with Message";	
}

// ======================
//       PUBLIC SLOTS
// ======================
void WebSocket::AppCafeStatusUpdate(QString msg){
  if(!msg.isEmpty()){ lastDispatchEvent = msg; }
  else{ msg = lastDispatchEvent; }
  //qDebug() << "Socket Status Update:" << msg;
  if(!SendAppCafeEvents){ return; } //don't report events on this socket
  RestOutputStruct out;
    out.CODE = RestOutputStruct::OK;
    out.in_struct.name = "event";
    out.in_struct.namesp = "events";
  //Define the output structures
  //QJsonObject ret; //return message
  //Pre-set any output fields
   QJsonObject outargs;	
   //ret.insert("namespace", QJsonValue("events"));
   //ret.insert("name", QJsonValue("event"));
   //ret.insert("id", QJsonValue(""));
     outargs.insert("name", "dispatcher");
     outargs.insert("args",QJsonValue(msg));
  out.out_args = outargs;
   //ret.insert("args",outargs);	

      //Assemble the output JSON document/text
      //QJsonDocument retdoc; 
      //retdoc.setObject(ret);
    //out.Body = retdoc.toJson();
    out.Header << "Content-Type: text/json; charset=utf-8";
  //Now send the message back through the socket
  if(SOCKET!=0){ SOCKET->sendTextMessage(out.assembleMessage()); }
  else if(TSOCKET!=0){ TSOCKET->write(out.assembleMessage().toUtf8().data()); }
}
