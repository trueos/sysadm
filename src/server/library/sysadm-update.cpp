//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QUuid>
#include "sysadm-general.h"
#include "sysadm-update.h"
#include "sysadm-global.h"
#include "globals.h"

#define UP_PIDFILE "/tmp/.updateInProgress"
#define UP_RBFILE "/tmp/.trueos-update-staged"
#define UP_UPFILE "/tmp/.updatesAvailable"

#define UP_CONFFILE "/usr/local/etc/trueos.conf"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

//Return the date/time that the last full check for updates was run
QDateTime Update::lastFullCheck(){
  return QFileInfo(UP_UPFILE).lastModified();
}

QDateTime Update::rebootRequiredSince(){
  if(!QFile::exists(UP_RBFILE)){ return QDateTime(); } //null time - no file exists
  return QFileInfo(UP_RBFILE).lastModified();
}

// Return a list of updates available
QJsonObject Update::checkUpdates(bool fast) {
  //NOTE: The "fast" option should only be used for automated/timed checks (to prevent doing this long check too frequently)
  QJsonObject retObject;
  //qDebug() << "Check for updates: fast=" << fast;
  //Quick check to ensure the tool is available
  if(!QFile::exists("/usr/local/bin/pc-updatemanager")){
    return retObject;
  }
  //See if the check for updates is currently running - just return nothing at the moment for that
  if(DISPATCHER->isJobActive("sysadm_update_checkupdates")){
    retObject.insert("status", "checkingforupdates");
    return retObject;
  }
  //Check if the system is waiting to reboot
  if(QFile::exists(UP_RBFILE)){
    retObject.insert("status","rebootrequired");
    if(QFile::exists(UP_UPFILE)){ QFile::remove(UP_UPFILE); } //ensure the next fast update does a full check
    //Also add the information on what updates are pending
    retObject.insert("details", sysadm::General::readTextFile(UP_RBFILE).join("\n") );
    return retObject;
  }
  //Check if an update is currently running
  if(QFile::exists(UP_PIDFILE)){
    //See if the process is actually running
    if( General::RunQuickCommand(QString("pgrep -F ")+UP_PIDFILE) ){
      //Success if return code == 0
      retObject.insert("status","updaterunning");
      if(QFile::exists(UP_UPFILE)){ QFile::remove(UP_UPFILE); } //ensure the next fast update does a full check
      return retObject;
    }
  }
  //Get the list of details from the update checks (fast/full)
  QStringList output;
  QDateTime cdt = QDateTime::currentDateTime();
  QDateTime fdt = cdt.addDays(-1); //just enough to trip the checks below if needed
  if(QFile::exists(UP_UPFILE)){ fdt = QFileInfo(UP_UPFILE).lastModified(); }
  //qDebug() << "Time Stamps (now/file):" << cdt << fdt;
  //qDebug() << " - File earlier than now:" << (fdt<cdt);
  //qDebug() << " - File +1 day earlier than now (+day):" << (fdt.addDays(1)<cdt);
  //qDebug() << " - File +1 day earlier than now (+secs):" << (fdt.addSecs(24*60)<cdt);
  //qDebug() << " - Seconds from File->Day time:" << fdt.secsTo(cdt);
  int secs = fdt.secsTo(cdt);
  if(fast && (secs<43200) ){
    //Note: The "fast" check will only be used if the last full check was less than 12 hours earlier.
    //qDebug() << " - UseFast Re-read";
    output = General::readTextFile(UP_UPFILE);
  }else if(secs<300 ){
    //Note: This will re-use the previous check if it was less than 5 minutes ago (prevent hammering servers from user checks)
    //qDebug() << " - Use Fast Re-read (failsafe -  less than 5 minute interval)";
    output = General::readTextFile(UP_UPFILE);
  }else{
    //qDebug() << " - Run full check";
    QStringList cmds; cmds << "pc-updatemanager syncconf" << "pc-updatemanager pkgcheck";
    DISPATCHER->queueProcess("sysadm_update_checkupdates", cmds );
    retObject.insert("status", "checkingforupdates");
    //qDebug() << " - Done starting check";
    return retObject;
    /*General::RunCommand("pc-updatemanager syncconf"); //always resync the config file before starting an update check
    output.append( General::RunCommand("pc-updatemanager pkgcheck").split("\n") );
    while(output.last().simplified()==""){ output.removeLast(); }
    if(!output.last().contains("ERROR:")){ //make sure there was network access available first -  otherwise let it try again soon
      General::writeTextFile(UP_UPFILE, output); //save this check for later "fast" updates
    }*/
  }
  //qDebug() << "pc-updatemanager checks:" << output;

  QString nameval;
  int pnum=1;
  for ( int i = 0; i < output.size(); i++)
  {
     if ( output.at(i).indexOf("NAME: ") != -1 )
       nameval = output.at(i).section(" ", 1, 1);

     if ( output.at(i).indexOf("TYPE: SECURITYUPDATE") != -1 ) {
       QJsonObject itemvals;
       itemvals.insert("name", nameval);
       retObject.insert("security", itemvals);
     }
     if ( output.at(i).indexOf("TYPE: SYSTEMUPDATE") != -1 ) {
       QJsonObject itemvals;
       itemvals.insert("name", nameval);
       if ( output.size() > ( i + 1) )
         itemvals.insert("tag", output.at(i+1).section(" ", 1, 1));
       if ( output.size() > ( i + 2) )
         itemvals.insert("version", output.at(i+2).section(" ", 1, 1));
       retObject.insert("majorupgrade", itemvals);
     }
     if ( output.at(i).indexOf("TYPE: PATCH") != -1 ) {
       QJsonObject itemvals;
       itemvals.insert("name", nameval);
       if ( output.size() > ( i + 1) )
         itemvals.insert("tag", output.at(i+1).section(" ", 1, 1));
       if ( output.size() > ( i + 2) )
         itemvals.insert("details", output.at(i+2).section(" ", 1, 1));
       if ( output.size() > ( i + 3) )
         itemvals.insert("date", output.at(i+3).section(" ", 1, 1));
       if ( output.size() > ( i + 4) )
         itemvals.insert("size", output.at(i+4).section(" ", 1, 1));
       retObject.insert("patch"+QString::number(pnum), itemvals);
       pnum++;
     }
     if ( output.at(i).indexOf("TYPE: PKGUPDATE") != -1 ) {
       QJsonObject itemvals;
       itemvals.insert("name", nameval);
       retObject.insert("packageupdate", itemvals);
     }
  }
  if(retObject.isEmpty()){
    retObject.insert("status", "noupdates");
  }else{
    // Update status that we have updates
    retObject.insert("status", "updatesavailable");
  }
  retObject.insert("details", output.join("\n") ); //full details of the check for updates
  retObject.insert("last_check",QFileInfo(UP_UPFILE).lastModified().toString(Qt::ISODate) );
  return retObject;
}

