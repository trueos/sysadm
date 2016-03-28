// ===============================
// PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
// These classes are for defining custom Dispatcher/Event notifications
//  for individual subsystems
//=================================
#include "globals-qt.h"
#include "EventWatcher.h"
#include "Dispatcher.h"

QJsonObject Dispatcher::CreateDispatcherEventNotification(QString ID, QJsonObject log){
  //key outputs - need to set these if an event is going to be sent out
  QJsonObject args; //any arguments to send out
  QString namesp, name; //the namespace/name of the subsystem used
  //Quick flags/simplifications for use later
  QString cCmd, cLog; //Current command/log for that command (might be a chain of commands)
  cCmd = log.value("current_cmd").toString(); //This is usually empty if the proc finished
  if(cCmd.isEmpty()){ cCmd = log.value("cmd_list").toArray().last().toString(); }
  cLog = log.value(cCmd).toString();
  bool isFinished = (log.value("state").toString()=="finished");
  //qDebug() << "Check Dispatcher Event:";
  //qDebug() << " - RAW LOG:" << log;
  //qDebug() << "cCmd:" << cCmd << "cLog:" << cLog << "isFinished:" << isFinished;
  //Add the generic process values
  args.insert("state",isFinished ? "finished" : "running");
  args.insert("process_details", log); //full process log array here
  
  //Now parse the notification based on the dispatch ID or current command
  //NOTE: There might be a random string on the end of the ID (to accomodate similar process calls)
  // == sysadm/iohyve ==
  if(ID.startsWith("sysadm_iohyve")){
    namesp = "sysadm"; name="iohyve";
    //Now perform additional cmd/system based filtering
    if(ID.section("::",0,0)=="sysadm_iohyve_fetch" || cCmd.startsWith("iohyve fetch ")){
      //Do some parsing of the log
      parseIohyveFetchOutput(cLog,&args);
    }
    
  // == sysadm/update ==
  }else if(ID.startsWith("sysadm_update")){
    namesp = "sysadm"; name="update";
    //No special parsing here: the pc-updatemanager output should be available as-is
    args.insert("update_log",cLog);
	  
	  
  // == sysadm/pkg ==
  }else if(ID.startsWith("sysadm_pkg")){
    namesp = "sysadm"; name="pkg";
    //most pkg commands have no special parsing the pkg output should be available as-is
    args.insert("pkg_log",cLog);
    args.insert("action", ID.section("-",0,0).section("_",1,-1) ); //so the client/user can tell which type of pkg action this is for
    if(ID.section("-",0,0)=="sysadm_pkg_check_upgrade"){
      if(isFinished){
	bool hasupdates = !cLog.contains("Your packages are up to date.");
	args.insert("updates_available", hasupdates ? "true" : "false");
      }
      
    }else if(ID.section("-",0,0)=="sysadm_pkg_audit" && isFinished){
      QStringList info = cLog.split("\n");
      QStringList vuln, effects;
      for(int i=0; i<info.length(); i++){
        if(info[i].startsWith("Packages that depend on ")){
	  vuln << info[i].section(":",0,0).section(" on ",1,1);
	  effects << info[i].section(": ",1,-1).split(", ");
	}
      }
      vuln.removeDuplicates(); vuln.removeAll("");
      effects.removeDuplicates(); effects.removeAll("");
      args.insert("vulnerable_pkgs",QJsonArray::fromStringList(vuln));
      args.insert("impacts_pkgs",QJsonArray::fromStringList(effects));
    }
    
  }
	
  //Now assemble the output as needed
  if(namesp.isEmpty() || name.isEmpty()){ return QJsonObject(); } //no event
  args.insert("event_system",namesp+"/"+name);
  return args;
}

void Dispatcher::parseIohyveFetchOutput(QString outputLog, QJsonObject *out){
  //Note: this is the standard parsing for a "fetch" call
  QStringList lines = outputLog.split("\n", QString::SkipEmptyParts);
  if(lines.isEmpty()){ return; } //nothing to report
  for(int i=lines.length()-1; i>=0; i--){
    qDebug() << "Parsing iohyve fetch line:" << lines[i];
    if(!lines[i].contains("% of ") && !lines[i].endsWith(" Bps") ){ continue; }
    out->insert("filename", lines[i].section("\t",0,0) );
    out->insert("percent_done",lines[i].section("\t",1,1) );
    out->insert("download_rate",lines[i].section("\t",2,-1) );
    break;
  }
}
