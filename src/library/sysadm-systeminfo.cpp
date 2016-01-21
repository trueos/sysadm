//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-systeminfo.h"
#include "sysadm-global.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h


//Battery Availability
QJsonObject SysInfo::batteryInfo(){
  QJsonObject retObject;
  bool ok;

  int val = General::RunCommand("apm -l").toInt(&ok);
  if ( ok && (val >= 0 && val <= 100) ) {
    retObject.insert("battery", "true");
  } else {
    retObject.insert("battery", "false");
    return retObject;
  }

  // We have a battery, return info about it
  //Battery Charge Level
  QString tmp;
  tmp.setNum(val);
  retObject.insert("level", tmp);

  //Battery Charging State
  int state = General::RunCommand("apm -a").toInt(&ok);
  if ( ok && state == 0 )
    retObject.insert("status", "offline");
  else if ( ok && state == 1 )
    retObject.insert("status", "charging");
  else if ( ok && state == 2 )
    retObject.insert("status", "backup");
  else
    retObject.insert("status", "unknown");

  return retObject;
}

// ==== ExternalDevicePaths() ====
QJsonObject SysInfo::externalDevicePaths() {
  QJsonObject retObject;

  //Returns: QStringList[<type>::::<filesystem>::::<path>]
  //Note: <type> = [USB, HDRIVE, DVD, SDCARD, UNKNOWN]
  QStringList devs = General::RunCommand("mount").split("\n");

  //Now check the output
  for(int i=0; i<devs.length(); i++){
    if(devs[i].startsWith("/dev/")){
      devs[i].replace("\t"," ");
      QString type = devs[i].section(" on ",0,0);
	type.remove("/dev/");
      //Determine the type of hardware device based on the dev node
      if(type.startsWith("da")){ type = "USB"; }
      else if(type.startsWith("ada")){ type = "HDRIVE"; }
      else if(type.startsWith("mmsd")){ type = "SDCARD"; }
      else if(type.startsWith("cd")||type.startsWith("acd")){ type="DVD"; }
      else{ type = "UNKNOWN"; }
      //Now put the device in the proper output format
      QString filesystem = devs[i].section("(",1,1).section(",",0,0);
      QString path = devs[i].section(" on ",1,50).section("(",0,0).simplified();
      QJsonObject vals;
      vals.insert("type",type);
      vals.insert("filesystem",filesystem);
      vals.insert("path",path);
      retObject.insert(devs[i].section(" ", 0, 0), vals);
    }else{
      //invalid device - remove it from the list
      devs.removeAt(i);
      i--;
    }
  }

  // Return the devices / mounts
  return retObject;
}
