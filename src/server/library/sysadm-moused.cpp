//===========================================
//  TrueOS source code
//  Copyright (c) 2017, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-moused.h"
#include "sysadm-global.h"
#include "globals.h"

#define _MOUSED_CONF QString("/etc/conf.d/moused")
#define _MOUSED_SYS_CONF QString("/etc/rc.conf")
#define _MOUSED_DEFAULT_CONF QString("/etc/defaults/rc.conf")

using namespace sysadm;

QJsonObject moused::listDevices(){
  QJsonObject out;
  QDir dir("/var/run");
  QStringList devs = dir.entryList(QStringList() << "moused-*.pid", QDir::Files, QDir::Name);
  //qDebug() << "Found moused devices:" << devs;
  for(int i=0; i<devs.length(); i++){
    QString dev = devs[i].section("-",1,-1).section(".pid",0,0);
    QString sysctlprefix = "dev."+dev.left(dev.length()-1)+"."+dev.right(1)+".";
    //qDebug() << " - Device:" << dev << "Sysctl Prefix:" << sysctlprefix;
    QJsonObject devObj;
      devObj.insert("device", dev);
      devObj.insert("description", General::sysctl(sysctlprefix+"%desc"));
      devObj.insert("parent", General::sysctl(sysctlprefix+"%parent"));
      devObj.insert("driver", General::sysctl(sysctlprefix+"%driver"));
    out.insert(dev, devObj);
  }
  //qDebug() << "Device List:" << out;
  return out;
}

//Service options for individual devices
QJsonObject moused::listOptions(){
  QJsonObject out;
  out.insert("emulate_button_3", QJsonArray() << "true" << "false");
  out.insert("hand_mode", QJsonArray() << "left" << "right");
  out.insert("virtual_scrolling", QJsonArray() << "true" << "false");
  out.insert("accel_exponential", "float min=1.0 max=2.0");
  out.insert("accel_linear", "float min=0.01 max=100.00");
  out.insert("resolution", QJsonArray() << "low" << "medium-low" << "medium-high" << "high" );
  out.insert("terminate_drift_threshold_pixels", "int min=0 max=1000");
  return out;
}
 
QJsonObject moused::readOptions(QJsonObject obj){
  QString device = obj.value("device").toString();
  //qDebug() << "Read Options for Device:" << device;
  if(device.isEmpty()){ return QJsonObject(); } //invalid inputs
  
  QStringList args = General::getConfFileValue(_MOUSED_CONF, "moused_args_"+device+"=" ).section("\"",1,-2).split(" ");
  if(args.isEmpty()){ General::getConfFileValue(_MOUSED_SYS_CONF, "moused_flags=" ).section("\"",1,-2).split(" "); }
  if(args.isEmpty()){ General::getConfFileValue(_MOUSED_DEFAULT_CONF, "moused_flags=" ).section("\"",1,-2).split(" "); }

  //qDebug() << " - Arguments:" << args;
  QJsonObject out;
  //Now parse the arguments and list them in the output object
  out.insert("emulate_button_3", args.contains("-3") ? "true" : "false");
  int index = args.indexOf("-m");
  bool righthand = true;
  while(index>=0 && args.length() > (index+1) ){
    if(args[index+1].startsWith("1=")){ righthand = (args[index+1] == "1=1"); }
    index = args.indexOf("-m", index+1);
  }
  out.insert("hand_mode", righthand ? "right" : "left");
  out.insert("virtual_scrolling", args.contains("-V") ? "true" : "false" );

  index = args.indexOf("-A");
  QString val = "1.0";
  if(index>=0 && args.length()>(index+1)){ val = args[index+1].section(",",0,0); }
  out.insert("accel_exponential", val);

  index = args.indexOf("-a");
  val = "1.0";
  if(index>=0 && args.length()>(index+1)){ val = args[index+1].section(",",0,0); }
  out.insert("accel_linear", val);

  index = args.indexOf("-r");
  val = "medium-low"; //Not sure about the actual default for moused resolution (not in the docs) - Ken Moore 1/18/17
  if(index>=0 && args.length()>(index+1)){ val = args[index+1]; }
  out.insert("resolution", val);

  index = args.indexOf("-T");
  val = "0"; //disabled by default
  if(index>=0 && args.length()>(index+1)){ val = args[index+1].section(",",0,0); }
  out.insert("terminate_drift_threshold_pixels", val);

  //Now make sure to re-tag the output object with the device ID
  out.insert("device", device);
  return out;
}

QJsonObject moused::setOptions(QJsonObject obj){
  QJsonObject Cobj = readOptions(obj); //will only use the "device" field of the input object
  if(Cobj.keys().isEmpty()){ return Cobj; } //invalid inputs
  //Overlay the user's settings on top of the current settings
  QStringList keys = Cobj.keys();
    keys.removeAll("device");
  bool foundchange = false;
  for(int i=0; i<keys.length(); i++){
    if(obj.contains(keys[i])){ foundchange = true; Cobj.insert(keys[i], obj.value(keys[i])); } //This will replace any current setting with the user-requested one
  }
  if(!foundchange){ return QJsonObject(); } //invalid inputs - nothing to do

  //Assemble the argument list based on the options
  QStringList args;
  for(int i=0; i<keys.length(); i++){
    QString val = Cobj.value(keys[i]).toString();
    if(keys[i]=="emulate_button_3" && val=="true"){ args << "-3"; }
    else if(keys[i]=="hand_mode" && val=="left"){ args << "-m" << "1=3" << "-m" << "3=1"; }
    else if(keys[i]=="virtual_scrolling" && val=="true"){ args << "-V" << "-H"; } //Enable both horizontal and vertical virtual scrolling
    else if(keys[i]=="accel_exponential" && val!="1.0"){ args << "-A" << val; }
    else if(keys[i]=="accel_linear" && val!="1.0"){ args << "-a" << val; } //both X and Y linear acceleration
    else if(keys[i]=="resolution"){ args << "-r " << val; }
    else if(keys[i]=="terminate_drift_threshold_pixels" && val!="0"){ args << "-T" << val; }
  }
  //Save the arguments into the settings file
  QString device = obj.value("device").toString();
  General::setConfFileValue(_MOUSED_CONF, "moused_args_"+device+"=", "moused_args_"+device+"=\""+args.join(" ")+"\"" );
  //Restart the moused daemon for this device
  General::RunQuickCommand("service moused."+device+" restart");
  return Cobj; //return the object for the settings we just used
}

//sysctl changes for classes of devices
/*QJsonObject moused::listSysctlDevices(){

}

QJsonObject moused::listSysctls(QJsonObject){

}

QJsonObject moused::readSysctls(QJsonObject){

}

QJsonObject moused::setSysctls(QJsonObject){

}*/
