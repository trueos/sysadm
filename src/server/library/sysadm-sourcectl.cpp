//===========================================
//  TrueOS source code
//  Copyright (c) 2017, JT (q5sys)
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-sourcectl.h"
#include "sysadm-global.h"
#include "globals.h"

#include <QUuid>
#include "sysadm-general.h"
#include "sysadm-update.h"
#include "sysadm-global.h"
#include "globals.h"

using namespace sysadm;

// ==================
//  INLINE FUNCTIONS
// ==================

// =================
//  MAIN FUNCTIONS
// =================

QJsonObject sourcectl::downloadsource(){
    // cmd that will be run = git clone https://github.com/trueos/freebsd.git /usr/src
  QJsonObject retObject;
  QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
  // Queue the update action
  QString cmd;
  cmd = "git clone https://github.com/trueos/freebsd.git /usr/src";
  DISPATCHER->queueProcess("sysadm_sourcectl_downloadsource::"+ID, cmd);

  // Return some details to user that the action was queued
  retObject.insert("command", "Downloading TrueOS Source Tree");
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}

QJsonObject sourcectl::updatesource(){
    // cmd that will be run = git reset --hard && git pull
  QJsonObject retObject;
  QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
  // Queue the update action
  QStringList cmds;
  cmds << "git reset --hard" << "git pull";
  DISPATCHER->queueProcess("sysadm_sourcectl_updatesource::"+ID, cmds, "/usr/src/");

  // Return some details to user that the action was queued
  retObject.insert("command", "Updating TrueOS Source Tree");
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}

QJsonObject sourcectl::deletesource(){
    // cmd that will be run = rm -rf /usr/src/
    QJsonObject retObject;
    QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
    // Queue the update action
    QString cmd;
    cmd = "rm -rf /usr/src/";
    DISPATCHER->queueProcess("sysadm_sourcectl_deletesource::"+ID, cmd);

    // Return some details to user that the action was queued
    retObject.insert("command", "Deleting TrueOS Source Tree");
    retObject.insert("comment", "Task Queued");
    retObject.insert("queueid", ID);
    return retObject;
}

QJsonObject sourcectl::stopsource(){}

QJsonObject sourcectl::downloadports(){
    // cmd that will be run = git clone https://github.com/trueos/freebsd-ports.git /usr/ports
    QJsonObject retObject;
    QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
    // Queue the update action
    QString cmd;
    cmd = "git clone https://github.com/trueos/freebsd-ports.git /usr/ports";
    DISPATCHER->queueProcess("sysadm_sourcectl_downloadports::"+ID, cmd);

    // Return some details to user that the action was queued
    retObject.insert("command", "Downloading TrueOS PortsTree");
    retObject.insert("comment", "Task Queued");
    retObject.insert("queueid", ID);
    return retObject;
}
QJsonObject sourcectl::updateports(){
    // cmd that will be run = git reset --hard && git pull
  QJsonObject retObject;
  QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
  // Queue the update action
  QStringList cmds;
  cmds << "git reset --hard" << "git pull";
  DISPATCHER->queueProcess("sysadm_sourcectl_updateports::"+ID, cmds, "/usr/ports/");

  // Return some details to user that the action was queued
  retObject.insert("command", "Updating TrueOS PortsTree");
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}


QJsonObject sourcectl::deleteports(){
    // cmd that will be run = rm -rf /usr/ports/
  QJsonObject retObject;
  QString ID = QUuid::createUuid().toString();   // Create a unique ID for this queued action
  // Queue the update action
  QString cmd;
  cmd = "rm -rf /usr/ports/";
  DISPATCHER->queueProcess("sysadm_sourcectl_deleteports::"+ID, cmd);

  // Return some details to user that the action was queued
  retObject.insert("command", "Deleting TrueOS PortsTree");
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}

QJsonObject sourcectl::stopports(){}


void sourcectl::saveSourceLog(QString){}
void sourcectl::savePortsLog(QString){}




/*

git clone https://github.com/trueos/freebsd.git /usr/src
git clone https://github.com/trueos/freebsd-ports.git /usr/ports

git reset --hard && git pull


rm -rf /usr/src/
rm -rf /usr/ports/



QJsonObject SysMgmt::fetchPortsTree(QString altDir){
//void SysMgmt::fetchPortsTree(QStringList &cmds, QStringList &dirs){
 QJsonObject out;
  if(altDir.isEmpty()){ altDir = "/usr/ports"; }
  //Does Ports tree exist?  If not create it.
  if(!QFile::exists(altDir)){
    QDir dir;
    if(!dir.mkpath(altDir) ){
     out.insert("error","Could not create directory: "+altDir);
     return out;
    }
  }
  //Does a local git repo exist? If not create it.
  QString URL = "https://www.github.com/trueos/freebsd-ports.git";
  if(QFile::exists(altDir+"/.git")){
    //Check if the remote URL is correct
    QString origin = General::gitCMD(altDir, "git remote show -n origin").filter("Fetch URL:").join("").section("URL:",1,30).simplified();
    if(origin != URL){
      General::gitCMD(altDir,"git",QStringList() << "remote" << "remove" << "origin");
      General::gitCMD(altDir,"git", QStringList() << "remote" << "add" << "origin" << URL);
    }
  }else{
    //new GIT setup
    General::emptyDir(altDir);
    General::gitCMD(altDir, "git", QStringList() << "init" << altDir );
    General::gitCMD(altDir, "git", QStringList() << "remote" << "add" << "origin" << URL );
  }
  //Now update the tree with git
  QString ID = "system_fetch_ports_tree";
  DISPATCHER->queueProcess(ID, "git pull origin", altDir );
  out.insert("result","process_started");
  out.insert("process_id",ID);
  return out;
}

/*void SysMgmt::fetchSourceTree(QString branch, QStringList &cmds, QStringList &dirs, QStringList &info){
  //Clear the output variables
  cmds.clear(); dirs.clear();
  //Check if the source directory even exists
  if(!QFile::exists("/usr/src")){
    cmds << "mkdir /usr/src"; dirs << ""; //Create the ports tree
  }
  //Now check if the git directory exists
  QString URL = "https://www.github.com/pcbsd/freebsd.git";
  if(QFile::exists("/usr/src/.git")){
    //Check if the remote URL is correct
    QString origin = General::gitCMD("/usr/src", "git remote show -n origin").filter("Fetch URL:").join("").section("URL:",1,30).simplified();
    if(origin != URL){
      cmds << "git remote remove origin"; dirs <<"/usr/src";
      cmds << "git remote add origin "+URL; dirs << "/usr/src/.git"; //setup PC-BSD git repo
    }
  }else{
    //new GIT setup
    General::emptyDir("/usr/src");
    cmds << "git init"; dirs << "/usr/src"; //setup git
    cmds << "git remote add origin "+URL; dirs << "/usr/src/.git"; //setup PC-BSD git repo
  }
  //Now update the tree with git
  cmds << "git fetch --depth=1"; dirs << "/usr/src/.git";
  cmds << "git checkout "+branch; dirs << "/usr/src";
}
*/
