// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> DEC 2015
// =================================
//  Note: Don't forget to run "AUTHSYSTEM->hasFullAccess(SockAuthToken)"
//    To restrict user access to some systems as needed!
// =================================
#include <WebSocket.h>

//sysadm library interface classes
#include "library/sysadm-general.h"
#include "library/sysadm-iocage.h"
#include "library/sysadm-lifepreserver.h"
#include "library/sysadm-network.h"
#include "library/sysadm-systeminfo.h"
#include "library/sysadm-update.h"

#include "syscache-client.h"
//#include "dispatcher-client.h"

#define DEBUG 0
#define SCLISTDELIM QString("::::") //SysCache List Delimiter
RestOutputStruct::ExitCode WebSocket::AvailableSubsystems(bool allaccess, QJsonObject *out){
  //Probe the various subsystems to see what is available through this server
  //Output format:
  /*<out>{
	<namespace1/name1> : <read/write/other>,
	<namespace2/name2> : <read/write/other>,
      }
  */

  // - syscache
  if(QFile::exists("/var/run/syscache.pipe")){
    out->insert("rpc/syscache","read"); //no write to syscache - only reads
  }

  // - dispatcher (Internal to server - always available)
  //"read" is the event notifications, "write" is the ability to queue up jobs
  out->insert("rpc/dispatcher", allaccess ? "read/write" : "read");
  
  // - network
  out->insert("sysadm/network","read/write");
  
  // - lifepreserver
  if(QFile::exists("/usr/local/bin/lpreserver")){
    out->insert("sysadm/lifepreserver", "read/write");
  }

  // - iocage
  if(QFile::exists("/usr/local/sbin/iocage")){
    out->insert("sysadm/iocage", "read/write");
  }
  
  // - Generic system information
  out->insert("sysadm/systeminfo","read/write");

  // - PC-BSD Updater
  if(QFile::exists("/usr/local/bin/pc-updatemanager")){
    out->insert("sysadm/update", "read/write");
  }

  return RestOutputStruct::OK;
}

RestOutputStruct::ExitCode WebSocket::EvaluateBackendRequest(const RestInputStruct &IN, QJsonObject *out){
  /*Inputs: 
	"namesp" - namespace for the request
	"name" - name of the request
	"args" - JSON input arguments structure
	"out" - JSON output arguments structure
  */
  QString namesp = IN.namesp.toLower(); QString name = IN.name.toLower();
  
  //Get/Verify subsystems
  if(namesp=="rpc" && name=="query"){
    return AvailableSubsystems(IN.fullaccess, out);
  }else{
    QJsonObject avail;
    AvailableSubsystems(IN.fullaccess, &avail);
    if(!avail.contains(namesp+"/"+name)){ return RestOutputStruct::NOTFOUND; }
  }
  
  //Go through and forward this request to the appropriate sub-system 
  if(namesp=="rpc" && name=="dispatcher"){
    return EvaluateDispatcherRequest(IN.fullaccess, IN.args, out);
  }else if(namesp=="sysadm" && name=="iocage"){
    return EvaluateSysadmIocageRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="lifepreserver"){
    return EvaluateSysadmLifePreserverRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="network"){
    return EvaluateSysadmNetworkRequest(IN.args, out);
  }else if(namesp=="rpc" && name=="syscache"){
    return EvaluateSyscacheRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="systeminfo"){
    return EvaluateSysadmSystemInfoRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="update"){
    return EvaluateSysadmUpdateRequest(IN.args, out);
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
RestOutputStruct::ExitCode WebSocket::EvaluateDispatcherRequest(bool allaccess, const QJsonValue in_args, QJsonObject *out){
  //dispatcher only needs a list of sub-commands at the moment (might change later)
  if(!in_args.isObject() || !in_args.toObject().contains("action") ){ return RestOutputStruct::BADREQUEST; }
  QString act = in_args.toObject().value("action").toString().toLower();
  
  //Determing the type of action to perform
  if(act=="run"){ }
  if(!allaccess){
    return RestOutputStruct::FORBIDDEN; //this user does not have permission to queue jobs
  }

  

  //Parse the input arguments structure
  /*if(in_args.isArray()){ in_req = JsonArrayToStringList(in_args.toArray()); }
  else if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    for(int i=0; i<keys.length(); i++){ in_req << JsonValueToString(in_args.toObject().value(keys[i])); }
  }else{ return RestOutputStruct::BADREQUEST; }

  //Run the Request (should be one value for each in_req)
  QStringList values = DispatcherClient::parseInputs(in_req, AUTHSYSTEM);
  while(values.length() < in_req.length()){ values << "[ERROR]"; } //ensure lists are same length

  //Format the result
  for(int i=0; i<values.length(); i++){ out->insert(in_req[i],values[i]); }*/
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
      if(act=="initreplication"){
	ok = true;
        out->insert("initreplication", sysadm::LifePreserver::initReplication(in_args.toObject()));
      }
      if(act=="listcron"){
	ok = true;
        out->insert("listcron", sysadm::LifePreserver::listCron());
      }
      if(act=="listreplication"){
	ok = true;
        out->insert("listreplication", sysadm::LifePreserver::listReplication());
      }
      if(act=="listsnap"){
	ok = true;
        out->insert("listsnap", sysadm::LifePreserver::listSnap(in_args.toObject()));
      }
      if(act=="removereplication"){
	ok = true;
        out->insert("removereplication", sysadm::LifePreserver::removeReplication(in_args.toObject()));
      }
      if(act=="removesnap"){
	ok = true;
        out->insert("removesnap", sysadm::LifePreserver::removeSnapshot(in_args.toObject()));
      }
      if(act=="revertsnap"){
	ok = true;
        out->insert("revertsnap", sysadm::LifePreserver::revertSnapshot(in_args.toObject()));
      }
      if(act=="runreplication"){
	ok = true;
        out->insert("runreplication", sysadm::LifePreserver::runReplication(in_args.toObject()));
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

//==== SYSADM -- SysInfo ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmSystemInfoRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="batteryinfo"){
	ok = true;
        out->insert("batteryinfo", sysadm::SysInfo::batteryInfo());
      }
      if(act=="cpupercentage"){
	ok = true;
        out->insert("cpupercentage", sysadm::SysInfo::cpuPercentage());
      }
      if(act=="cputemps"){
	ok = true;
        out->insert("cputemps", sysadm::SysInfo::cpuTemps());
      }
      if(act=="externalmounts"){
	ok = true;
        out->insert("externalmounts", sysadm::SysInfo::externalDevicePaths());
      }
      if(act=="memorystats"){
	ok = true;
        out->insert("memorystats", sysadm::SysInfo::memoryStats());
      }
      if(act=="systeminfo"){
	ok = true;
        out->insert("systeminfo", sysadm::SysInfo::systemInfo());
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

//==== SYSADM -- Update ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmUpdateRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="checkupdates"){
	ok = true;
        out->insert("checkupdates", sysadm::Update::checkUpdates());
      }
      if(act=="listbranches"){
	ok = true;
        out->insert("listbranches", sysadm::Update::listBranches());
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

//==== SYSADM -- iocage ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmIocageRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="getdefaultsettings"){
	ok = true;
        out->insert("getdefaultsettings", sysadm::Iocage::getDefaultSettings());
      }
      if(act=="getjailsettings"){
	ok = true;
        out->insert("getjailsettings", sysadm::Iocage::getJailSettings(in_args.toObject()));
      }
      if(act=="listjails"){
	ok = true;
        out->insert("listjails", sysadm::Iocage::listJails());
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
