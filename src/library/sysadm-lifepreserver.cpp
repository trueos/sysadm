//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-lifepreserver.h"
//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

#include "sysadm-global.h"

using namespace sysadm;

// Build list of scheduled cron snapshot jobs
QList<QStringList> LifePreserver::listCron() {
   QStringList output = General::RunCommand("lpreserver listcron").split("\n");
   QList<QStringList> snaps;
   QStringList snapitems;
   
   // Parse the output
   bool inSection = false;
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("-----------------") != -1 ) {
         inSection = true;
         continue;
      }

      if (!inSection)
         continue;

      if ( output.at(i).isEmpty() || output.at(i).indexOf("-----------------") != -1 )
	  break;

      // Save this cron job
      snapitems.clear();
      snapitems << output.at(i).section("-", 0, 0).simplified();
      snapitems << output.at(i).section("-", 1, 1).simplified();
      snapitems << output.at(i).section("-", 2, 2).simplified().replace("total: ", "");

      snaps << snapitems;
   }

   return snaps;
}
