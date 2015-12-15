// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> DEC 2015
// =================================
#include <WebSocket.h>

//sysadm library interface classes
#include <sysadm-general.h>
#include <sysadm-network.h>

#include "syscache-client.h"
#include "dispatcher-client.h"

#define DEBUG 0
#define SCLISTDELIM QString("::::") //SysCache List Delimiter

void WebSocket::EvaluateBackendRequest(QString name, const QJsonValue args, QJsonObject *out){
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
  
}