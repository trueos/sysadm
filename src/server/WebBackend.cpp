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
#include "library/sysadm-beadm.h"
#include "library/sysadm-general.h"
#include "library/sysadm-iocage.h"
#include "library/sysadm-iohyve.h"
#include "library/sysadm-lifepreserver.h"
#include "library/sysadm-network.h"
#include "library/sysadm-systemmanager.h"
#include "library/sysadm-update.h"
#include "library/sysadm-zfs.h"
#include "library/sysadm-pkg.h"

#include "syscache-client.h"

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
  // - server settings (always available)
  out->insert("sysadm/settings","read/write");

  // - syscache
  if(QFile::exists("/var/run/syscache.pipe")){
    out->insert("rpc/syscache","read"); //no write to syscache - only reads
  }

  // - beadm
  if(QFile::exists("/usr/local/sbin/beadm")){
    out->insert("sysadm/beadm", "read/write");
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

  // - iohyve
  if(QFile::exists("/usr/local/sbin/iohyve")){
    out->insert("sysadm/iohyve", "read/write");
  }
  
  // - zfs
  if(QFile::exists("/sbin/zfs") && QFile::exists("/sbin/zpool")){
    out->insert("sysadm/zfs", allaccess ? "read/write" : "read");
  }
  
  // - pkg
  if(QFile::exists("/usr/local/sbin/pkg")){
    out->insert("sysadm/pkg", "read/write");
  }
  
  // - Generic system information
  out->insert("sysadm/systemmanager","read/write");

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
  //qDebug() << "Evaluate Backend Request:" << namesp << name;
  //Go through and forward this request to the appropriate sub-system
  if(namesp=="sysadm" && name=="settings"){
    return EvaluateSysadmSettingsRequest(IN.args, out);
  }else if(namesp=="rpc" && name=="dispatcher"){
    return EvaluateDispatcherRequest(IN.fullaccess, IN.args, out);
  }else if(namesp=="sysadm" && name=="beadm"){
    return EvaluateSysadmBEADMRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="iocage"){
    return EvaluateSysadmIocageRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="iohyve"){
    return EvaluateSysadmIohyveRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="lifepreserver"){
    return EvaluateSysadmLifePreserverRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="network"){
    return EvaluateSysadmNetworkRequest(IN.args, out);
  }else if(namesp=="rpc" && name=="syscache"){
    return EvaluateSyscacheRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="systemmanager"){
    return EvaluateSysadmSystemMgmtRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="update"){
    return EvaluateSysadmUpdateRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="zfs"){
    return EvaluateSysadmZfsRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="pkg"){
    return EvaluateSysadmPkgRequest(IN.args, out);
  }else{
    return RestOutputStruct::BADREQUEST;
  }

}

// === SYSADM SETTINGS ===
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmSettingsRequest(const QJsonValue in_args, QJsonObject *out){
  qDebug() << "sysadm/settings Request:" << in_args;
  if(!in_args.isObject()){ return RestOutputStruct::BADREQUEST; }
  QJsonObject argsO = in_args.toObject();
  QStringList keys = argsO.keys();
  qDebug() << " - keys:" << keys;
  if(!keys.contains("action")){ return RestOutputStruct::BADREQUEST; }
  QString act = argsO.value("action").toString();
  bool ok = false;
  if(act=="register_ssl_cert" && keys.contains("pub_key")){
    //Required arguments: "pub_key" (String)
    //Optional arguments: "nickname" (String), "email" (String)
    QString pub_key, nickname, email;
    pub_key = argsO.value("pub_key").toString();
    if(keys.contains("nickname")){ nickname = argsO.value("nickname").toString(); }
    if(keys.contains("email")){ email = argsO.value("email").toString(); }
    
    if(!pub_key.isEmpty()){
      ok = AUTHSYSTEM->RegisterCertificate(SockAuthToken, pub_key, nickname, email);
	    if(!ok){ return RestOutputStruct::FORBIDDEN; }
    }
  }else if(act=="list_ssl_certs"){
    AUTHSYSTEM->ListCertificates(SockAuthToken, out);
    ok = true; //always works for current user (even if nothing found)
  }else if(act=="revoke_ssl_cert" && keys.contains("pub_key") ){
    //Additional arguments: "user" (optional), "pub_key" (String)
    QString user; if(keys.contains("user")){ user = argsO.value("user").toString(); }
    ok = AUTHSYSTEM->RevokeCertificate(SockAuthToken,argsO.value("pub_key").toString(), user);
  }

  if(ok){ return RestOutputStruct::OK; }
  else{ return RestOutputStruct::BADREQUEST; }
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
  if(act=="run"){
    if(!allaccess){ return RestOutputStruct::FORBIDDEN; } //this user does not have permission to queue jobs
    QStringList ids = in_args.toObject().keys();
    ids.removeAll("action"); //already handled the action
    for(int i=0; i<ids.length(); i++){
      //Get the list of commands for this id
      QStringList cmds;
      QJsonValue val = in_args.toObject().value(ids[i]);
      if(val.isArray()){ cmds = JsonArrayToStringList(val.toArray()); }
      else if(val.isString()){ cmds << val.toString(); }
      else{
	ids.removeAt(i);
	i--;
	continue;
      }
      //queue up this process

      DISPATCHER->queueProcess(ids[i], cmds);
    }
    //Return the PENDING result
    LogManager::log(LogManager::HOST, "Client Launched Processes["+SockPeerIP+"]: "+ids.join(",") );
    out->insert("started", QJsonArray::fromStringList(ids));
  //}else if(act=="read"){

  }else{
    return RestOutputStruct::BADREQUEST;
  }

  //Return Success
  return RestOutputStruct::OK;
}

