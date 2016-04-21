//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QUuid>
#include "sysadm-general.h"
#include "sysadm-lifepreserver.h"
#include "sysadm-global.h"
#include "globals.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// Add a new replication target
QJsonObject LifePreserver::addReplication(QJsonObject jsin) {
   QJsonObject retObject;
   QString host, user, port, password, ldataset, rdataset, frequency;

   QStringList keys = jsin.keys();
   if (! keys.contains("host")
     || ! keys.contains("user")
     || ! keys.contains("port")
     || ! keys.contains("password")
     || ! keys.contains("dataset")
     || ! keys.contains("remotedataset")
     || ! keys.contains("frequency") ) {
     retObject.insert("error", "Missing required keys");
     return retObject;
   }

   // Get the key values
   host = jsin.value("host").toString();
   user = jsin.value("user").toString();
   port = jsin.value("port").toString();
   password = jsin.value("password").toString();
   ldataset = jsin.value("dataset").toString();
   rdataset = jsin.value("remotedataset").toString();
   frequency = jsin.value("frequency").toString();

   // Make sure we have the dataset / snap key(s)
   if ( host.isEmpty()
     || user.isEmpty()
     || port.isEmpty()
     || password.isEmpty()
     || ldataset.isEmpty()
     || rdataset.isEmpty()
     || frequency.isEmpty() ) {
     retObject.insert("error", "Empty dataset or snap keys ");
     return retObject;
   }

   // Run the command with the SSHPASS env variable set
   QStringList output;
   output = General::RunCommand("lpreserver", QStringList() << "replicate" << "add"
	<< host
	<< user
	<< port
	<< ldataset
	<< rdataset
	<< frequency, "", QStringList() << "SSHPASS=" + password ).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("host", host);
   values.insert("user", user);
   values.insert("port", port);
   values.insert("ldataset", ldataset);
   values.insert("rdataset", rdataset);
   values.insert("frequency", frequency);

   return values;
}

// Create a snapshot
QJsonObject LifePreserver::createSnapshot(QJsonObject jsin) {
   QJsonObject retObject;
   QString dataset, snap, comment;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("snap")){
     retObject.insert("error", "Requires dataset and snap keys");
     return retObject;
   }

   // Get the dataset / snap
   dataset = jsin.value("dataset").toString();
   snap = jsin.value("snap").toString();

   // Optional snapshot comment
   if( keys.contains("comment")){
     comment = jsin.value("comment").toString();
   }

   // Make sure we have the dataset / snap key(s)
   if ( dataset.isEmpty() || snap.isEmpty() ) {
     retObject.insert("error", "Empty dataset or snap keys ");
     return retObject;
   }

   QStringList output;
   output = General::RunCommand("lpreserver", QStringList() << "snapshot" << "create" << dataset << snap << comment).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("dataset", dataset);
   values.insert("snap", snap);
   if ( ! comment.isEmpty() )
     values.insert("comment", comment);

   return values;
}


// Re-init the LP replication target
QJsonObject LifePreserver::initReplication(QJsonObject jsin) {
   QJsonObject retObject;
   QString dset, rhost;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("host")){
     retObject.insert("error", "Missing dataset or host key");
     return retObject;
   }

   // Check which pool we are looking at
   dset = jsin.value("dataset").toString();
   rhost = jsin.value("host").toString();

   // Make sure we have the pool key
   if ( dset.isEmpty() || rhost.isEmpty()) {
     retObject.insert("error", "Missing dataset or host key");
     return retObject;
   }

   // TODO - This command can take a LONG TIME. Find a way to queue / background it and return an event
   // via websockets later, or block here and return when finished if this is REST
   QStringList output = General::RunCommand("lpreserver replicate init " + dset + " " + rhost).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   QJsonObject values;
   values.insert("dataset", dset);
   values.insert("host", rhost);
   return values;
}

