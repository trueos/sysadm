// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> DEC 2015
// =================================
#include <WebSocket.h>

//sysadm library interface classes
#include "sysadm-general.h"
#include "sysadm-network.h"
#include "sysadm-lifepreserver.h"

#include "syscache-client.h"
#include "dispatcher-client.h"

#define DEBUG 0
#define SCLISTDELIM QString("::::") //SysCache List Delimiter

RestOutputStruct::ExitCode WebSocket::EvaluateBackendRequest(QString namesp, QString name, const QJsonValue args, QJsonObject *out){
  /*Inputs: 
	"namesp" - namespace for the request
	"name" - name of the request
	"args" - JSON input arguments structure
	"out" - JSON output arguments structure
  */
  namesp = namesp.toLower(); name = name.toLower();
  //Go through and forward this request to the appropriate sub-system
  if(namesp=="rpc" && name=="syscache"){
    return EvaluateSyscacheRequest(args, out);
  }else if(namesp=="rpc" && name=="dispatcher"){
    return EvaluateSyscacheRequest(args, out);
  }else if(namesp=="sysadm" && name=="network"){
    return EvaluateSysadmNetworkRequest(args, out);
  }else if(namesp=="sysadm" && name=="lifepreserver"){
    return EvaluateSysadmLifePreserverRequest(args, out);
  }else{
    return RestOutputStruct::BADREQUEST; 
  }

}

//==== SYSCACHE ====
RestOutputStruct::ExitCode WebSocket::EvaluateSyscacheRequest(const QJsonValue in_args, QJsonObject *out){
  //syscache only needs a list of sub-commands at the moment (might change later)
  QStringList in_req;

  //Parse the input arguments structure
  if(in_args.isArray()){ in_req = JsonArrayToStringList(in_args.toArray()); }
  else if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    for(int i=0; i<keys.length(); i++){ in_req << JsonValueToString(in_args.toObject().value(keys[i])); }
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

//==== DISPATCHER ====
RestOutputStruct::ExitCode WebSocket::EvaluateDispatcherRequest(const QJsonValue in_args, QJsonObject *out){
  //dispatcher only needs a list of sub-commands at the moment (might change later)
  QStringList in_req;

  //Parse the input arguments structure
  if(in_args.isArray()){ in_req = JsonArrayToStringList(in_args.toArray()); }
  else if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    for(int i=0; i<keys.length(); i++){ in_req << JsonValueToString(in_args.toObject().value(keys[i])); }
  }else{ return RestOutputStruct::BADREQUEST; }

  //Run the Request (should be one value for each in_req)
  QStringList values = DispatcherClient::parseInputs(in_req, AUTHSYSTEM);;
  while(values.length() < in_req.length()){ values << "[ERROR]"; } //ensure lists are same length

  //Format the result
  for(int i=0; i<values.length(); i++){ out->insert(in_req[i],values[i]); }
  //Return Success
  return RestOutputStruct::OK;
}

//==== SYSADM -- Network ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmNetworkRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="list-devices"){
	ok = true;
        QStringList devs = sysadm::NetDevice::listNetDevices();
	for(int i=0; i<devs.length(); i++){
	  sysadm::NetDevice D(devs[i]);
	  QJsonObject obj;
	    //assemble the information about this device into an output object
	    obj.insert("ipv4", D.ipAsString());
	    obj.insert("ipv6", D.ipv6AsString());
	    obj.insert("netmask", D.netmaskAsString());
	    obj.insert("description", D.desc());
	    obj.insert("MAC", D.macAsString());
	    obj.insert("status", D.mediaStatusAsString());
	    obj.insert("is_active", D.isUp() ? "true" : "false" );
	    obj.insert("is_dhcp", D.usesDHCP() ? "true" : "false" );
	    obj.insert("is_wireless", D.isWireless() ? "true" : "false" );
	  //Add this device info to the main output structure
	  out->insert(devs[i], obj);
	}
      }

    } //end of "action" key usage
    
    //If nothing done - return the proper code
    if(!ok){
      return RestOutputStruct::BADREQUEST;
    }
  }else{  // if(in_args.isArray()){
    return RestOutputStruct::BADREQUEST;
  }
  return RestOutputStruct::OK;
}

//==== SYSADM -- LifePreserver ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmLifePreserverRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="addreplication"){
	ok = true;
        out->insert("addreplication", sysadm::LifePreserver::addReplication(in_args.toObject()));
      }
      if(act=="cronscrub"){
	ok = true;
        out->insert("cronscrub", sysadm::LifePreserver::scheduleScrub(in_args.toObject()));
      }
      if(act=="cronsnap"){
	ok = true;
        out->insert("cronsnap", sysadm::LifePreserver::scheduleSnapshot(in_args.toObject()));
      }
      if(act=="listcron"){
	ok = true;
        out->insert("listcron", sysadm::LifePreserver::listCron());
      }
      if(act=="listsnap"){
	ok = true;
        out->insert("listsnap", sysadm::LifePreserver::listSnap(in_args.toObject()));
      }
      if(act=="removesnap"){
	ok = true;
        out->insert("removesnap", sysadm::LifePreserver::removeSnapshot(in_args.toObject()));
      }
      if(act=="revertsnap"){
	ok = true;
        out->insert("revertsnap", sysadm::LifePreserver::revertSnapshot(in_args.toObject()));
      }
      if(act=="savesettings"){
	ok = true;
        out->insert("savesettings", sysadm::LifePreserver::saveSettings(in_args.toObject()));
      }
      if(act=="settings"){
	ok = true;
        out->insert("settings", sysadm::LifePreserver::settings());
      }

    } //end of "action" key usage
    
    //If nothing done - return the proper code
    if(!ok){
      return RestOutputStruct::BADREQUEST;
    }
  }else{  // if(in_args.isArray()){
    return RestOutputStruct::BADREQUEST;
  }
  return RestOutputStruct::OK;
}