//==== SYSADM -- BEADM ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmBEADMRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action")).toLower();
      if(act=="listbes"){
	ok = true;
        out->insert("listbes", sysadm::BEADM::listBEs());
      }else if(act=="renamebe"){
	ok = true;
        out->insert("renamebe", sysadm::BEADM::renameBE(in_args.toObject()));
      }else if(act=="activatebe"){
	ok = true;
        out->insert("activatebe", sysadm::BEADM::activateBE(in_args.toObject()));
      }else if(act=="createbe"){
	ok = true;
        out->insert("createbe", sysadm::BEADM::createBE(in_args.toObject()));
      }else if(act=="destroybe"){
	ok = true;
        out->insert("destroybe", sysadm::BEADM::destroyBE(in_args.toObject()));
      }else if(act=="mountbe"){
	ok = true;
        out->insert("mountbe", sysadm::BEADM::mountBE(in_args.toObject()));
     }else if(act=="umountbe"){
	ok = true;
        out->insert("umountbe", sysadm::BEADM::umountBE(in_args.toObject()));
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

//==== SYSADM -- SysMgmt ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmSystemMgmtRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      if(act=="batteryinfo"){
	ok = true;
        out->insert("batteryinfo", sysadm::SysMgmt::batteryInfo());
      }
      if(act=="cpupercentage"){
	ok = true;
        out->insert("cpupercentage", sysadm::SysMgmt::cpuPercentage());
      }
      if(act=="cputemps"){
	ok = true;
        out->insert("cputemps", sysadm::SysMgmt::cpuTemps());
      }
      if(act=="externalmounts"){
	ok = true;
        out->insert("externalmounts", sysadm::SysMgmt::externalDevicePaths());
      }
      if(act=="halt"){
	ok = true;
        out->insert("halt", sysadm::SysMgmt::systemHalt());
      }
      if(act=="killproc"){
	ok = true;
        out->insert("killproc", sysadm::SysMgmt::killProc(in_args.toObject()));
      }
      if(act=="memorystats"){
	ok = true;
        out->insert("memorystats", sysadm::SysMgmt::memoryStats());
      }
      if(act=="procinfo"){
	ok = true;
        out->insert("procinfo", sysadm::SysMgmt::procInfo());
      }
      if(act=="reboot"){
	ok = true;
        out->insert("reboot", sysadm::SysMgmt::systemReboot());
      }
      if(act=="setsysctl"){
	ok = true;
        out->insert("setsysctl", sysadm::SysMgmt::setSysctl(in_args.toObject()));
      }
      if(act=="sysctllist"){
	ok = true;
        out->insert("sysctllist", sysadm::SysMgmt::sysctlList());
      }
      if(act=="systeminfo"){
	ok = true;
        out->insert("systeminfo", sysadm::SysMgmt::systemInfo());
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
	      
      }else if(act=="listbranches"){
	ok = true;
        out->insert("listbranches", sysadm::Update::listBranches());
	      
      }else if(act=="startupdate"){
	ok = true;
	out->insert("startupdate", sysadm::Update::startUpdate(in_args.toObject()) );
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
      if(act=="execjail"){
	ok = true;
        out->insert("execjail", sysadm::Iocage::execJail(in_args.toObject()));
      }
      if(act=="df"){
	ok = true;
        out->insert("df", sysadm::Iocage::df());
      }
      if(act=="destroyjail"){
	ok = true;
        out->insert("destroyjail", sysadm::Iocage::destroyJail(in_args.toObject()));
      }
      if(act=="createjail"){
	ok = true;
        out->insert("createjail", sysadm::Iocage::createJail(in_args.toObject()));
      }
      if(act=="clonejail"){
	ok = true;
        out->insert("clonejail", sysadm::Iocage::cloneJail(in_args.toObject()));
      }
      if(act=="cleanall"){
	ok = true;
        out->insert("cleanall", sysadm::Iocage::cleanAll());
      }
      if(act=="cleantemplates"){
	ok = true;
        out->insert("cleantemplates", sysadm::Iocage::cleanTemplates());
      }
      if(act=="cleanreleases"){
	ok = true;
        out->insert("cleanreleases", sysadm::Iocage::cleanReleases());
      }
      if(act=="cleanjails"){
	ok = true;
        out->insert("cleanjails", sysadm::Iocage::cleanJails());
      }
      if(act=="capjail"){
	ok = true;
        out->insert("capjail", sysadm::Iocage::capJail(in_args.toObject()));
      }
      if(act=="deactivatepool"){
	ok = true;
        out->insert("deactivatepool", sysadm::Iocage::deactivatePool(in_args.toObject()));
      }
      if(act=="activatepool"){
	ok = true;
        out->insert("activatepool", sysadm::Iocage::activatePool(in_args.toObject()));
      }
      if(act=="stopjail"){
	ok = true;
        out->insert("stopjail", sysadm::Iocage::stopJail(in_args.toObject()));
      }
      if(act=="startjail"){
	ok = true;
        out->insert("startjail", sysadm::Iocage::startJail(in_args.toObject()));
      }
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

//==== SYSADM -- iohyve ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmIohyveRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action"));
      //qDebug() << " - iohyve action:" << act;
      if(act=="adddisk"){
	ok = true;
        out->insert("adddisk", sysadm::Iohyve::addDisk(in_args.toObject()));
      }
      if(act=="create"){
	ok = true;
        out->insert("create", sysadm::Iohyve::createGuest(in_args.toObject()));
      }
      if(act=="delete"){
	ok = true;
        out->insert("delete", sysadm::Iohyve::deleteGuest(in_args.toObject()));
      }
      if(act=="deletedisk"){
	ok = true;
        out->insert("deletedisk", sysadm::Iohyve::deleteDisk(in_args.toObject()));
      }
      else if(act=="listdisks"){
	ok = true;
        out->insert("listdisks", sysadm::Iohyve::listDisks(in_args.toObject()));
      }
      else if(act=="listvms"){
	ok = true;
        out->insert("listvms", sysadm::Iohyve::listVMs());
      }
      else if(act=="listisos"){
        ok = true;
	out->insert("listisos", sysadm::Iohyve::listISOs());
      }
      else if(act=="fetchiso"){
	ok = true;
	//DProcess fetchproc;
        out->insert("fetchiso", sysadm::Iohyve::fetchISO(in_args.toObject()));
      }
      else if(act=="install"){
	ok = true;
        out->insert("install", sysadm::Iohyve::installGuest(in_args.toObject()));
      }
      else if(act=="issetup"){
	ok = true;
        out->insert("issetup", sysadm::Iohyve::isSetup());
      }
      else if(act=="renameiso"){
	ok = true;
        out->insert("renameiso", sysadm::Iohyve::renameISO(in_args.toObject()));
      }
      else if(act=="rmiso"){
	ok = true;
        out->insert("rmiso", sysadm::Iohyve::rmISO(in_args.toObject()));
      }
      else if(act=="resizedisk"){
	ok = true;
        out->insert("resizedisk", sysadm::Iohyve::resizeDisk(in_args.toObject()));
      }
      else if(act=="setup"){
	ok = true;
        out->insert("setup", sysadm::Iohyve::setupIohyve(in_args.toObject()));
      }
      else if(act=="start"){
	ok = true;
        out->insert("start", sysadm::Iohyve::startGuest(in_args.toObject()));
      }
      else if(act=="stop"){
	ok = true;
        out->insert("stop", sysadm::Iohyve::stopGuest(in_args.toObject()));
      }
      else if(act=="version"){
	ok = true;
        out->insert("version", sysadm::Iohyve::version());
      }
      //qDebug() << " - iohyve action finished:" << act << ok;
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

// ==== SYSADM ZFS API ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmZfsRequest(const QJsonValue in_args, QJsonObject *out){
  if(!in_args.isObject() || !in_args.toObject().contains("action") ){ return RestOutputStruct::BADREQUEST; }
  QString act = in_args.toObject().value("action").toString();
  if(act=="list_pools"){
    QJsonObject pools = sysadm::ZFS::zpool_list();
    if(!pools.isEmpty()){ out->insert("list_pools",pools); }
  }else{
    //unknown action
    return RestOutputStruct::BADREQUEST;
  }
  
  return RestOutputStruct::OK;
}

