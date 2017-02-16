//===========================================
//  PC-BSD source code
//  Copyright (c) 2015-2017, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-iocage.h"
#include "sysadm-global.h"
//need access to the global DISPATCHER object
#include "globals.h" 

using namespace sysadm;

// ============ GLOBAL OPTIONS ==============
//  Current activation status
QJsonObject Iocage::activateStatus(){
  QJsonObject retObject;
  bool success = false;
  QString output = General::RunCommand(success, "iocage activate --status");
  retObject.insert("activated", success ? "true" : "false");
  if(success){
    //Grab the currently activated pool out of the return, and list that
    QString pool = output.simplified();
      retObject.insert("pool", pool);
  }
  return retObject;
}

// Activate a zpool for iocage on the box
QJsonObject Iocage::activatePool(QJsonObject jsin) {
  QJsonObject retObject;
  QStringList keys = jsin.keys();

  // Get the key values
  QString pool = jsin.value("pool").toString();
  bool success = false;
  QStringList output = General::RunCommand(success, "iocage activate " + pool).split("\n");
  if(success){
      retObject.insert("success", "pool " + pool + " activated.");
  } else {
      retObject.insert("error", output.join("\n"));
  }
  return retObject;
}


// Deactivate a zpool for iocage on the box
QJsonObject Iocage::deactivatePool(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("pool") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString pool = jsin.value("pool").toString();
  bool success = false;
  QStringList output = General::RunCommand(success, "iocage deactivate " + pool).split("\n");
  QJsonObject vals;

  if (success){
    retObject.insert("success", "pool " + pool + " deactivated.");
  }else{
    retObject.insert("error", output.join("\n"));
  }
  return retObject;
}

// Clean everything iocage related on a box
QJsonObject Iocage::cleanAll() {
  QJsonObject retObject;
  bool success = false;
  QStringList output = General::RunCommand(success, "iocage clean -fa ").split("\n");

  if(!success) {
    retObject.insert("error", output.join("\n"));
  } else {
    retObject.insert("success", "All iocage datasets have been cleaned.");
  }

  return retObject;
}

//================TEMPLATE MANAGEMENT===================
QJsonObject Iocage::listTemplates(){
  QJsonObject retObject;
  QStringList local = General::RunCommand("iocage list -tlh ").split("\n");
  for(int i=0; i<local.length(); i++){
    QStringList info = local[i].split("\t"); //the -h flag is for scripting use (tabs as separators)
    //NOTE ABOUT FORMAT:
    // [JID, UUID, BOOT, STATE, TAG, TYPE, IP4, RELEASE, TEMPLATE]
    if(info.length()!=9){ continue; } //invalid line
    QJsonObject obj;
    obj.insert("jid",info[0]);
    obj.insert("uuid",info[1]);
    obj.insert("boot",info[2]);
    obj.insert("state",info[3]);
    obj.insert("tag",info[4]);
    obj.insert("type",info[5]);
    obj.insert("ip4",info[6]);
    obj.insert("release",info[7]);
    obj.insert("template",info[8]);
    retObject.insert(info[8], obj);
  }
  
  return retObject;
}

QJsonObject Iocage::listReleases(){
  QJsonObject retObject;
  // Locally-available releases
  QStringList local = General::RunCommand("iocage list -rh").split("\n");
  retObject.insert("local", QJsonArray::fromStringList(local) );
  //Remote releases available for download
  QStringList remote = General::RunCommand("iocage list -Rh").split("\n");
  for(int i=0; i<remote.length(); i++){
    if(remote[i].startsWith("[")){ remote[i] = remote[i].section("]",1,-1); }
    else{  remote.removeAt(i); i--; }
  }
  retObject.insert("remote", QJsonArray::fromStringList(remote));
  return retObject;
}

QJsonObject Iocage::listPlugins(){
  QJsonObject retObject;
  //Not sure about format of this yet (just commited upstream) - just treat it as line-delimited for now. (2/16/17)
  //locally downloaded plugins
  QStringList local = General::RunCommand("iocage list -ph ").split("\n");
  retObject.insert("local", QJsonArray::fromStringList(local) );
  //Remote plugins available for download/use
  QStringList remote = General::RunCommand("iocage list -Ph").split("\n");
  retObject.insert("remote", QJsonArray::fromStringList(remote));
  return retObject;
}

