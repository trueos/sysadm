//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QUuid>
#include "sysadm-general.h"
#include "sysadm-beadm.h"
#include "sysadm-global.h"
#include "globals.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// List all the available BEs
QJsonObject BEADM::listBEs() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("beadm list -H").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {

    if ( output.at(i).isEmpty() )
      break;

    QString line = output.at(i).simplified();

    QString beName = line.section(" ", 0, 0);
    QString beActive = line.section(" ", 1, 1);
    QString beMount = line.section(" ", 2, 2);
    QString beSpace = line.section(" ", 3, 3);
    QString beDate = line.section(" ", 4, 4);
    QString beNick = line.section(" ", 5, 5);

    QJsonObject vals;
    vals.insert("active", beActive);
    vals.insert("mount", beMount);
    vals.insert("space", beSpace);
    vals.insert("date", beDate);
    vals.insert("nick", beNick);

    retObject.insert(beName, vals);
  }

  return retObject;
}

// Rename the specified source BE to a a new target BE name

QJsonObject BEADM::renameBE(QJsonObject jsin) {
  QJsonObject retObject;
  
  QStringList keys = jsin.keys();
  if (! keys.contains("source") || ! keys.contains("target") ) {
    retObject.insert("error", "Missing required key(s) 'source / target'");
    return retObject;
  }

  // Get the key values
  QString source = jsin.value("source").toString();
  QString target = jsin.value("target").toString();

  QStringList output = General::RunCommand("beadm rename " + source + " " + target).split("\n");
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("ERROR") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }
  
  retObject.insert("source", source);
  retObject.insert("target", target);
  return retObject;
}

// Activate the given BeName for the next boot

QJsonObject BEADM::activateBE(QJsonObject jsin) {
	QJsonObject retObject;

	QStringList keys = jsin.keys();
	if (! keys.contains("target") ) {
		retObject.insert("error", "Missing required key(s) 'target'");
		return retObject;
}

  // Get the key values

  QString target = jsin.value("target").toString();


  QStringList output = General::RunCommand("beadm activate " + target).split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("ERROR") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("target", target);
  return retObject;
}

// createbe  (optional flag -e for cloning from an inactive BE) : Create a new Boot environment

QJsonObject BEADM::createBE(QJsonObject jsin) {
	QJsonObject retObject;

	QStringList keys = jsin.keys();
	if (! keys.contains("newbe") ) {
		retObject.insert("error", "Missing required key(s) 'target'");
		return retObject;
	}

  // Get the key values

  QString newbe = jsin.value("newbe").toString();
  QString flags;
  if ( keys.contains("clonefrom") ) {
    flags  = "-e " + jsin.value("clonefrom").toString();
  }

  QStringList output = General::RunCommand("beadm create " + flags + " " + newbe).split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("ERROR") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("newbe", newbe);
  if ( keys.contains("clonefrom") ) {
    retObject.insert("clonefrom", jsin.value("clonefrom").toString());
  }
  return retObject;

}

// Destroy the given boot environment and unmount it immediately withour confirmation

QJsonObject BEADM::destroyBE(QJsonObject jsin) {
	QJsonObject retObject;

	QStringList keys = jsin.keys();
	if (! keys.contains("target") ) {
		retObject.insert("error", "Missing required key(s) 'target'");
		return retObject;
}

  // Get the key values

  QString target = jsin.value("target").toString();


  QStringList output = General::RunCommand("beadm destroy -F "+ target).split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("ERROR") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    }
  }

  retObject.insert("target", target);
  return retObject;
}