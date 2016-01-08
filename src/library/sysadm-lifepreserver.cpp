//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-lifepreserver.h"
#include "sysadm-global.h"

#include "QJsonValue"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// Build list of scheduled cron snapshot jobs
QJsonObject LifePreserver::listCron() {
   QJsonObject retObject;

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

      // Breakdown this cron job
      snapitems.clear();
      snapitems << output.at(i).section("-", 0, 0).simplified();
      snapitems << output.at(i).section("-", 1, 1).simplified();
      snapitems << output.at(i).section("-", 2, 2).simplified().replace("total: ", "");

      QJsonObject values;
      values.insert("schedule", snapitems.at(1));
      values.insert("keep", snapitems.at(2));
      retObject.insert(snapitems.at(0), values);
   }

   return retObject;
}

// Return a list of snapshots on a particular pool / dataset
QJsonObject LifePreserver::listSnap(QJsonObject jsin) {
   QJsonObject retObject;
   QString pool;

   QStringList keys = jsin.keys();
   bool ok = false;
   if(! keys.contains("pool")){
     retObject.insert("error", "Missing pool key");
     return retObject;
   }

   // Check which pool we are looking at
   pool = jsin.value("pool").toString();

   // Make sure we have the pool key
   if ( pool.isEmpty() ) {
     retObject.insert("error", "Missing pool key");
     return retObject;
   }

   QStringList output = General::RunCommand("lpreserver listsnap " + pool ).split("\n");
   QList<QStringList> snaps;
   QStringList snapitems;
   QRegExp sep("\\s+");

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

      // Breakdown this snapshot
      snapitems.clear();
      snapitems << output.at(i).section(sep, 0, 0).simplified();
      snapitems << output.at(i).section(sep, 1, -1).simplified();

      QJsonObject values;
      values.insert("comment", snapitems.at(1));
      retObject.insert(snapitems.at(0), values);
  }

  return retObject;
}