// ==== SYSADM PKG API ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmPkgRequest(const QJsonValue in_args, QJsonObject *out){
  if(!in_args.isObject() || !in_args.toObject().contains("action") ){ return RestOutputStruct::BADREQUEST; }
  //REQUIRED: "action"
  QString act = in_args.toObject().value("action").toString();
  //OPTIONAL: "repo" (uses local repo database by default)
  QString repo = "local";
  if(in_args.toObject().contains("repo")){ repo = in_args.toObject().value("repo").toString(); }
  //OPTIONAL: "pkg_origins" (defaults to everything for listing functions)
  QStringList pkgs;
  if(in_args.toObject().contains("pkg_origins")){
    if(in_args.toObject().value("pkg_origins").isString()){ pkgs << in_args.toObject().value("pkg_origins").toString(); }
    else if(in_args.toObject().value("pkg_origins").isArray()){ pkgs = JsonArrayToStringList(in_args.toObject().value("pkg_origins").toArray()); }
  }
  //Parse 
  if(act=="pkg_info"){
    QJsonObject info = sysadm::PKG::pkg_info(pkgs, repo);
    if(!info.isEmpty()){ out->insert("pkg_info",info); }
    else{ return RestOutputStruct::NOCONTENT; }
  }else{
    //unknown action
    return RestOutputStruct::BADREQUEST;
  }
  
  return RestOutputStruct::OK;
}