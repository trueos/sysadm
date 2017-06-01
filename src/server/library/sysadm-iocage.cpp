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
  QStringList info = General::RunCommand(success, "iocage get -p").split("\n");
  retObject.insert("activated", success ? "true" : "false");
  if(success){
    //Grab the currently activated pool out of the return, and list that
    QString pool = info.last().simplified();
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
  bool ok = false;
  QStringList local = General::RunCommand(ok, "iocage list -tlh ").split("\n");
  if(ok){
    QJsonObject temp;
    for(int i=0; i<local.length(); i++){
      QStringList info = local[i].split("\t"); //the -h flag is for scripting use (tabs as separators)
      //NOTE ABOUT FORMAT:
      // [JID, UUID, BOOT, STATE, TAG, TYPE, RELEASE, IP4, IP6, TEMPLATE]
      if(info.length()!=10){ continue; } //invalid line
      QJsonObject obj;
      obj.insert("jid",info[0]);
      obj.insert("uuid",info[1]);
      obj.insert("boot",info[2]);
      obj.insert("state",info[3]);
      obj.insert("tag",info[4]);
      obj.insert("type",info[5]);
      obj.insert("release",info[6]);
      obj.insert("ip4",info[7]);
      obj.insert("ip6",info[8]);
      obj.insert("template",info[9]);
      temp.insert(info[9], obj);
    }
    retObject.insert("templates", temp);
  }else{
    retObject.insert("error",local.join("\n"));
  }
  return retObject;
}

QJsonObject Iocage::listReleases(){
  QJsonObject retObject;
  // Locally-available releases
  bool ok = false;
  QStringList local = General::RunCommand(ok, "iocage list -rh").split("\n");
  if(ok){ retObject.insert("local", QJsonArray::fromStringList(local) ); }
  //Remote releases available for download
  QStringList remote = General::RunCommand(ok, "iocage list -rRh").split("\n");
  if(ok){
    for(int i=0; i<remote.length(); i++){
      if(remote[i].startsWith("[")){ remote[i] = remote[i].section("]",1,-1).simplified(); }
      else{ remote.removeAt(i); i--; }
    }
    retObject.insert("remote", QJsonArray::fromStringList(remote));
  }
  return retObject;
}

