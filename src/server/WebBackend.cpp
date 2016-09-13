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
#include "library/sysadm-filesystem.h"
#include "library/sysadm-iocage.h"
#include "library/sysadm-iohyve.h"
#include "library/sysadm-lifepreserver.h"
#include "library/sysadm-network.h"
#include "library/sysadm-systemmanager.h"
#include "library/sysadm-update.h"
#include "library/sysadm-zfs.h"
#include "library/sysadm-pkg.h"
#include "library/sysadm-users.h"
#include "library/sysadm-servicemanager.h"
#include "library/sysadm-firewall.h"

#define DEBUG 0
//#define SCLISTDELIM QString("::::") //SysCache List Delimiter
RestOutputStruct::ExitCode WebSocket::AvailableSubsystems(bool allaccess, QJsonObject *out){
  //Probe the various subsystems to see what is available through this server
  //Output format:
  /*<out>{
	<namespace1/name1> : <read/write/other>,
	<namespace2/name2> : <read/write/other>,
      }
  */
  // - server settings (always available)
  out->insert("rpc/settings","read/write");
  out->insert("rpc/logs", allaccess ? "read/write" : "read");

  // - beadm
  if(QFile::exists("/usr/local/sbin/beadm")){
    out->insert("sysadm/beadm", "read/write");
  }


  // - dispatcher (Internal to server - always available)
  //"read" is the event notifications, "write" is the ability to queue up jobs
  out->insert("rpc/dispatcher", allaccess ? "read/write" : "read");

  // - filesystem
  out->insert("sysadm/fs","read/write");

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

  // - User Manager
  out->insert("sysadm/users","read/write");
  //- Service Manager
  out->insert("sysadm/services","read/write");
  // - Firewall Manager
  out->insert("sysadm/firewall","read/write");

  return RestOutputStruct::OK;
}

RestOutputStruct::ExitCode WebSocket::EvaluateBackendRequest(const RestInputStruct &IN, QJsonObject *out){
  /*Inputs:
	"namesp" - namespace for the request
	"name" - name of the request
	"args" - JSON input arguments structure
	"out" - JSON output arguments structure
  */
  //qDebug() << "Evaluate Backend Request:" << IN.namesp << IN.name << IN.id << IN.args << IN.fullaccess;
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
  if(namesp=="rpc" && name=="settings"){
    return EvaluateSysadmSettingsRequest(IN.args, out);
  }else if(namesp=="rpc" && name=="logs"){
    return EvaluateSysadmLogsRequest(IN.fullaccess, IN.args, out);
  }else if(namesp=="rpc" && name=="dispatcher"){
    return EvaluateDispatcherRequest(IN.fullaccess, IN.args, out);
  }else if(namesp=="sysadm" && name=="beadm"){
    return EvaluateSysadmBEADMRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="fs"){
    return EvaluateSysadmFSRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="iocage"){
    return EvaluateSysadmIocageRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="iohyve"){
    return EvaluateSysadmIohyveRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="lifepreserver"){
    return EvaluateSysadmLifePreserverRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="network"){
    return EvaluateSysadmNetworkRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="systemmanager"){
    return EvaluateSysadmSystemMgmtRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="update"){
    return EvaluateSysadmUpdateRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="zfs"){
    return EvaluateSysadmZfsRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="pkg"){
    return EvaluateSysadmPkgRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="users"){
    return EvaluateSysadmUserRequest(IN.fullaccess, AUTHSYSTEM->userForToken(SockAuthToken), IN.args, out);
  }else if(namesp=="sysadm" && name=="services"){
    return EvaluateSysadmServiceRequest(IN.args, out);
  }else if(namesp=="sysadm" && name=="firewall"){
    return EvaluateSysadmFirewallRequest(IN.args, out);
  }else{
    return RestOutputStruct::BADREQUEST;
  }

}

