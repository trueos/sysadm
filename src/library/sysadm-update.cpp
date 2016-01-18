//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-update.h"
#include "sysadm-global.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// Return a list of updates available
QJsonObject Update::checkUpdates() {
   QJsonObject retObject;

   QStringList output = General::RunCommand("pc-updatemanager check").split("\n");
   QString nameval;

   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("Your system is up to date!") != -1 )
      {
        retObject.insert("status", "noupdates");
	return retObject;
      }
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
        retObject.insert("patch", itemvals);
      }
      if ( output.at(i).indexOf("TYPE: PKGUPDATE") != -1 ) {
        QJsonObject itemvals;
	itemvals.insert("name", nameval);
        retObject.insert("packageupdate", itemvals);
      }
  }

  // Update status that we have updates
  retObject.insert("status", "updatesavailable");

  return retObject;
}