void Update::saveCheckUpdateLog(QString log){
    QStringList output = log.split("\n");
    qDebug() << "Got Check Update Log:" << log << output;
    while(!output.isEmpty() && output.last().simplified().isEmpty()){ output.removeLast(); }
    if(output.isEmpty()){ return; }
    if(!output.last().contains("ERROR:")){ //make sure there was network access available first -  otherwise let it try again soon
      General::writeTextFile(UP_UPFILE, output); //save this check for later "fast" updates
    }
}

// List available branches we can switch to
QJsonObject Update::listBranches() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("pc-updatemanager branches").split("\n");

  bool inSection = false;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("-----------------") != -1 ) {
      inSection = true;
      continue;
    }

    if (!inSection)
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QString tmp = output.at(i).section(" ", 0, 0);
    QString tmp2 = output.at(i).section(" ", 1, 1);
    if ( tmp2 == "*" )
      retObject.insert(tmp, "active");
    else
      retObject.insert(tmp, "available");
  }

  return retObject;
}

// Kickoff an update process
QJsonObject Update::startUpdate(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("target") ) {
    retObject.insert("error", "Missing required key 'target'");
    return retObject;
  }
  // Save the target
  QString target;
  target = jsin.value("target").toString();

  QString flags;
  if ( target == "chbranch" ) {
    if (! keys.contains("branch") ) {
      retObject.insert("error", "Missing required key 'branch'");
      return retObject;
    }
    flags = "chbranch " + jsin.value("branch").toString();
  } else if ( target == "pkgupdate" ) {
    flags = "pkgupdate";
/*  } else if ( target == "fbsdupdate" ) {
    flags = "fbsdupdate";
  } else if ( target == "fbsdupdatepkgs" ) {
    flags = "fbsdupdatepkgs";*/
  } else if ( target == "standalone" ) {
    if (! keys.contains("tag") ) {
      retObject.insert("error", "Missing required key 'tag'");
      return retObject;
    }
    flags = "install " + jsin.value("tag").toString();
  } else {
    // Ruh-roh
    retObject.insert("error", "Unknown target key: " + target);
    return retObject;
  }

  // Create a unique ID for this queued action
  QString ID = QUuid::createUuid().toString();

  // Queue the update action
  DISPATCHER->queueProcess("sysadm_update_runupdates::"+ID, "pc-updatemanager " + flags);

  if(QFile::exists(UP_UPFILE)){ QFile::remove(UP_UPFILE); } //ensure the next fast update does a full check

  // Return some details to user that the action was queued
  retObject.insert("command", "pc-updatemanger " + flags);
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}

// Stop an update process
QJsonObject Update::stopUpdate() {
  //See if the update is running in the dispatcher
  QJsonObject ret;
  QJsonObject cup = DISPATCHER->listJobs();
  QStringList ids = cup.keys().filter("sysadm_update_runupdates::");
  if(!ids.isEmpty()){
    //Found a dispatcher process - go ahead and request that it stop
    DISPATCHER->killJobs(ids);
    ret.insert("result","success");
  }else if( QFile::exists(UP_PIDFILE) ){
    //See if the process is actually running
    if( General::RunQuickCommand(QString("pgrep -F ")+UP_PIDFILE) ){
      //Success if return code == 0
      int rtc = General::RunQuickCommand( QString("pkill -F ")+UP_PIDFILE );
      if(rtc==0){ ret.insert("result","success"); }
      else{ ret.insert("result","error: could not kill update process"); }
    }
  }else{
    ret.insert("result","error: no update processes found");
  }
  return ret;
}