QJsonObject Iocage::fetchReleases(QJsonObject inobj){
  QJsonObject retObject;
  if(!inobj.contains("releases")){ return retObject; } //nothing to do
  QStringList releases; 
  if(inobj.value("releases").isArray()){ releases = General::JsonArrayToStringList(inobj.value("releases").toArray()); }
  else if(inobj.value("releases").isString()){ releases << inobj.value("releases").toString(); }
  //Now start up each of these downloads as appropriate
  QStringList cids = DISPATCHER->listJobs().value("no_queue").toObject().keys(); //all currently running/pending jobs
  QString jobprefix = "sysadm_iocage_fetch_release_";
  QJsonArray started;
  for(int i=0; i<releases.length(); i++){
    releases[i] = releases[i].section(" ",0,0, QString::SectionSkipEmpty); //all valid releases are a single word - do not allow injection of other commands
    if(cids.contains(jobprefix+releases[i]) ){ continue; } //this fetch job is already running - skip it for now
    DISPATCHER->queueProcess(jobprefix+releases[i], "iocage fetch --verify -r "+releases[i]);
    started << jobprefix+releases[i];
  }
  if(started.count()>0){ retObject.insert("started_dispatcher_id", started); }
  return retObject;
}

QJsonObject Iocage::fetchPlugins(QJsonObject inobj){
  QJsonObject retObject;
  if(!inobj.contains("plugins")){ return retObject; } //nothing to do
  QStringList plugins; 
  if(inobj.value("plugins").isArray()){ plugins = General::JsonArrayToStringList(inobj.value("plugins").toArray()); }
  else if(inobj.value("plugins").isString()){ plugins << inobj.value("plugins").toString(); }
  //Now start up each of these downloads as appropriate
  QStringList cids = DISPATCHER->listJobs().value("no_queue").toObject().keys(); //all currently running/pending jobs
  QString jobprefix = "sysadm_iocage_fetch_plugin_";
  QJsonArray started;
  for(int i=0; i<plugins.length(); i++){
    plugins[i] = plugins[i].section(" ",0,0, QString::SectionSkipEmpty); //all valid releases are a single word - do not allow injection of other commands
    if(cids.contains(jobprefix+plugins[i]) ){ continue; } //this fetch job is already running - skip it for now
    DISPATCHER->queueProcess(jobprefix+plugins[i], "iocage fetch --verify -P "+plugins[i]);
    started << jobprefix+plugins[i];
  }
  if(started.count()>0){ retObject.insert("started_dispatcher_id", started); }
  return retObject;
}
// Clean all templates on a box
QJsonObject Iocage::cleanTemplates() {
  QJsonObject retObject;
  bool success = false;
  QString output = General::RunCommand(success, "iocage clean -ft ");
  if(!success) {
    retObject.insert("error", output);
  } else {
    retObject.insert("success", "All templates have been cleaned.");
  }
  return retObject;
}

// Clean all RELEASEs on a box
QJsonObject Iocage::cleanReleases() {
  QJsonObject retObject;
  bool success = false;
  QString output = General::RunCommand(success, "iocage clean -fr ");
  if(!success) {
    retObject.insert("error", output);
  } else {
    retObject.insert("success", "All RELEASEs have been cleaned.");
  }
  return retObject;
}

// =================JAIL MANAGEMENT====================


// Execute a process in a jail on the box
QJsonObject Iocage::execJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail")
     || ! keys.contains("user")
     || ! keys.contains("command") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QString user = jsin.value("user").toString();
  QString command = jsin.value("command").toString();

  QStringList output;

  output = General::RunCommand("iocage exec -U " + user + " " + jail + " " + command).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("execvp:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

      vals.insert(key, value);
    }
  }

  retObject.insert("success", vals);

  return retObject;
}

// Show resource usage for jails on the box
QJsonObject Iocage::df() {
  QJsonObject retObject;

  // Get the key values
  QStringList output = General::RunCommand("iocage df").split("\n");
  QJsonObject vals;

  for ( int i = 0; i < output.size(); i++)
  {
    // Null output at first
    if ( output.at(i).isEmpty() )
      continue;

    QJsonObject jail;
    QString line = output.at(i).simplified();
    QString uuid = line.section(" ", 0, 0);

    // Otherwise we get a list of what we already know.
    if ( line.section(" ", 0, 0) == "UUID" )
      continue;

    jail.insert("crt", line.section(" ", 1, 1));
    jail.insert("res", line.section(" ", 2, 2));
    jail.insert("qta", line.section(" ", 3, 3));
    jail.insert("use", line.section(" ", 4, 4));
    jail.insert("ava", line.section(" ", 5, 5));
    jail.insert("tag", line.section(" ", 6, 6));

    retObject.insert(uuid, jail);
  }

  return retObject;
}