// === SYSADM SSL SETTINGS ===
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmSettingsRequest(const QJsonValue in_args, QJsonObject *out){
  //qDebug() << "sysadm/settings Request:" << in_args;
  if(!in_args.isObject()){ return RestOutputStruct::BADREQUEST; }
  QJsonObject argsO = in_args.toObject();
  QStringList keys = argsO.keys();
  //qDebug() << " - keys:" << keys;
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
  }else if(act=="list_ssl_checksums"){
    AUTHSYSTEM->ListCertificateChecksums(out);
    ok = true;
  }else if(act=="revoke_ssl_cert" && keys.contains("pub_key") ){
    //Additional arguments: "user" (optional), "pub_key" (String)
    QString user; if(keys.contains("user")){ user = argsO.value("user").toString(); }
    ok = AUTHSYSTEM->RevokeCertificate(SockAuthToken,argsO.value("pub_key").toString(), user);
  }

  if(ok){ return RestOutputStruct::OK; }
  else{ return RestOutputStruct::BADREQUEST; }
}

// === sysadm/logs ===
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmLogsRequest(bool allaccess, const QJsonValue in_args, QJsonObject *out){
  if(!in_args.isObject() || !in_args.toObject().contains("action") ){ return RestOutputStruct::BADREQUEST; }
  QString act = in_args.toObject().value("action").toString().toLower();
  QJsonObject obj = in_args.toObject();
  //Determine the type of action to perform
  if(act=="read_logs"){
    // OPTIONAL ARGUMENTS:
    // "logs" : <string or array of strings> Possible Values: "hostinfo", "dispatcher", "events-dispatcher", "events-lifepreserver", "events-state";
    // "time_format" : "<format>" Possible Values: "time_t_seconds", "epoch_mseconds", "relative_[day/month/second]", "<QDateTime String format>"
    //  See (http://doc.qt.io/qt-5/qdatetime.html#fromString) for details on the QDateTime String format codes
    // "start_time" : "<number>" (according to format specified)
    // "end_time" : "<number>" (according to format specified)

    //First figure out which logs to read
    QStringList logs;
    if(obj.contains("logs")){
      if(obj.value("logs").isString()){ logs << obj.value("logs").toString(); }
      else if(obj.value("logs").isArray()){ logs = JsonArrayToStringList(obj.value("logs").toArray()); }
    }
    if(logs.isEmpty()){
      //Use all logs if no particular one(s) are specified
      logs << "hostinfo" << "dispatcher" << "events-dispatcher" << "events-lifepreserver" << "events-state";
    }
    //Get the time range for the logs
    QString format = obj.value("time_format").toString();
    QDateTime endtime = QDateTime::currentDateTime();
    QDateTime starttime = endtime.addSecs( -3600*12); //12 hours back by default
    if(!format.isEmpty()){
      QString str_endtime = obj.value("end_time").toString();
      QString str_starttime = obj.value("start_time").toString();
      if(!str_endtime.isEmpty()){
        if(format=="time_t_seconds"){ endtime = QDateTime::fromTime_t(str_endtime.toInt()); }
        else if(format=="epoch_mseconds"){ endtime = QDateTime::fromMSecsSinceEpoch(str_endtime.toInt()); }
        else if(format=="relative_day"){ endtime = endtime.addDays( 0-qAbs(str_endtime.toInt()) ); }
        else if(format=="relative_month"){ endtime = endtime.addMonths( 0-qAbs(str_endtime.toInt()) ); }
        else if(format=="relative_second"){ endtime = endtime.addSecs( 0-qAbs(str_endtime.toInt()) ); }
        else{ endtime = QDateTime::fromString(str_endtime, format); }
      }
      if(!str_starttime.isEmpty()){
        if(format=="time_t_seconds"){ starttime = QDateTime::fromTime_t(str_starttime.toInt()); }
        else if(format=="epoch_mseconds"){ starttime = QDateTime::fromMSecsSinceEpoch(str_starttime.toInt()); }
        else if(format=="relative_day"){ starttime = endtime.addDays( 0-qAbs(str_starttime.toInt()) ); }
        else if(format=="relative_month"){ starttime = endtime.addMonths( 0-qAbs(str_starttime.toInt()) ); }
        else if(format=="relative_second"){ starttime = endtime.addSecs( 0-qAbs(str_starttime.toInt()) ); }
        else{ starttime = QDateTime::fromString(str_starttime, format); }
      }
    }
    //Now read/return the logs
    for(int i=0; i<logs.length(); i++){
      int log = -1; //this needs to correspond to the LogManager::LOG_FILE enumeration
      if(logs[i]=="hostinfo"){ log = 0; }
      else if(logs[i]=="dispatcher"){ log = 1; }
      else if(logs[i]=="events-dispatcher"){ log = 2; }
      else if(logs[i]=="events-lifepreserver"){ log = 3; }
      else if(logs[i]=="events-state"){ log = 4; }
      
      if(log>=0){
	QStringList info = LogManager::readLog( (LogManager::LOG_FILE)(log), starttime, endtime);
	//REMINDER of format: "[datetime]<message>"
        if(info.isEmpty()){ continue; } //nothing here
        QJsonObject lobj;
        for(int j=0; j<info.length(); j++){
          if(log>=2){
	    //event logs - message is JSON data
            lobj.insert(info[j].section("]",0,0).section("[",1,1), QJsonDocument::fromJson( info[j].section("]",1,-1).toLocal8Bit() ).object() );
          }else{
            //Simple text log
            lobj.insert(info[j].section("]",0,0).section("[",1,1), info[j].section("]",1,-1));
          }
        }//end loop over log info
        out->insert( logs[i], lobj);
      }
    }//end loop over log types
  }else{
    return RestOutputStruct::BADREQUEST;
  }

  //Return Success
  return RestOutputStruct::OK;
}

