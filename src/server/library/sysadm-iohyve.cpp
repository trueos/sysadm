//===========================================
//  PC-BSD source code
//  Copyright (c) 2016, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-iohyve.h"
#include "sysadm-global.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

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
