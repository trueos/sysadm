//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-iocage.h"
#include "sysadm-global.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// List the jails on the box
QJsonObject Iocage::listJails() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage list").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QJsonObject jail;
    QString line = output.at(i).simplified();
    QString uuid = line.section(" ", 1, 1);

    jail.insert("jid", line.section(" ", 0, 0));
    jail.insert("boot", line.section(" ", 2, 2));
    jail.insert("state", line.section(" ", 3, 3));
    jail.insert("tag", line.section(" ", 4, 4));
    jail.insert("type", line.section(" ", 5, 5));

    retObject.insert(uuid, jail);
  }

  return retObject;
}
