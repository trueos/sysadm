//===========================================
//  TrueOS source code
//  Copyright (c) 2016, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-users.h"
#include "sysadm-general.h"
#include "sysadm-global.h"
#include "globals.h"

using namespace sysadm;

bool UserManager::listUsers(QJsonObject *out, bool showall, QString user){
  QStringList args; args << "usershow";
  if(showall){ args << "-a"; }
  else{ args << user; }
  bool ok = false;
  QStringList users = sysadm::General::RunCommand(ok, "pw", args, "",QStringList() << "MM_CHARSET=UTF-8").split("\n");
    if(ok){
      //Get a list of all current PersonaCrypt users (if any)
      bool PCinstalled = QFile::exists("/var/db/personacrypt");
      //Go ahead and parse/list all the users
      for(int i=0; i<users.length(); i++){
        QStringList info = users[i].split(":");
        if(info.length() == 10){
          QJsonObject uinfo;
          uinfo.insert("name", info[0]);
          //uinfo.insert("name", info[1]); //Skip Password field (just a "*" in this viewer anyway)
          uinfo.insert("uid", info[2]);
          uinfo.insert("gid", info[3]);
          uinfo.insert("class", info[4]);
          uinfo.insert("change", info[5]);
          uinfo.insert("expire", info[6]);
          uinfo.insert("comment", info[7]);
          uinfo.insert("home_dir", info[8]);
          uinfo.insert("shell", info[9]);
	  if(PCinstalled && QFile::exists("/var/db/personacrypt/" + info[0] + ".key") ){ uinfo.insert("personacrypt_enabled","true"); }
          out->insert(info[0], uinfo); //use the username as the unique object name
        }else if(info.length() == 7){
          QJsonObject uinfo;
          uinfo.insert("name", info[0]);
          //uinfo.insert("name", info[1]); //Skip Password field (just a "*" in this viewer anyway)
          uinfo.insert("uid", info[2]);
          uinfo.insert("gid", info[3]);
          uinfo.insert("comment", info[4]);
          uinfo.insert("home_dir", info[5]);
          uinfo.insert("shell", info[6]);
	  if(PCinstalled && QFile::exists("/var/db/personacrypt/" + info[0] + ".key") ){ uinfo.insert("personacrypt_enabled","true"); }
          out->insert(info[0], uinfo); //use the username as the unique object name
        }
      }
    }else{
      //Bad result from "pw" - inputs were just fine (just return nothing)
      ok = true;
    }
  if(!ok){ out->insert("error",users.join("\n")); } //should never happen - "pw" is an OS built-in on FreeBSD
  return ok;
}

bool UserManager::addUser(QJsonObject* out, QJsonObject obj){
  bool ok = false;
  //REQUIRED: "name"  AND "password"
  //OPTIONAL: "uid", "comment", "home_dir", "expire", "change", "shell", "group", "other_groups", "class"
  //OPTIONAL: "personacrypt_init"=<devicename>, "personacrypt_password"
  //OPTIONAL: "personacrypt_import"=<contents of key file - base64 encoded>
  if(obj.contains("password") && obj.contains("name") ){
    QString username = obj.value("name").toString();
    QStringList args; args << "useradd";
    args << "-n" << username;
    if(obj.contains("uid")){ args << "-u" << obj.value("uid").toString(); }
    if(obj.contains("comment")){ args << "-c" << obj.value("comment").toString(); }
    if(obj.contains("home_dir")){ args << "-d" << obj.value("home_dir").toString(); }
    if(obj.contains("expire")){ args << "-e" << obj.value("expire").toString(); }
    if(obj.contains("change")){ args << "-p" << obj.value("change").toString(); }
    if(obj.contains("shell")){ args << "-s" << obj.value("shell").toString(); }
    if(obj.contains("group")){ args << "-g" << obj.value("group").toString(); }
    if(obj.contains("other_groups")){
      if(obj.value("other_groups").isString()){ args << "-G" << obj.value("other_groups").toString(); }
      else if(obj.value("other_groups").isArray()){ args << "-G" << General::JsonArrayToStringList(obj.value("other_groups").toArray()).join(","); }
    }
    if(obj.contains("class")){ args << "-L" << obj.value("class").toString(); }
    //See if PersonaCrypt should be used on this user as well
    QString PCdev;
    if(obj.contains("personacrypt_init")){
      QString dev = obj.value("personacrypt_init").toString();
      if(dev.startsWith("/dev/")){ dev = dev.remove(0,5); }
      //Verify that the given device is valid
      QStringList valid = getAvailablePersonaCryptDevices();
      for(int i=0; i<valid.length(); i++){
        if(valid[i].startsWith(dev+": ")){ PCdev = dev; } //this is a valid device
      }
      if(PCdev!=dev){ return false; } //invalid inputs - invalid device for PersonaCrypt
    }
    QTemporaryFile pwfile("/tmp/.XXXXXXXXXXXXXXX");
    bool usercreated = false;
    if(pwfile.open()){ 
      pwfile.write( obj.value("password").toString().toUtf8().data() );
      pwfile.close(); //closed but still exists - will go out of scope and get removed in a moment
      args << "-h" << "0"; //read from std input
      ok = (0== system("cat "+pwfile.fileName().toUtf8()+" | pw "+args.join(" ").toUtf8()) );
      usercreated = ok;
    }else{
      out->insert("error","Could not open temporary file for password"); //should never happen
    }

    if(usercreated && !PCdev.isEmpty()){
      // INIT PERSONACRYPT DEVICE NOW
      //User created fine - go ahead and initialize the PersonaCrypt device for this user now
      QString pass = obj.value("password").toString(); //assume the same password as the user unless specified otherwise
      if(obj.contains("personacrypt_password")){ pass = obj.value("personacrypt_password").toString(); } //separate password for device
      ok = InitializePersonaCryptDevice(username, pass, PCdev);
      if(!ok){ out->insert("error","could not initialize personacrypt device"); }
    }else if(usercreated && obj.contains("personacrypt_import")){
      // IMPORT PERSONACRYPT KEY NOW
      ok = false;
      //Need to save key to a local file temporarily
      QTemporaryFile keyfile("/tmp/.XXXXXXXXXXXXXXX");
      if(keyfile.open()){
        keyfile.write( QByteArray::fromBase64(obj.value("personacrypt_import").toString().toUtf8()).data() );
        keyfile.close();
        ok = ImportPersonaCryptKey(keyfile.fileName());
      }
      if(!ok){ out->insert("error","could not import personacrypt key"); }
    }
    if(usercreated && !ok){
      //One of the personacrypt options failed - treat this as a full failure and also remove the newly-created user
      removeUser(username);
    }
  } //end check for valid inputs
  if(ok){ out->insert("result","success"); }
  return ok;
}