QJsonObject Iocage::listPlugins(){
  QJsonObject retObject;
  //Remote plugins available for download/use
  bool ok = false;
  QStringList remote = General::RunCommand(ok,"iocage list -PhR").split("\n");
  QStringList local = General::RunCommand("iocage list -Ph").split("\n");
  if(!ok && remote.first().startsWith("Traceback")){ 
    //older version of iocage - no local browsing (remote uses the local syntax)
    remote = local;
    local.clear();
  }
  QJsonObject plugins;
  for(int i=0; i<remote.length(); i++){
    if(remote[i].startsWith("[")){ remote[i] = remote[i].section("]",1,-1); }
    else{  remote.removeAt(i); i--; continue; }
    //Now parse the line and put it into the plugins object
    QJsonObject obj;
    obj.insert("name", remote[i].section(" - ",0,0).simplified());
    obj.insert("description", remote[i].section(" - ",1,-1).section("(",0,-2).simplified());
    obj.insert("id", remote[i].section("(",-1).section(")",0,0).simplified());
    plugins.insert(obj.value("id").toString(), obj);
  }
  retObject.insert("remote", plugins);
  //Now do the local plugins
  plugins = QJsonObject(); //clear it
  for(int i=0; i<local.length(); i++){
    QStringList info = local[i].split("\t"); //the -h flag is for scripting use (tabs as separators)
    //NOTE ABOUT FORMAT:
    // [JID, UUID, BOOT, STATE, TAG, TYPE, RELEASE, IP4, IP6, TEMPLATE]
    if(info.length()!=10){ continue; } //invalid line
    QJsonObject obj;
    obj.insert("jid",info[0]);
    obj.insert("uuid",info[1]);
    obj.insert("boot",info[2]);
    obj.insert("state",info[3]);
    obj.insert("tag",info[4]); //name of the plugin used (non-unique)
    obj.insert("type",info[5]);
    obj.insert("release",info[6]);
    obj.insert("ip4",info[7]);
    obj.insert("ip6",info[8]);
    obj.insert("template",info[9]);
    plugins.insert(info[4]+"_"+info[0], obj);
  }
  retObject.insert("local",plugins);
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
    DISPATCHER->queueProcess(jobprefix+plugins[i], "iocage fetch --plugins --verify "+plugins[i]);
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
// List the jails on the box
QJsonObject Iocage::listJails() {
  QJsonObject retObject;
  QStringList output = General::RunCommand("iocage list -lh").split("\n");
  for(int i=0; i<output.length(); i++){
    QStringList info = output[i].split("\t");
    //FORMAT NOTE: (long output: "-l" flag)
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
    retObject.insert(info[1], obj); //use uuid as main id tag
  }
  return retObject;
}

// Show resource usage for jails on the box
QJsonObject Iocage::df() {
  QJsonObject retObject;
  bool success = false;
  QStringList output = General::RunCommand(success, "iocage df -lh").split("\n");
  for(int i=0; i<output.length(); i++){
    QStringList info = output[i].split("\t");
    //FORMAT NOTE: (long output: "-l" flag)
    // [UUID, CRT, RES, QTA, USE, AVA, TAG]
    if(info.length()!=7){ continue; } //invalid line
    QJsonObject obj;
    obj.insert("uuid",info[0]);
    obj.insert("crt",info[1]);
    obj.insert("res",info[2]);
    obj.insert("qta",info[3]);
    obj.insert("use",info[4]);
    obj.insert("ava",info[5]);
    obj.insert("tag",info[6]);
    retObject.insert(info[0], obj);
  }
  return retObject;
}

// Create a jail on the box
QJsonObject Iocage::createJail(QJsonObject jsin) {
  QJsonObject retObject;
  //Validate input arguments
  QString release = jsin.value("release").toString();
  QString templ = jsin.value("template").toString();
  if(release.isEmpty() && templ.isEmpty()){ return retObject; } //invalid inputs, need one or the other
  QStringList pkgs;
  if(jsin.value("packages").isArray()){ pkgs = General::JsonArrayToStringList(jsin.value("packages").toArray()); }
  else if(jsin.value("packages").isString()){ pkgs << jsin.value("packages").toString(); }
  QString props;
  if(jsin.contains("properties")){ props = jsin.value("properties").toString(); }
  //Assemble the arguments
  QStringList args;
  args << "iocage" << "create";
  if(!templ.isEmpty()){ args << "-t" << templ; }
  else if(!release.isEmpty()){ args << "-r" << release; }
  if(!pkgs.isEmpty()){ args << "-p" << pkgs.join(","); }
  if(!props.isEmpty()){ args << props; } //other properties
  //Start the jail creation
  QString newid = "sysadm_iocage_create_jail_"+QUuid::createUuid().toString();
  DISPATCHER->queueProcess(Dispatcher::IOCAGE_QUEUE, newid, args.join(" "));
  retObject.insert("new_dispatcher_id", newid);
  return retObject;
}

// Destroy a jail on the box
QJsonObject Iocage::destroyJail(QJsonObject jsin) {
  QJsonObject retObject;
  if( !jsin.contains("jail_uuid") ){ return retObject;  }
  QString jail = jsin.value("jail_uuid").toString();
  bool success = false;
  QString output = General::RunCommand(success, "iocage destroy -f " + jail);
  if(success){
    retObject.insert("success",jail+"destroyed");
  }else{
    retObject.insert("error",output);
  }
  return retObject;
}

// Clean all jails on a box
QJsonObject Iocage::cleanJails() {
  QJsonObject retObject;
  bool success = false;
  QString output = General::RunCommand(success, "iocage clean -fj ");
  if(success){
    retObject.insert("success", "jails cleaned");
  }else{
    retObject.insert("error", output);
  }
  return retObject;
}

// Stop a jail on the box
QJsonObject Iocage::stopJail(QJsonObject jsin) {
  QJsonObject retObject;
  if(!jsin.contains("jail_uuid")){ return retObject; }
  // Get the key values
  QString jail = jsin.value("jail_uuid").toString();
  bool success = false;
  QString output = General::RunCommand(success, "iocage stop " + jail);
  if(success){ retObject.insert("success","jail "+jail+" stopped"); }
  else{ retObject.insert("error",output); }
  return retObject;
}

// Start a jail on the box
QJsonObject Iocage::startJail(QJsonObject jsin) {
  QJsonObject retObject;
  if(!jsin.contains("jail_uuid")){ return retObject; }
  // Get the key values
  QString jail = jsin.value("jail_uuid").toString();
  bool success = false;
  QString output = General::RunCommand(success, "iocage start " + jail);
  if(success){ retObject.insert("success","jail "+jail+" started"); }
  else{ retObject.insert("error",output); }
  return retObject;
}

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