// Build list of scheduled cron snapshot jobs
QJsonObject LifePreserver::listCron() {
   QJsonObject retObject;

   QStringList output = General::RunCommand("lpreserver cronsnap list").split("\n");
   QList<QStringList> snaps;
   
   // Parse the output
   bool inSnapSection = false;
   bool inScrubSection = false;
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("-----------------") != -1 ) {
         inSnapSection = true;
         continue;
      }

      if (!inSnapSection)
         continue;

      if ( output.at(i).indexOf("Pools scheduled for scrub") != -1 ) {
         inScrubSection = true;
	 continue;
      }

      if ( output.at(i).isEmpty() || output.at(i).indexOf("-----------------") != -1 )
	  continue;

      if ( inSnapSection && ! inScrubSection ) {
        // Breakdown this cron job
        QString pool = output.at(i).section("-", 0, 0).simplified();
        QString freq = output.at(i).section("-", 1, 1).simplified();
        QString keep = output.at(i).section("-", 2, 2).simplified().replace("total: ", "");

        QJsonObject values;
        values.insert("schedule", freq);
        values.insert("keep", keep);
        retObject.insert(pool, values);
      } else if (inSnapSection && inScrubSection ) {
        // Add a cron scrub
        QString pool = output.at(i).section("-", 0, 0).simplified();
        QString freq = output.at(i).section("-", 1, 1).simplified().replace(" ", "");
	qDebug() << "Found scrub:" << pool << freq;

        QJsonObject values;
        QStringList keys = retObject.keys();
        if( keys.contains(pool)){
          // We have an existing pool object, add it
          values = retObject[pool].toObject();
          retObject.remove(pool);
          values.insert("scrub", freq);
          retObject.insert(pool, values);
	} else {
          // Add a new pool object with the scrub
          values.insert("scrub", freq);
          retObject.insert(pool, values);
	}
      }
   }

   return retObject;
}

// Return a list of replication targets
QJsonObject LifePreserver::listReplication() {
   QJsonObject retObject;

   QStringList output = General::RunCommand("lpreserver replicate list").split("\n");
   QStringList setitems;
   QString tmpkey;
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

      // Breakdown the settings
      QJsonObject values;
      tmpkey = "";
      QString dset, rdset, user, host, port, parseline, time;
      dset = output.at(i).section(sep, 0, 0).simplified();
      parseline = output.at(i).section(sep, 2, 2).simplified();
      user = parseline.section("@", 0, 0);
      host = parseline.section("@", 1, 1).section("[", 0, 0);
      port = parseline.section("@", 1, 1).section("[", 1, 1).section("]", 0, 0);
      rdset = parseline.section(":", 1, 1);
      time = output.at(i).section(sep, 4, 4).simplified();

      values.insert("dataset", dset);
      values.insert("user", user);
      values.insert("port", port);
      values.insert("host", host);
      values.insert("rdset", rdset);
      values.insert("frequency", time);
      retObject.insert(dset + "->" + host, values);
  }

  return retObject;
}

// Return a list of snapshots on a particular pool / dataset
QJsonObject LifePreserver::listSnap(QJsonObject jsin) {
   QJsonObject retObject;
   QString pool;

   QStringList keys = jsin.keys();
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

   QStringList output = General::RunCommand("lpreserver snapshot list " + pool ).split("\n");
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

// Remove a replication task
QJsonObject LifePreserver::removeReplication(QJsonObject jsin) {
   QJsonObject retObject;
   QString dataset, host;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("host")){
     retObject.insert("error", "Requires dataset and host keys");
     return retObject;
   }

   // Get the dataset / host
   dataset = jsin.value("dataset").toString();
   host = jsin.value("host").toString();

   // Make sure we have the dataset / host key(s)
   if ( dataset.isEmpty() || host.isEmpty() ) {
     retObject.insert("error", "Empty dataset or host keys ");
     return retObject;
   }

   QStringList output;
   output = General::RunCommand("lpreserver replicate remove " + dataset + " " + host).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("dataset", dataset);
   values.insert("host", host);

   return values;
}

