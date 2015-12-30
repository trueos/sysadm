// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_SERVER_REST_STRUCTS_H
#define _PCBSD_REST_SERVER_REST_STRUCTS_H
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>

#define CurHttpVersion QString("HTTP/1.1")

//NOTE: The input structure parsing assumes a JSON input body
//NOTE: Common VERB's are:
/*	GET - Read a resource (no changes made)
	PUT - Insert/update a resource (makes changes - nothing automatically assigned for a new resource)
	POST - Insert/update a resource (makes changes - automatically assigns data for new resource as necessary)
	DELETE - Remove a resource (makes changes)
	OPTIONS - List the allowed options on a resource (no changes made)
	HEAD - List the response headers only (no changes made)
*/
class RestInputStruct{
public:
	//REST Variables
	QString VERB, URI, HTTPVERSION;
	//JSON input variables
	QString name, namesp, id;
	QJsonValue args;
	//Raw Text
	QStringList Header; //REST Headers
	QString Body; //Everything else

	RestInputStruct(QString message = ""){
	  HTTPVERSION = CurHttpVersion; //default value
	  if(message.isEmpty()){ return; }
	  //Pull out any REST headers
	  if(!message.startsWith("{")){ 
	    Header = message.section("\n{",0,0).split("\n");
	  }
	  if(!Header.isEmpty()){
	    QString line = Header.takeFirst(); //The first line is special (not a generic header)
	    VERB = line.section(" ",0,0);
	    URI = line.section(" ",1,1);
	    HTTPVERSION = line.section(" ",2,2);
	    Body = message.remove(Header.join("\n")+"\n"); //chop the headers off the front
	  }
	  //Now Parse out the Body into the JSON fields and/or arguments structure
	  Body = Body.simplified(); //remove any extra whitespace on the beginning/end
	  if(Body.startsWith("{") && Body.endsWith("}") ){
	    QJsonDocument doc = QJsonDocument::fromJson(Body.toUtf8());
	    if(!doc.isNull() && doc.isObject() ){
	      //Valid JSON found
	      if(doc.object().contains("namespace") ){ namesp = doc.object().value("namespace").toString(); }
	      if(doc.object().contains("name") ){ name = doc.object().value("name").toString(); }
	      if(doc.object().contains("id") ){ namesp = doc.object().value("id").toString(); }
	      if(doc.object().contains("args") ){ args = doc.object().value("args"); }
	      else{
	        //no args structure - treat the entire body as the arguments struct
		args = doc.object();
	      }
	    }
	  }
	  //Now do any REST -> JSON conversions if necessary
	  if(!URI.isEmpty()){
	    //TO-DO
	    name = URI.section("/",-1); //last entry
	    namesp = URI.section("/",0,-2); //URI excluding name
	  }
	}
	~RestInputStruct(){}
		
};

class RestOutputStruct{
public:
	enum ExitCode{OK, CREATED, ACCEPTED, NOCONTENT, RESETCONTENT, PARTIALCONTENT, PROCESSING, BADREQUEST, UNAUTHORIZED, FORBIDDEN, NOTFOUND };
	RestInputStruct in_struct;
	ExitCode CODE;
	QStringList Header; //REST output header lines
	QJsonValue out_args;
	
	RestOutputStruct(){
	  CODE = BADREQUEST; //default exit code
	}
	~RestOutputStruct(){}
		
	QString assembleMessage(){
	  if( !in_struct.VERB.isEmpty() ){
	    //REST output syntax
	    QStringList headers;
	    QString firstline = in_struct.HTTPVERSION;
	    if(firstline.isEmpty()){ firstline = CurHttpVersion; }//default value
	    QString Body;
	    if(!out_args.isNull()){ 
	      QJsonObject obj; obj.insert("args", out_args);
	      Body = QJsonDocument(obj).toJson(QJsonDocument::Compact); 
	    }
	    switch(CODE){
	      case PROCESSING:
	        firstline.append(" 102 Processing"); break;
	      case OK:
	        firstline.append(" 200 OK"); break;
	      case CREATED:
	        firstline.append(" 201 Created"); break;
	      case ACCEPTED:
	        firstline.append(" 202 Accepted"); break;
	      case NOCONTENT:
	        firstline.append(" 204 No Content"); break;
	      case RESETCONTENT:
	        firstline.append(" 205 Reset Content"); break;
	      case PARTIALCONTENT:
	        firstline.append(" 206 Partial Content"); break;
	      case BADREQUEST:
	        firstline.append(" 400 Bad Request"); break;
	      case UNAUTHORIZED:
	        firstline.append(" 401 Unauthorized"); break;
	      case FORBIDDEN:
	        firstline.append(" 403 Forbidden"); break;
	      case NOTFOUND:
	        firstline.append(" 404 Not Found"); break;
	    }
	    headers << firstline;
	    headers << "Date: "+QDateTime::currentDateTime().toString(Qt::ISODate);
	    if(!Header.isEmpty()){ headers << Header; }
	    //Now add the body of the return
	    if(!Body.isEmpty()){ headers << "Content-Length: "+QString::number(Body.length()); }
	    headers << Body;
	    return headers.join("\n");
	    
	  }else{
	    //JSON output (load all the input fields for the moment)
	    QString oname = "response"; //use this for valid responses (input ID found)
	    if(in_struct.id.isEmpty()){ oname = in_struct.name; }
	    QString onamesp = in_struct.namesp;
	    QString oid = in_struct.id;
	    if(CODE!=OK){
	      //Format the output based on the type of error
	      QJsonObject out_err;
	      switch(CODE){
	      case PROCESSING:
		//oname = onamesp = "error";
		out_err.insert("code","102"); out_err.insert("message", "Processing"); break;
	      case CREATED:
		//oname = onamesp = "error";
		out_err.insert("code","201"); out_err.insert("message", "Created"); break;
	      case ACCEPTED:
		//oname = onamesp = "error";
		out_err.insert("code","202"); out_err.insert("message", "Accepted"); break;
	      case NOCONTENT:
		oname = onamesp = "error";
		out_err.insert("code","204"); out_err.insert("message", "No Content"); break;
	      case RESETCONTENT:
		//oname = onamesp = "error";
		out_err.insert("code","205"); out_err.insert("message", "Reset Content"); break;
	      case PARTIALCONTENT:
		oname = onamesp = "error";
		out_err.insert("code","206"); out_err.insert("message", "Partial Content"); break;
	      case BADREQUEST:
		oname = onamesp = "error";
		out_err.insert("code","400"); out_err.insert("message", "Bad Request"); break;
	      case UNAUTHORIZED:
		oname = onamesp = "error";
		out_err.insert("code","401"); out_err.insert("message", "Unauthorized"); break;
	      case FORBIDDEN:
		oname = onamesp = "error";
		out_err.insert("code","403"); out_err.insert("message", "Forbidden"); break;
	      case NOTFOUND:
		oname = onamesp = "error";
		out_err.insert("code","404"); out_err.insert("message", "Not Found"); break;
	      default:
		break;
	      }
	      out_args = out_err;
	    }
	    //Now assemple the JSON output
	    QJsonObject obj;
	    obj.insert("namespace",onamesp);
	    obj.insert("name",oname);
	    obj.insert("id",oid);
	    obj.insert("args", out_args);
	    //Convert the JSON to string form
	    QJsonDocument doc(obj);
	    return doc.toJson(QJsonDocument::Compact);
	  }
	}
};

#endif
