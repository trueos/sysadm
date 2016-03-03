// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
// These static classes are for defining custom Dispatcher/Event notifications
//  for individual subsystems
//=================================
#ifndef _PCBSD_SYSADM_DISPATCH_SUBSYSTEM_FILTER_SYSTEM_H
#define _PCBSD_SYSADM_DISPATCH_SUBSYSTEM_FILTER_SYSTEM_H

#include "globals-qt.h"
#include "EventWatcher.h"

static QJsonObject CreateDispatcherEventNotification(QString ID, QJsonObject log){
  //key outputs - need to set these if an event is going to be sent out
  QJsonObject args; //any arguments to send out
  QString namesp, name; //the namespace/name of the subsystem used
  //Quick flags/simplifications for use later
  QString cCmd, cLog; //Current command/log for that command (might be a chain of commands)
  cCmd = log.value("current_cmd").toString(); //This is usually empty if the proc finished
  if(!cCmd.isEmpty()){ cLog = log.value(cCmd).toString(); }
  bool isFinished = log.contains("return_codes/"+cCmd) || cCmd.isEmpty();
  qDebug() << "Check Dispatcher Event:";
  qDebug() << " - RAW LOG:" << log;
  qDebug() << "cCmd:" << cCmd << "cLog:" << cLog << "isFinished:" << isFinished;
  //Now parse the notification based on the dispatch ID or current command
  //NOTE: There might be a random string on the end of the ID (to accomodate similar process calls)
  if(ID.startsWith("sysadm_iohyve")){
    namesp = "sysadm"; name="iohyve";
    //Now perform additional cmd/system based filtering
    if(ID.section("::",0,0)=="sysadm_iohyve_fetch" || cCmd.startsWith("iohyve fetch ")){
      //Do some parsing of the log
      if(isFinished){ 
	args.insert("state","finished");
      }else{ 
	args.insert("state","running");
	args.insert("progress", cLog.section("\n",-1, QString::SectionSkipEmpty)); //send the last line of the fetch
      }
    }
	  
  }
	
  //Now assemble the output as needed
  if(namesp.isEmpty() || name.isEmpty()){ return QJsonObject(); } //no event
  args.insert("event_system",namesp+"/"+name);
  return args;
}


#endif