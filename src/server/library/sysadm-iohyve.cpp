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

// Add a new disk for a VM
QJsonObject Iohyve::addDisk(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") || ! keys.contains("size") ) {
    retObject.insert("error", "Missing required key(s) 'name', 'size'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();
  QString size = jsin.value("size").toString();
  QString pool = jsin.value("pool").toString();

  QStringList output = General::RunCommand("iohyve", QStringList() << "add" << name << size << pool).split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot create") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  QJsonObject vm;
  vm.insert("size", size);
  if ( ! pool.isEmpty() )
   vm.insert("pool", pool);
  retObject.insert(name, vm);

  return retObject;
}

// Create a new guest VM
QJsonObject Iohyve::createGuest(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") || !keys.contains("size") ) {
    retObject.insert("error", "Missing required key(s) 'name/size'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();
  QString size = jsin.value("size").toString();

  QStringList output = General::RunCommand("iohyve", QStringList() << "create" <<  name << size).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot create") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  // Return some details to user that the action was queued
  retObject.insert("name", name);
  retObject.insert("size", size);
  return retObject;
}

// Delete a guest disk
QJsonObject Iohyve::deleteDisk(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") || !keys.contains("disk") ) {
    retObject.insert("error", "Missing required key(s) 'name/disk'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();
  QString disk = jsin.value("disk").toString();

  // Can't remove disk0
  if ( disk == "disk0" ) {
    retObject.insert("error", "disk0 cannot be removed!");
    return retObject;
  }

  // Remove the disk now
  QStringList output = General::RunCommand("iohyve", QStringList() << "remove" << "-f" << name << disk).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    // This doesn't work, iohyve doesn't return error message right now
    if ( output.at(i).indexOf("No such guest") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
    if ( output.at(i).indexOf("Not") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("name", name);
  retObject.insert("disk", disk);
  return retObject;
}

// Delete a guest
QJsonObject Iohyve::deleteGuest(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  // Do the stop right now
  QStringList output = General::RunCommand("iohyve", QStringList() << "delete" << "-f" << name).split("\n");
  qDebug() << output;
  for ( int i = 0; i < output.size(); i++)
  {
    // This doesn't work, iohyve doesn't return error message right now
    if ( output.at(i).indexOf("No such guest") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("name", name);
  return retObject;
}

// Get all the properties for a guest
QJsonObject Iohyve::getProps(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  QStringList output = General::RunCommand("iohyve", QStringList() << "getall" << name).split("\n");

  QJsonObject props;

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("Getting ") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QString line = output.at(i).simplified();
    QString prop = line.section(" ", 0, 0);
    QString val = line.section(" ", 1, 1);
    props.insert(prop, val);
  }

  retObject.insert(name, props);
  return retObject;
}


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
  QString ID = "sysadm_iohyve_fetch::"+QUuid::createUuid().toString();

  // Queue the fetch action
  DISPATCHER->queueProcess(ID, "iohyve fetch " + url);

  // Return some details to user that the action was queued
  retObject.insert("command", "iohyve fetch " + url);
  retObject.insert("comment", "Task Queued");
  retObject.insert("queueid", ID);
  return retObject;
}

// Create a new guest VM
QJsonObject Iohyve::installGuest(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") || !keys.contains("iso") ) {
    retObject.insert("error", "Missing required key(s) 'name/iso'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();
  QString iso = jsin.value("iso").toString();

  QStringList output = General::RunCommand("iohyve", QStringList() << "install" << name << iso).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("Could not open") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  // Return some details to user that the action was queued
  retObject.insert("name", name);
  retObject.insert("iso", iso);
  return retObject;
}

// Return if iohyve is setup on the box
QJsonObject Iohyve::isSetup() {
  QJsonObject retObject;

  // Check if iohyve is setup on this box
  // We check the flags variable, enabling / disabling is done via service mgmt
  QString ioflags = General::getConfFileValue("/etc/rc.conf", "iohyve_flags=", 1);
  if ( ioflags.isEmpty() )
    retObject.insert("setup", "false");
  else
    retObject.insert("setup", "true");

  return retObject;
}

// List the disks for a VM
QJsonObject Iohyve::listDisks(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  QStringList output = General::RunCommand("iohyve", QStringList() << "disks" << name).split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("Listing") != -1 )
      continue;
    if ( output.at(i).indexOf("diskN") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QJsonObject vm;
    QString line = output.at(i).simplified();
    QString disk = line.section(" ", 0, 0);
    QString size = line.section(" ", 1, 1);
    retObject.insert(disk, size);
  }

  return retObject;
}

// List the VMs on the box
QJsonObject Iohyve::listVMs() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iohyve", QStringList() << "list").split("\n");

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

// List the ISOs on the box
QJsonArray Iohyve::listISOs(){
  QJsonArray arr;
  QStringList output = General::RunCommand("iohyve", QStringList() << "isolist").split("\n");
  for(int i=1; i<output.length(); i++){ //first line is headers
    if(output[i].isEmpty()){ continue; }
    arr.append(output[i]);
  }
  return arr;
  
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

  QStringList output = General::RunCommand("iohyve", QStringList() << "renameiso" << source << target).split("\n");
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

// Resize a guest disk
QJsonObject Iohyve::resizeDisk(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") || !keys.contains("disk") || !keys.contains("size") ) {
    retObject.insert("error", "Missing required key(s) 'name/disk/size'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();
  QString disk = jsin.value("disk").toString();
  QString size = jsin.value("size").toString();

  // Resize the disk now
  QStringList output = General::RunCommand("iohyve", QStringList() << "resize" << name << disk << size).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    // This doesn't work, iohyve doesn't return error message right now
    if ( output.at(i).indexOf("Please stop") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
    if ( output.at(i).indexOf("Not a valid") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("name", name);
  retObject.insert("disk", disk);
  retObject.insert("size", size);
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

  QStringList output = General::RunCommand("iohyve", QStringList() << "rmiso" << target).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("cannot open") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
    if ( output.at(i).indexOf("cannot destroy") != -1 ) {
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
  QStringList output = General::RunCommand("iohyve", QStringList() << "setup" << "pool=" + pool << "kmod=1" << "net=" + nic).split("\n");
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

// Set properties
QJsonObject Iohyve::setProp(QJsonObject jsin) {
  QJsonObject retObject;
  QJsonObject props;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  // Load the supplied settings
  QStringList settings;
  QStringList values;

  for ( int i = 0; i < keys.size(); i++)
  {
    if ( keys.at(i) == "name" )
      continue;
    if ( keys.at(i) == "action" )
      continue;
   
    settings << keys.at(i);
    values << jsin.value(keys.at(i)).toString();
    props.insert(keys.at(i), jsin.value(keys.at(i)).toString());
  }

  if ( settings.isEmpty() ) {
    retObject.insert("error", "No settings supplied!");
    return retObject;
  }

  QStringList setargs;
  setargs << "set" << name;
  for ( int i = 0; i < settings.size(); i++)
  {
    setargs << settings.at(i) + "=" + values.at(i);
  }

  // Set it now
  QString output = General::RunCommand("iohyve", setargs);
  retObject.insert(name, props);
  return retObject;
}


// Start a guest
QJsonObject Iohyve::startGuest(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  // Do the setup right now
  QStringList output = General::RunCommand("iohyve", QStringList() << "start" << name).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("Not a valid") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("name", name);
  return retObject;
}

// Stop a guest
QJsonObject Iohyve::stopGuest(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("name") ) {
    retObject.insert("error", "Missing required key 'name'");
    return retObject;
  }

  // Get the key values
  QString name = jsin.value("name").toString();

  QString stoparg = "stop";
  if (! keys.contains("force") ) {
    if ( jsin.value("force").toString() == "true" ) {
      stoparg = "forcekill";
    }
  }

  // Do the stop right now
  QStringList output = General::RunCommand("iohyve", QStringList() << stoparg << name).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    // This doesn't work, iohyve doesn't return error message right now
    if ( output.at(i).indexOf("No such guest") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("name", name);
  return retObject;
}

// List the version of iohyve
QJsonObject Iohyve::version() {
  QJsonObject retObject;
  QString output = General::RunCommand("iohyve", QStringList() << "version").simplified();
  retObject.insert("version", output);
  return retObject;
}