// Remove a snapshot
QJsonObject LifePreserver::removeSnapshot(QJsonObject jsin) {
   QJsonObject retObject;
   QString dataset, snap;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("snap")){
     retObject.insert("error", "Requires dataset and snap keys");
     return retObject;
   }

   // Get the dataset / snap
   dataset = jsin.value("dataset").toString();
   snap = jsin.value("snap").toString();

   // Make sure we have the dataset / snap key(s)
   if ( dataset.isEmpty() || snap.isEmpty() ) {
     retObject.insert("error", "Empty dataset or snap keys ");
     return retObject;
   }

   QStringList output;
   output = General::RunCommand("lpreserver snapshot remove " + dataset + " " + snap).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("dataset", dataset);
   values.insert("snap", snap);

   return values;
}

// Run a replication task
QJsonObject LifePreserver::runReplication(QJsonObject jsin) {
   QJsonObject retObject;
   QString dataset, host;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("host")){
     retObject.insert("error", "Requires dataset and host keys");
     return retObject;
   }

   // Get the dataset / host
   dataset = jsin.value("dataset").toString();
   host = jsin.value("host").toString();

   // Make sure we have the dataset / host key(s)
   if ( dataset.isEmpty() || host.isEmpty() ) {
     retObject.insert("error", "Empty dataset or host keys ");
     return retObject;
   }

   // Create a unique ID for this queued action
   QString ID = QUuid::createUuid().toString();
   
   DISPATCHER->queueProcess(ID, "lpreserver replicate run " + dataset + " " + host);

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("queueid", ID);
   values.insert("command", "lpreserver replicate run " + dataset + " " + host);
   values.insert("comment", "Task Queued");

   return values;
}

// Revert to a snapshot
QJsonObject LifePreserver::revertSnapshot(QJsonObject jsin) {
   QJsonObject retObject;
   QString dataset, snap;

   QStringList keys = jsin.keys();
   if(! keys.contains("dataset") || ! keys.contains("snap")){
     retObject.insert("error", "Requires dataset and snap keys");
     return retObject;
   }

   // Get the dataset / snap
   dataset = jsin.value("dataset").toString();
   snap = jsin.value("snap").toString();

   // Make sure we have the dataset / snap key(s)
   if ( dataset.isEmpty() || snap.isEmpty() ) {
     retObject.insert("error", "Empty dataset or snap keys ");
     return retObject;
   }

   QStringList output;
   output = General::RunCommand("lpreserver snapshot revert " + dataset + " " + snap).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      }
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("dataset", dataset);
   values.insert("snap", snap);

   return values;
}

// Save system-wide settings
QJsonObject LifePreserver::saveSettings(QJsonObject jsin) {
   QJsonObject retObject;
   QString warn, email, emailopts, recursive;

   QStringList keys = jsin.keys();
   if(! keys.contains("duwarn") && ! keys.contains("email") && ! keys.contains("emailopts") && !keys.contains("recursive")){
     retObject.insert("error", "Requires duwarn, email, emailopts or recursive to be set!");
     return retObject;
   }

   QJsonObject values;
   QStringList output;

   // Get the settings
   if ( keys.contains("duwarn") ) {
     warn = jsin.value("duwarn").toString();
     output = General::RunCommand("lpreserver set duwarn " + warn).split("\n");
     // Check for any errors
     for ( int i = 0; i < output.size(); i++)
     {
        if ( output.at(i).indexOf("ERROR:") != -1 ) {
         retObject.insert("error", output.at(i));
         return retObject;
        }
     }
     values.insert("duwarn", warn);
   }
   if ( keys.contains("email") ) {
     email = jsin.value("email").toString();
     output = General::RunCommand("lpreserver set email " + email).split("\n");
     // Check for any errors
     for ( int i = 0; i < output.size(); i++)
     {
        if ( output.at(i).indexOf("ERROR:") != -1 ) {
         retObject.insert("error", output.at(i));
         return retObject;
        }
     }
     values.insert("email", email);
   }
   if ( keys.contains("emailopts") ) {
     emailopts = jsin.value("emailopts").toString();
     output = General::RunCommand("lpreserver set emailopts " + emailopts.toUpper()).split("\n");
     // Check for any errors
     for ( int i = 0; i < output.size(); i++)
     {
        if ( output.at(i).indexOf("ERROR:") != -1 ) {
         retObject.insert("error", output.at(i));
         return retObject;
        }
     }
     values.insert("emailopts", emailopts);
   }
   if ( keys.contains("recursive") ) {
     recursive = jsin.value("recursive").toString();
     QString recset = "ON";
     if ( recursive.toUpper() == "FALSE" )
       recset = "OFF";
     output = General::RunCommand("lpreserver set recursive " + recset).split("\n");
     // Check for any errors
     for ( int i = 0; i < output.size(); i++)
     {
        if ( output.at(i).indexOf("ERROR:") != -1 ) {
         retObject.insert("error", output.at(i));
         return retObject;
        }
     }
     values.insert("recursive", recursive);
   }

   return values;
}


