//===========================================
//  PC-BSD source code
//  Copyright (c) 2016, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QUuid>
#include "sysadm-general.h"
#include "sysadm-iohyve.h"
#include "sysadm-global.h"
#include "globals.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h
// Queue the fetch of an ISO
QJsonObject Iohyve::fetchISO(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("url") ) {
    retObject.insert("error", "Missing required key 'url'");
    return retObject;
  }

  // Get the key values
  QString url = jsin.value("url").toString();

  // Create a unique ID for this queued action
  QString ID = QUuid::createUuid().toString();

  // Queue the fetch action
  DISPATCHER->queueProcess(ID, "iohyve fetch " + url);

  // Return some details to user that the action was queued
  retObject.insert("command", "iohyve fetch " + url);
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}


// List the VMs on the box
QJsonObject Iohyve::listVMs() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iohyve list").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("VMM") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QJsonObject vm;
    QString line = output.at(i).simplified();
    QString guest = line.section(" ", 0, 0);

    vm.insert("vmm", line.section(" ", 1, 1));
    vm.insert("running", line.section(" ", 2, 2));
    vm.insert("rcboot", line.section(" ", 3, 3));
    vm.insert("description", line.section(" ", 4, -1));

    retObject.insert(guest, vm);
  }

  return retObject;
}

// Rename an ISO file
QJsonObject Iohyve::renameISO(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("source") || ! keys.contains("target") ) {
    retObject.insert("error", "Missing required key(s) 'source / target'");
    return retObject;
  }

  // Get the key values
  QString source = jsin.value("source").toString();
  QString target = jsin.value("target").toString();

  QStringList output = General::RunCommand("iohyve renameiso " + source + " " + target).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot open") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("source", source);
  retObject.insert("target", target);
  return retObject;
}

// Remove an ISO file
QJsonObject Iohyve::rmISO(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("target") ) {
    retObject.insert("error", "Missing required key 'target'");
    return retObject;
  }

  // Get the key values
  QString target = jsin.value("target").toString();

  QStringList output = General::RunCommand("iohyve rmiso " + target).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot open") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }
  retObject.insert("target", target);
  return retObject;
}

// setup iohyve
QJsonObject Iohyve::setupIohyve(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("pool") || ! keys.contains("nic") ) {
    retObject.insert("error", "Missing required key(s) 'pool / nic'");
    return retObject;
  }

  // Get the key values
  QString pool = jsin.value("pool").toString();
  QString nic = jsin.value("nic").toString();

  // Enable the rc.conf values
  if ( ! General::setConfFileValue("/etc/rc.conf", "iohyve_flags=", "iohyve_flags=\"kmod=1 net=" + nic  + "\"", -1 ) ) {
    retObject.insert("error", "Failed enabling rc.conf iohyve_flags");
    return retObject;
  }

  // Do the setup right now
  QStringList output = General::RunCommand("iohyve setup pool=" + pool + " kmod=1 net=" + nic).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot create") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("pool", pool);
  retObject.insert("nic", nic);
  return retObject;
}