bool UserManager::removeUser(QString username, bool deletehomedir){
  bool ok = false;
  QStringList args; 
  args << "userdel" << "-n" << username;
  if(deletehomedir){ args << "-r"; }
  ok = General::RunQuickCommand("pw", args);
  return ok;  
}

bool UserManager::modifyUser(QJsonObject* out, QJsonObject obj){
  bool ok = false;
  // REQUIRED: "name"
  //OPTIONAL: "newname", "uid", "comment", "home_dir", "expire", "change", "shell", "group", "other_groups", "class"
  //OPTIONAL: "personacrypt_init"=<devicename>, "personacrypt_password"
  //OPTIONAL: "personacrypt_import"=<contents of key file - base64 encoded>
  if(obj.contains("name") ){
    QString username = obj.value("name").toString();
    QStringList args; args << "usermod";
    args << "-n" << username;
    if(obj.contains("uid")){ args << "-u" << obj.value("uid").toString(); }
    if(obj.contains("comment")){ args << "-c" << obj.value("comment").toString(); }
    if(obj.contains("home_dir")){ args << "-d" << obj.value("home_dir").toString(); }
    if(obj.contains("expire")){ args << "-e" << obj.value("expire").toString(); }
    if(obj.contains("change")){ args << "-p" << obj.value("change").toString(); }
    if(obj.contains("shell")){ args << "-s" << obj.value("shell").toString(); }
    if(obj.contains("group")){ args << "-g" << obj.value("group").toString(); }
    if(obj.contains("other_groups")){
      if(obj.value("other_groups").isString()){ args << "-G" << obj.value("other_groups").toString(); }
      else if(obj.value("other_groups").isArray()){ args << "-G" << General::JsonArrayToStringList(obj.value("other_groups").toArray()).join(","); }
    }
    if(obj.contains("class")){ args << "-L" << obj.value("class").toString(); }
    if(obj.contains("newname")){ args << "-l" << obj.value("newname").toString(); }
    if(obj.contains("password")){
      //changing password too - need to handle differently
      QTemporaryFile pwfile("/tmp/.XXXXXXXXXXXXXXX");
      bool usercreated = false;
      if(pwfile.open()){ 
        pwfile.write( obj.value("password").toString().toUtf8().data() );
        pwfile.close(); //closed but still exists - will go out of scope and get removed in a moment
        args << "-h" << "0"; //read from std input
        ok = (0== system("cat "+pwfile.fileName().toUtf8()+" | pw "+args.join(" ").toUtf8()) );
      }else{
        out->insert("error","Could not open temporary file for password"); //should never happen
      }
    }else{
      //No password change - simple command
      ok = General::RunQuickCommand("pw", args);
    }
  }
  if(ok){ out->insert("result","success"); }
  else{ out->insert("error","Could not modify user"); }
  return ok;
}

// === PERSONACRYPT FUNCTIONS ===
//List all the devices currently available to be used for a PersonaCrypt User
QStringList UserManager::getAvailablePersonaCryptDevices(){
  QStringList devs = General::RunCommand("personacrypt", QStringList() << "list" << "-r").split("\n");
  for(int i=0; i<devs.length(); i++){
    //Check validity of each line really quick (no gpart errors output and such)
    if(devs[i].isEmpty() || devs[i].startsWith("gpart")){ devs.removeAt(i); i--; }
  }
  return devs;
}

//PersonaCrypt Modification functions
bool UserManager::InitializePersonaCryptDevice(QString username, QString pass, QString device){
  QTemporaryFile pfile("/tmp/.XXXXXXXXXXXXXXX");
  bool ok = false;
  if( pfile.open() ){
    pfile.write(pass.toUtf8().data());
    pfile.close();
    ok = General::RunQuickCommand("personacrypt", QStringList() << "init" << username << pfile.fileName() << device);
  }
  return ok;
}

bool UserManager::ImportPersonaCryptKey(QString keyfile){
  if(!QFile::exists(keyfile)){ return false; } //quick check first
  bool ok = General::RunQuickCommand("personacrypt", QStringList() << "import" << keyfile);    
  return ok;
}
