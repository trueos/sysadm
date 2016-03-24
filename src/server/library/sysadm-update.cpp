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
#define UP_RBFILE "/tmp/.rebootRequired"
#define UP_UPFILE "/tmp/.updatesAvailable"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// Return a list of updates available
QJsonObject Update::checkUpdates(bool fast) {
  //NOTE: The "fast" option should only be used for automated/timed checks (to prevent doing this long check too frequently)
  QJsonObject retObject;
	
  //Quick check to ensure the tool is available
  if(!QFile::exists("/usr/local/bin/pc-updatemanager")){ 
    return retObject;
  }
  //Check if the system is waiting to reboot
  if(QFile::exists(UP_RBFILE)){
    retObject.insert("status","rebootrequired");
    if(QFile::exists(UP_UPFILE)){ QFile::remove(UP_UPFILE); } //ensure the next fast update does a full check
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
  //Get the list of deatils from the update checks (fast/full)
  QStringList output;
  if(fast && QFile::exists(UP_UPFILE) && (QFileInfo(UP_UPFILE).lastModified().addSecs(43200)<QDateTime::currentDateTime()) ){
    //Note: The "fast" check will only be used if the last full check was less than 12 hours earlier.
    output = General::readTextFile(UP_UPFILE);
  }else{
    output = General::RunCommand("pc-updatemanager check").split("\n");
    output.append( General::RunCommand("pc-updatemanager pkgcheck").split("\n") );
    General::writeTextFile(UP_UPFILE, output); //save this check for later "fast" updates
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
  return retObject;
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
  } else if ( target == "fbsdupdate" ) {
    flags = "fbsdupdate";
  } else if ( target == "fbsdupdatepkgs" ) {
    flags = "fbsdupdatepkgs";
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

  // Return some details to user that the action was queued
  retObject.insert("command", "pc-updatemanger " + flags);
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}