// Schedule a new scrub routine
QJsonObject LifePreserver::scheduleScrub(QJsonObject jsin) {
   QJsonObject retObject;
   QString pool, frequency;

   QStringList keys = jsin.keys();
   if(! keys.contains("pool") || ! keys.contains("frequency")){
     retObject.insert("error", "Requires pool and frequency keys");
     return retObject;
   }

   // Check which pool we are looking at
   pool = jsin.value("pool").toString();
   frequency = jsin.value("frequency").toString();

   // Make sure we have the pool / frequency / keep key(s)
   if ( pool.isEmpty() || frequency.isEmpty() ) {
     retObject.insert("error", "Empty pool or frequency keys ");
     return retObject;
   }

   QStringList output;
   if ( frequency == "none" )
     output = General::RunCommand("lpreserver cronscrub " + pool + " stop " + frequency).split("\n");
   else
     output = General::RunCommand("lpreserver cronscrub " + pool + " start " + frequency).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      } 
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("pool", pool);
   values.insert("frequency", frequency);

   return values;
}


// Schedule a new snapshot routine
QJsonObject LifePreserver::scheduleSnapshot(QJsonObject jsin) {
   QJsonObject retObject;
   QString pool, frequency, keep;

   QStringList keys = jsin.keys();
   if(! keys.contains("pool") || ! keys.contains("frequency") || ! keys.contains("keep")){
     retObject.insert("error", "Requires pool, frequency and keep keys");
     return retObject;
   }

   // Check which pool we are looking at
   pool = jsin.value("pool").toString();
   frequency = jsin.value("frequency").toString();
   keep = jsin.value("keep").toString();

   // Make sure we have the pool / frequency / keep key(s)
   if ( pool.isEmpty() || frequency.isEmpty() || keep.isEmpty() ) {
     retObject.insert("error", "Empty pool, frequency and keep keys ");
     return retObject;
   }

   QStringList output;
   if ( frequency == "none" )
     output = General::RunCommand("lpreserver cronsnap stop " + pool ).split("\n");
   else
     output = General::RunCommand("lpreserver cronsnap start " + pool + " " + frequency + " " + keep ).split("\n");

   // Check for any errors
   for ( int i = 0; i < output.size(); i++)
   {
      if ( output.at(i).indexOf("ERROR:") != -1 ) {
       retObject.insert("error", output.at(i));
       return retObject;
      } 
   }

   // Got to the end, return the good json
   QJsonObject values;
   values.insert("pool", pool);
   values.insert("frequency", frequency);
   values.insert("keep", keep);

   return values;
}


// Return a list of settings for life-preserver
QJsonObject LifePreserver::settings() {
   QJsonObject retObject;

   QStringList output = General::RunCommand("lpreserver get").split("\n");
   QStringList setitems;
   QString tmpkey;
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

      // Breakdown the setting we got
      tmpkey = "";
      setitems.clear();
      setitems << output.at(i).section(":", 0, 0).simplified();
      setitems << output.at(i).section(":", 1, 1).simplified();
      if ( setitems.at(0) == "Recursive mode" )
	tmpkey = "recursive";
      if ( setitems.at(0) == "E-mail notifications" )
	tmpkey = "email";
      if ( setitems.at(0) == "E-mail addresses" )
	tmpkey = "emailaddress";
      if ( setitems.at(0) == "Disk space warn at" )
	tmpkey = "diskwarn";

      // Unknown key we dont support?
      if ( tmpkey.isEmpty() )
        continue;

      retObject.insert(tmpkey, setitems.at(1));
  }

  return retObject;
}