//==== DISPATCHER ====
RestOutputStruct::ExitCode WebSocket::EvaluateDispatcherRequest(bool allaccess, const QJsonValue in_args, QJsonObject *out){
  //dispatcher only needs a list of sub-commands at the moment (might change later)
  if(!in_args.isObject() || !in_args.toObject().contains("action") ){ return RestOutputStruct::BADREQUEST; }
  QString act = in_args.toObject().value("action").toString().toLower();

  //Determine the type of action to perform
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
  }else if(act=="list"){
    QJsonObject info = DISPATCHER->listJobs();
    out->insert("jobs", info);
  }else if(act=="kill" && in_args.toObject().contains("job_id") ){
    if(!allaccess){ return RestOutputStruct::FORBIDDEN; } //this user does not have permission to modify jobs
    QStringList ids;
    QJsonValue val = in_args.toObject().value("job_id");
    if(val.isArray()){ ids = JsonArrayToStringList(val.toArray()); }
    else if(val.isString()){ ids << val.toString(); }
    else{ return RestOutputStruct::BADREQUEST; }
    out->insert("killed", DISPATCHER->killJobs(ids));
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

//==== SYSADM -- FS ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmFSRequest(const QJsonValue in_args, QJsonObject *out){
  if(in_args.isObject()){
    QStringList keys = in_args.toObject().keys();
    bool ok = false;
    if(keys.contains("action")){
      QString act = JsonValueToString(in_args.toObject().value("action")).toLower();
      if(act=="dirlist"){
	ok = true;
        out->insert("dirlist", sysadm::FS::list_dir(in_args.toObject()));
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
      if(act=="createsnap"){
	ok = true;
        out->insert("createsnap", sysadm::LifePreserver::createSnapshot(in_args.toObject()));
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
        bool fastcheck = true;
        fastcheck = in_args.toObject().value("force").toString().toLower()!="true";
        out->insert("checkupdates", sysadm::Update::checkUpdates(fastcheck));
	      
      }else if(act=="listbranches"){
	ok = true;
        out->insert("listbranches", sysadm::Update::listBranches());
	      
      }else if(act=="startupdate"){
	ok = true;
	out->insert("startupdate", sysadm::Update::startUpdate(in_args.toObject()) );

      }else if(act=="stopupdate"){
	ok = true;
	out->insert("stopupdate", sysadm::Update::stopUpdate() );

      }else if(act=="listsettings"){
	ok = true;
	out->insert("listsettings", sysadm::Update::readSettings() );

      }else if(act=="changesettings"){
	ok = true;
	out->insert("changesettings", sysadm::Update::writeSettings(in_args.toObject()) );
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
  if( ! in_args.isObject()){
    return RestOutputStruct::BADREQUEST;
  }
  QStringList keys = in_args.toObject().keys();
  bool ok = false;
  if(keys.contains("action")) {
    QString act = JsonValueToString(in_args.toObject().value("action"));
    if(act=="list_pools"){
      ok = true;
      QJsonObject pools = sysadm::ZFS::zpool_list();
      if(!pools.isEmpty()){ out->insert("list_pools",pools); }
    }
    else if(act=="datasets"){
      ok = true;
      out->insert("datasets", sysadm::ZFS::zfs_list(in_args.toObject()));
    }
  } //end of "action" key usage

  //If nothing done - return the proper code
  if(!ok){
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
  //OPTIONAL: "category" (only used if "pkg_origins" is not specified)
  QString cat;
  if(in_args.toObject().contains("category")){ cat = in_args.toObject().value("category").toString(); }
  //OPTIONAL: "pkg_origins" (defaults to everything for listing functions)
  QStringList pkgs;
  if(in_args.toObject().contains("pkg_origins")){
    if(in_args.toObject().value("pkg_origins").isString()){ pkgs << in_args.toObject().value("pkg_origins").toString(); }
    else if(in_args.toObject().value("pkg_origins").isArray()){ pkgs = JsonArrayToStringList(in_args.toObject().value("pkg_origins").toArray()); }
  }
    
  //Parse  the action and perform accordingly
  if(act=="pkg_info"){
    //OPTIONAL: "pkg_origins" OR "category"
    //OPTIONAL: "repo"
    //OPTIONAL: "result" = "full" or "simple" (Default: "simple")
    bool fullresults = false; 
    if(in_args.toObject().contains("result")){ fullresults = (in_args.toObject().value("result").toString()=="full"); }
    
    //Now run the info fetch routine
    QJsonObject info = sysadm::PKG::pkg_info(pkgs, repo, cat, fullresults);
    if(!info.isEmpty()){ out->insert("pkg_info",info); }
    else{ return RestOutputStruct::NOCONTENT; }
    
  }else if(act=="pkg_search" && in_args.toObject().contains("search_term")){
    //REQUIRED: "search_term" (string to search for)
    //OPTIONAL: "repo"
    //OPTIONAL: "category"
    //OPTIONAL: "search_excludes" (array of string or single string);
    QString srch = in_args.toObject().value("search_term").toString();
    if(srch.isEmpty()){ return RestOutputStruct::BADREQUEST; }
    QStringList exclude;
    if(in_args.toObject().contains("search_excludes")){
      if(in_args.toObject().value("search_excludes").isString()){ exclude << in_args.toObject().value("search_excludes").toString(); }
      else if(in_args.toObject().value("search_excludes").isArray()){ exclude = JsonArrayToStringList( in_args.toObject().value("search_excludes").toArray() ); }
    }
    QStringList pkgs = sysadm::PKG::pkg_search(repo, srch, exclude, cat);
    if(!pkgs.isEmpty()){
      QJsonObject info = sysadm::PKG::pkg_info(pkgs, repo, cat, false); //always do simple results for a search
      info.insert("results_order", QJsonArray::fromStringList(pkgs));
      if(!info.isEmpty()){ out->insert("pkg_search",info); }
    }else{
      return RestOutputStruct::NOCONTENT;
    }
    
  }else if(act=="list_categories"){
    //OPTIONAL: "repo"
    QJsonArray cats = sysadm::PKG::list_categories(repo);
    if(!cats.isEmpty()){ out->insert("list_categories", cats); }
    else{ return RestOutputStruct::NOCONTENT; }
    
  }else if(act=="list_repos"){
    QJsonArray repos = sysadm::PKG::list_repos();
    if(!repos.isEmpty()){ out->insert("list_repos", repos); }
    else{ return RestOutputStruct::NOCONTENT; }
    
  }else if(act=="pkg_install" && !pkgs.isEmpty() ){
    //REQUIRED: "pkg_origins"
    //OPTIONAL: "repo" (pkg will determine the best repo to use if not supplied)
    out->insert("pkg_install", sysadm::PKG::pkg_install(pkgs,repo));
  }else if(act=="pkg_remove" && !pkgs.isEmpty() ){
    //REQUIRED: "pkg_origins"
    //OPTIONAL: "recursive"="true" or "false" (default: "true")
    bool recursive = true;
    if(in_args.toObject().contains("recursive")){ recursive = in_args.toObject().value("recursive").toString()!="false"; }
    out->insert("pkg_remove", sysadm::PKG::pkg_remove(pkgs, recursive));
  }else if(act=="pkg_lock" && !pkgs.isEmpty() ){
    //REQUIRED: "pkg_origins"
    out->insert("pkg_lock", sysadm::PKG::pkg_lock(pkgs));	 
  }else if(act=="pkg_unlock" && !pkgs.isEmpty() ){
    //REQUIRED: "pkg_origins"
    out->insert("pkg_unlock", sysadm::PKG::pkg_unlock(pkgs));	  
  }else if(act=="pkg_update"){
    //OPTIONAL: "force" = ["true"/"false"]  (default: "false")
    bool force = false;
    if(in_args.toObject().contains("force")){ force = in_args.toObject().value("force").toString()=="true"; }
    out->insert("pkg_update", sysadm::PKG::pkg_update(force));
  }else if(act=="pkg_check_upgrade"){
    out->insert("pkg_check_upgrade", sysadm::PKG::pkg_check_upgrade());
  }else if(act=="pkg_upgrade"){
    out->insert("pkg_upgrade", sysadm::PKG::pkg_upgrade());
  }else if(act=="pkg_audit"){
    out->insert("pkg_audit", sysadm::PKG::pkg_audit());
  }else if(act=="pkg_autoremove"){
    out->insert("pkg_autoremove", sysadm::PKG::pkg_autoremove());
  }else{
    //unknown action
    return RestOutputStruct::BADREQUEST;
  }
  
  return RestOutputStruct::OK;
}

// ==== SYSADM USER API ====
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmUserRequest(bool allaccess, QString user, const QJsonValue in_args, QJsonObject *out){
  bool ok = false;
  //REQUIRED: "action"
  QString action =in_args.toObject().value("action").toString().toLower();
  if(action=="usershow"){
    ok = sysadm::UserManager::listUsers(out, allaccess, user);

  }else if(action=="useradd" && allaccess){ //requires all access to create new users
    ok = sysadm::UserManager::addUser(out, in_args.toObject());

  }else if(action=="userdelete" && allaccess){ //requires all access to remove users
    //REQUIRED: "name"
    //OPTIONAL: "clean_home"="false" (true by default)
    QString deluser = in_args.toObject().value("name").toString();
    if(deluser != user){ //cannot delete the currently-used user
      bool clean = true;
      if(in_args.toObject().contains("clean_home")){ clean = (in_args.toObject().value("clean_home").toString().toLower() != "false"); }
      ok = sysadm::UserManager::removeUser(deluser, clean);
      if(ok){ out->insert("result","success"); }
      else{ out->insert("error","Could not delete user"); }
    }else{
      out->insert("error","Cannot delete the current user");
    }

  }else if(action=="usermod"){
    bool go = true;
    if(!allaccess){
      //ensure that the user being acted on is the current user - otherwise deny access
      go = (in_args.toObject().value("name").toString() == user);
    }
    if(go){ ok = sysadm::UserManager::modifyUser(out, in_args.toObject() ); }

  }else if(action=="groupshow"){
    ok = sysadm::UserManager::listGroups(out, (allaccess ? "" : user) );

  }else if(action=="groupadd" && allaccess){
    ok = sysadm::UserManager::addGroup(out, in_args.toObject() );

  }else if(action=="groupdelete" && allaccess){
    //REQUIRED: "name"
    QString name = in_args.toObject().value("name").toString();
    if(!name.isEmpty()){
      ok = sysadm::UserManager::removeGroup(name);
    }
    if(ok){ out->insert("result","success"); }
    else{ out->insert("error","Could not delete group"); }

  }else if(action=="groupmod" && allaccess){
    ok = sysadm::UserManager::modifyGroup(out, in_args.toObject() );

  }else if(action=="personacrypt_init"){
    qDebug() << "got PC init request:" << in_args << allaccess << user;
    bool go = true;
    if(!allaccess){
      //ensure that the user being acted on is the current user - otherwise deny access
      go = (in_args.toObject().value("name").toString() == user);
    }
    if(go){ 
      //REQUIRED: "name", "password","device"
      QJsonObject obj = in_args.toObject();
      if(obj.contains("name") && obj.contains("password") && obj.contains("device") ){
        ok = sysadm::UserManager::InitializePersonaCryptDevice(obj.value("name").toString(), obj.value("password").toString(), obj.value("device").toString() ); 
        if(ok){ out->insert("result","success"); }
        else{ out->insert("error","Could not initialize Personacrypt device"); }
      }
    }

  }else if(action=="personacrypt_listdevs"){
    QStringList devs = sysadm::UserManager::getAvailablePersonaCryptDevices();
    for(int i=0; i<devs.length(); i++){
      out->insert(devs[i].section(":",0,0), devs[i].section(":",1,-1).simplified()); //<device>:<info>
    }
    ok = true;
  }

  return (ok ? RestOutputStruct::OK : RestOutputStruct::BADREQUEST);
}

// SERVICE MANAGER (sysadm/services)
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmServiceRequest(const QJsonValue in_args, QJsonObject *out){
  bool ok = false;
  QString action = in_args.toObject().value("action").toString();
  sysadm::ServiceManager SMGR;
  if(action=="list_services"){
    QList<sysadm::Service> list = SMGR.GetServices();
    QList<bool> listEnabled = SMGR.isEnabled(list);
    QJsonObject services;
    for(int i=0; i<list.length(); i++){
      QJsonObject S;
      S.insert("name", list[i].Name);
      S.insert("tag", list[i].Tag);
      S.insert("path", list[i].Path);
      S.insert("description", list[i].Description);
      S.insert("is_enabled", listEnabled[i] ? "true" : "false" );
      S.insert("is_running",SMGR.isRunning(list[i]) ? "true" : "false" );
      //S.insert("filename", list[i].Directory);
      //Need to add status info as well (isRunning, isEnabled);
      services.insert(list[i].Name, S);
    }
    ok = true;
    out->insert("services",services);

  }else if(action=="start" && in_args.toObject().contains("services") ){
    QJsonValue sval = in_args.toObject().value("services");
    QStringList services;
    if(sval.isString()){ services << sval.toString(); }
    else if(sval.isArray()){ services = JsonArrayToStringList(sval.toArray()); }
    if(!services.isEmpty()){
      QStringList success;
      ok = true;
      for(int i=0; i<services.length(); i++){
        if( SMGR.Start( SMGR.GetService(services[i]) ) ){ success << services[i]; }
      }
      out->insert("services_started", QJsonArray::fromStringList(success));
    }

  }else if(action=="stop" && in_args.toObject().contains("services") ){
    QJsonValue sval = in_args.toObject().value("services");
    QStringList services;
    if(sval.isString()){ services << sval.toString(); }
    else if(sval.isArray()){ services = JsonArrayToStringList(sval.toArray()); }
    if(!services.isEmpty()){
      QStringList success;
      ok = true;
      for(int i=0; i<services.length(); i++){
        if( SMGR.Stop( SMGR.GetService(services[i]) ) ){ success << services[i]; }
      }
      out->insert("services_stopped", QJsonArray::fromStringList(success));
    }
  }else if(action=="restart" && in_args.toObject().contains("services") ){
    QJsonValue sval = in_args.toObject().value("services");
    QStringList services;
    if(sval.isString()){ services << sval.toString(); }
    else if(sval.isArray()){ services = JsonArrayToStringList(sval.toArray()); }
    if(!services.isEmpty()){
      QStringList success;
      ok = true;
      for(int i=0; i<services.length(); i++){
        if( SMGR.Restart( SMGR.GetService(services[i]) ) ){ success << services[i]; }
      }
      out->insert("services_restarted", QJsonArray::fromStringList(success));
    }
  }else if(action=="enable" && in_args.toObject().contains("services") ){
    QJsonValue sval = in_args.toObject().value("services");
    QStringList services;
    if(sval.isString()){ services << sval.toString(); }
    else if(sval.isArray()){ services = JsonArrayToStringList(sval.toArray()); }
    if(!services.isEmpty()){
      QStringList success;
      ok = true;
      for(int i=0; i<services.length(); i++){
        if( SMGR.Enable( SMGR.GetService(services[i]) ) ){ success << services[i]; }
      }
      out->insert("services_enabled", QJsonArray::fromStringList(success));
    }
  }else if(action=="disable" && in_args.toObject().contains("services") ){
    QJsonValue sval = in_args.toObject().value("services");
    QStringList services;
    if(sval.isString()){ services << sval.toString(); }
    else if(sval.isArray()){ services = JsonArrayToStringList(sval.toArray()); }
    if(!services.isEmpty()){
      QStringList success;
      ok = true;
      for(int i=0; i<services.length(); i++){
        if( SMGR.Disable( SMGR.GetService(services[i]) ) ){ success << services[i]; }
      }
      out->insert("services_disabled", QJsonArray::fromStringList(success));
    }
  }


  if(out->keys().isEmpty()){
    if(ok){ out->insert("result","success"); }
    else{ out->insert("error","error"); }
  }
  return (ok ? RestOutputStruct::OK : RestOutputStruct::BADREQUEST);
}

// FIREWALL MANAGER (sysadm/firewall)
RestOutputStruct::ExitCode WebSocket::EvaluateSysadmFirewallRequest(const QJsonValue in_args, QJsonObject *out){
 bool ok = false;
  QString action = in_args.toObject().value("action").toString();
  sysadm::Firewall FMGR;
  //Now perform actions as needed
  if(action=="known_ports"){
    ok = true;
    QList<sysadm::PortInfo> all = FMGR.allPorts(); //this is all known ports (number/type, name, description) - it does not know about open/closed
    for(int i=0; i<all.length(); i++){
      QJsonObject obj;
        obj.insert("name",all[i].Keyword);
        obj.insert("port", QString::number(all[i].Port)+"/"+all[i].Type);
        if(all[i].Description.isEmpty() && i>0 && (all[i-1].Keyword == all[i].Keyword) ){
          obj.insert("description", all[i-1].Description);
        }else{
          obj.insert("description", all[i].Description);
        }
      out->insert(obj.value("port").toString(), obj); //use the port number/type as the unique identifier
    }

  }else if(action=="list_open"){
    ok = true;
    QList<sysadm::PortInfo> all = FMGR.OpenPorts(); //this is all ports currently opened
    QStringList oports;
    for(int i=0; i<all.length(); i++){
      oports << QString::number(all[i].Port)+"/"+all[i].Type;
    }
    out->insert("openports", QJsonArray::fromStringList(oports));

  }else if(action=="status"){
    ok = true;
    out->insert("is_running", FMGR.IsRunning() ? "true" : "false" );
    out->insert("is_enabled", FMGR.IsEnabled() ? "true" : "false" );

  }else if(action=="open" && in_args.toObject().contains("ports")){
    //REQUIRED: "ports" = [<num>/<type>, <num2>/<type2>, etc..]
    QJsonValue val = in_args.toObject().value("ports");
    QStringList ports;
    QList<sysadm::PortInfo> P;
    if(val.isString()){ ports << val.toString(); }
    else if(val.isArray()){ ports = JsonArrayToStringList(val.toArray()); }
    for(int i=0; i<ports.length(); i++){
      sysadm::PortInfo info = FMGR.LookUpPort(ports[i].section("/",0,0).toInt(), ports[i].section("/",1,1));
      if(info.Port<0 || (info.Type!="tcp" && info.Type!="udp") ){ continue; }
      P << info;
    }
    if(!P.isEmpty()){
      ok = true;
      FMGR.OpenPort(P);
    }

  }else if(action=="close" && in_args.toObject().contains("ports")){
    //REQUIRED: "ports" = [<num>/<type>, <num2>/<type2>, etc..]
    QJsonValue val = in_args.toObject().value("ports");
    QStringList ports;
    QList<sysadm::PortInfo> P;
    if(val.isString()){ ports << val.toString(); }
    else if(val.isArray()){ ports = JsonArrayToStringList(val.toArray()); }
    for(int i=0; i<ports.length(); i++){
      sysadm::PortInfo info = FMGR.LookUpPort(ports[i].section("/",0,0).toInt(), ports[i].section("/",1,1));
      if(info.Port<0 || (info.Type!="tcp" && info.Type!="udp") ){ continue; }
      P << info;
    }
    if(!P.isEmpty()){
      ok = true;
      FMGR.ClosePort(P);
    }

  }else if(action=="start"){
    ok = true;
    FMGR.Start();

  }else if(action=="stop"){
    ok = true;
    FMGR.Stop();

  }else if(action=="restart"){
    ok = true;
    FMGR.Restart();

  }else if(action=="enable"){
    ok = true;
    FMGR.Enable();

  }else if(action=="disable"){
    ok = true;
    FMGR.Disable();

  }else if(action=="reset-defaults"){
   ok = FMGR.RestoreDefaults();

  }


  //Evaluate outputs
  if(out->keys().isEmpty()){
    if(ok){ out->insert("result","success"); }
    else{ out->insert("error","error"); }
  }
  return (ok ? RestOutputStruct::OK : RestOutputStruct::BADREQUEST);
}
