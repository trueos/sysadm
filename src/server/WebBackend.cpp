// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> DEC 2015
// =================================
#include <WebSocket.h>

//sysadm library interface classes
#include "sysadm-general.h"
#include "sysadm-network.h"

#include "syscache-client.h"
#include "dispatcher-client.h"

#define DEBUG 0
#define SCLISTDELIM QString("::::") //SysCache List Delimiter

void WebSocket::EvaluateBackendRequest(QString name, const QJsonValue args, QJsonObject *out){
  //Go through and forward this request to the appropriate sub-system
  if(name.toLower()=="syscache"){
    return EvaluateSyscacheRequest(args, out);
  }else if(name.toLower()=="dispatcher"){
    return EvaluateSyscacheRequest(args, out);
  }else{
    return RestOutputStruct::BADREQUEST; 
  }

}

RestOutputStruct::ExitCode WebSocket::EvaluateSyscacheRequest(const QJsonValue in_args, QJSonObject *out){
  //syscache only needs a list of sub-commands at the moment (might change later)
  QStringList in_req;

  //Parse the input arguments structure
  if(args.isArray()){ in_req = JsonArrayToStringList(args.toArray()); }
  else if(args.isObject()){
    QStringList keys = arg.toObject().keys();
    for(int i=0; i<keys.length(); i++){ in_req << JsonValueToString(args.toObject().value(keys[i])); }
  }else if(args.isValue()){ in_req << JsonValueToString(args.toValue()); }
  }else{ return RestOutputStruct::BADREQUEST; }

  //Run the Request (should be one value for each in_req)
  QStringList values = SysCacheClient::parseInputs(in_req);
  while(values.length() < in_req.length()){ values << "[ERROR]"; } //ensure lists are same length

  //Format the result
  for(int i=0; i<values.length(); i++){
      if(values[i].contains(SCLISTDELIM)){
	  //This is an array of values from syscache
	  QStringList vals = values[i].split(SCLISTDELIM);
	  vals.removeAll("");
	  QJsonArray arr;
	    for(int j=0; j<vals.length(); j++){ arr.append(vals[j]); }
	    out->insert(in_req[i],arr);
      }else{
          out->insert(in_req[i],values[i]);
      }
    }
  //Return Success
  return RestOutputStruct::OK;
}

RestOutputStruct::ExitCode WebSocket::EvaluateDispatcherRequest(const QJsonValue in_args, QJSonObject *out){
  //dispatcher only needs a list of sub-commands at the moment (might change later)
  QStringList in_req;

  //Parse the input arguments structure
  if(args.isArray()){ in_req = JsonArrayToStringList(args.toArray()); }
  else if(args.isObject()){
    QStringList keys = arg.toObject().keys();
    for(int i=0; i<keys.length(); i++){ in_req << JsonValueToString(args.toObject().value(keys[i])); }
  }else if(args.isValue()){ in_req << JsonValueToString(args.toValue()); }
  }else{ return RestOutputStruct::BADREQUEST; }

  //Run the Request (should be one value for each in_req)
  QStringList values = DispatcherClient::parseInputs(in_req, AUTHSYSTEM);;
  while(values.length() < in_req.length()){ values << "[ERROR]"; } //ensure lists are same length

  //Format the result
  for(int i=0; i<values.length(); i++){ out->insert(in_req[i],values[i]); }
  //Return Success
  return RestOutputStruct::OK;
}