// Destroy a jail on the box
QJsonObject Iocage::destroyJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QStringList output;

  output = General::RunCommand("iocage destroy -f " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

      vals.insert(key, value);
    }
  }

  retObject.insert("success", vals);

  return retObject;
}

// Create a jail on the box
QJsonObject Iocage::createJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();

  // Get the key values
  QString switches = jsin.value("switches").toString();
  QString props = jsin.value("props").toString();
  QStringList output;

  if ( keys.contains("switches" ) ) {
    output = General::RunCommand("iocage create " + switches + " " + props).split("\n");
  } else {
    output = General::RunCommand("iocage create " + props).split("\n");
  }

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

      if ( keys.contains("switches" ) ) {
        vals.insert("uuid", key);
      } else {
        vals.insert(key, value);
      }
    }
  }

  retObject.insert("switches", switches);
  retObject.insert("props", props);
  retObject.insert("success", vals);

  return retObject;
}

// Clone a jail on the box
QJsonObject Iocage::cloneJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QString props = jsin.value("props").toString();

  QStringList output = General::RunCommand("iocage clone " + jail + " " + props).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
    }
  }

  retObject.insert("jail", jail);
  retObject.insert("props", props);
  retObject.insert("success", vals);

  return retObject;
}



// Clean all jails on a box
QJsonObject Iocage::cleanJails() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage clean -fj ").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    // This means either a mount is stuck or a jail cannot be stopped,
    // give them the error.
    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      break;
    } else {
      // iocage does a spinner for these that is distracting to see returned,
      // returning what a user wants to actually see.
      retObject.insert("success", "All jails have been cleaned.");
    }
  }

  return retObject;
}

// Resource cap a jail on the box
QJsonObject Iocage::capJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QStringList output = General::RunCommand("iocage cap " + jail).split("\n");
  QJsonObject vals;

  for ( int i = 0; i < output.size(); i++)
  {
    if ( ! output.at(i).isEmpty())
      break;

    // When a cap is successful, iocage doesn't return anything, so we have to
    // fudge the output a bit.
    retObject.insert("success", "jail " + jail + " capped.");
  }

  return retObject;
}

// Stop a jail on the box
QJsonObject Iocage::stopJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();

  QStringList output = General::RunCommand("iocage stop " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert(jail, vals);
  return retObject;
}

// Start a jail on the box
QJsonObject Iocage::startJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();

  QStringList output = General::RunCommand("iocage start " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert(jail, vals);
  return retObject;
}

// Return all the default iocage settings
/*QJsonObject Iocage::getDefaultSettings() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage defaults").split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section("=", 0, 0);
    QString value = output.at(i).simplified().section("=", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert("defaults", vals);
  return retObject;
}*/

// Return all of the jail settings
QJsonObject Iocage::getJailSettings(QJsonObject jsin) {
  QJsonObject retObject;
  QStringList output;

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QString prop = jsin.value("prop").toString();

  QString switches = jsin.value("switches").toString();

  QStringList keys = jsin.keys();
  if (! keys.contains("jail")
     && keys.contains("prop")
     && keys.contains("switches") ) {
    output = General::RunCommand("iocage get " + switches + " " + prop).split("\n");
  } else if ( ! keys.contains("jail")
     && ! keys.contains("prop")
     && ! keys.contains("switches") ){
      retObject.insert("error", "Missing required keys");
      return retObject;
  }

  if ( ! keys.contains("prop")
     && ! keys.contains("switches") ) {
    output = General::RunCommand("iocage get all " + jail).split("\n");
  } else if ( keys.contains("prop")
       && ! keys.contains("switches") ) {
      output = General::RunCommand("iocage get " + prop + " " + jail).split("\n");
  }

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    if ( keys.contains("switches" ) ) {
      QString line = output.at(i).simplified();

      // Otherwise we get a list of what we already know.
      if ( line.section(" ", 0, 0) == "UUID" )
        continue;

      QJsonObject jail;
      QString uuid = line.section(" ", 0, 0);

      jail.insert("TAG", line.section(" ", 1, 1));
      jail.insert(prop, line.section(" ", 2, 2));
      retObject.insert(uuid, jail);
      continue;
    }

    if ( keys.contains("prop" )
       && ! keys.contains("switches")
       && prop != "all") {
      vals.insert(prop, key);
      retObject.insert(jail, vals);
      continue;
    }

    vals.insert(key, value);
    retObject.insert(jail, vals);
    }
  }

  return retObject;
}

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
    jail.insert("ip4", line.section(" ", 6, 6));

    retObject.insert(uuid, jail);
  }

  return retObject;
}
