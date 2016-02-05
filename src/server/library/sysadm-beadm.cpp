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