QJsonObject Update::applyUpdates(){
  QJsonObject ret;
  QProcess::startDetached("pc-updatemanager startupdate");
  ret.insert("result","rebooting to complete updates");
  return ret;
}
//SETTINGS OPTIONS
QJsonObject Update::readSettings(){
  QJsonObject ret;
  QStringList knownsettings;
  knownsettings << "PACKAGE_SET" << "PACKAGE_URL" << "AUTO_UPDATE" << "MAXBE" << "AUTO_UPDATE_REBOOT";// << "CDN_TYPE";

  QStringList info = General::readTextFile(UP_CONFFILE);
  for(int i=0; i<info.length(); i++){
    if(info[i].section("#",0,0).simplified().isEmpty()){ continue; } //nothing on this line
    QString line = info[i].section("#",0,0).simplified();
    QString var = line.section(":",0,0).simplified();
    if(knownsettings.contains(var)){
      ret.insert(var.toLower(), line.section(":",1,-1).simplified());
    }
  }
  return ret;
}

QJsonObject Update::writeSettings(QJsonObject obj){
  QJsonObject ret;
  //Check inputs
  QStringList knownsettings;
  knownsettings << "PACKAGE_SET" << "PACKAGE_URL" << "AUTO_UPDATE" << "MAXBE" << "AUTO_UPDATE_REBOOT";// << "CDN_TYPE";
  QStringList keys = obj.keys();
  QStringList vals;
  bool clearlastCheck = false;
  for(int i=0; i<keys.length(); i++){
    if(knownsettings.contains(keys[i].toUpper())){
      if(keys[i].toUpper().startsWith("PACKAGE_")){ clearlastCheck = true; }
      vals << obj.value(keys[i]).toString();
      keys[i] = keys[i].toUpper();
    }else{
      keys.removeAt(i); i--;
    }
  }
  if(keys.isEmpty()){ ret.insert("result","no changes"); return ret; }
  //Note: Now the keys/vals lists are "paired" up, with same size and info for the same index in the list
  //Save Settings
  // - first replace existing variables in the config file
  QStringList info = General::readTextFile(UP_CONFFILE);
  for(int i=0; i<info.length(); i++){
    if(info[i].section("#",0,0).simplified().isEmpty()){ continue; } //nothing on this line
    QString line = info[i].section("#",0,0).simplified();
    QString var = line.section(":",0,0).simplified();
    int index = keys.indexOf(var);
    if(index>=0){
      info[i] = keys[index]+": "+vals[index];
      keys.removeAt(index); vals.removeAt(index);
    }
  }
  // if the variable was not previously defined, just add it to the end
  for(int i=0; i<keys.length(); i++){
    info << keys[i]+": "+vals[i];
  }
  if( General::writeTextFile(UP_CONFFILE, info, true) ){
    QProcess::startDetached("pc-updatemanager syncconf"); //sync up the config files as needed
    ret.insert("result","success");
    if(clearlastCheck){
      if(QFile::exists(UP_UPFILE)){ QFile::remove(UP_UPFILE); } //ensure the next fast update does a full check
    }
  }else{
    ret.insert("result","error");
  }

  return ret;
}

//List/Read update logs
QJsonObject Update::listLogs(){
  QJsonObject out;
  QDir logdir("/var/log");
  QFileInfoList logs = logdir.entryInfoList(QStringList() << "pc-updatemanager*", QDir::Files, QDir::Time);
  for(int i=0; i<logs.length(); i++){
    QJsonObject tmp;
    tmp.insert("started", QString::number( logs[i].created().toTime_t() ) );
    tmp.insert("finished", QString::number( logs[i].lastModified().toTime_t() ) );
    tmp.insert("name", logs[i].fileName());
    out.insert(logs[i].fileName(), tmp);
  }
  return out;
}

QJsonObject Update::readLog(QJsonObject obj){
  QJsonObject ret;
  //Check Inputs
  if(obj.contains("logs")){
    QJsonValue val = obj.value("logs");
    QStringList logs;
    if(val.isString()){ logs << val.toString(); }
    else if(val.isArray()){ logs = General::JsonArrayToStringList(obj.value("logs").toArray()); }
    QString logdir = "/var/log/";
    for(int i=0; i<logs.length(); i++){
      if(!logs[i].startsWith("pc-updatemanager")){ continue; } //wrong type of log file - this only handles pc-updatemanager logs
      QStringList info = General::readTextFile(logdir+logs[i]);
      if(!info.isEmpty()){
        ret.insert(logs[i], info.join("\n"));
      }
    }
  }
  return ret;
}
