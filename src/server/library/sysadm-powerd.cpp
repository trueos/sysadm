//===========================================
//  TrueOS source code
//  Copyright (c) 2017, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-powerd.h"
#include "sysadm-global.h"
#include "globals.h"

#include "sysadm-servicemanager.h"

#define _POWERD_CONF QString("/etc/conf.d/powerd")
#define _POWERD_SYS_CONF QString("/etc/rc.conf")
#define _POWERD_DEFAULT_CONF QString("/etc/defaults/rc.conf")

using namespace sysadm;

//Service options for individual devices
QJsonObject powerd::listOptions(){
  QJsonObject out;
  out.insert("ac_power_mode", QJsonArray() << "maximum" << "hiadaptive" << "adaptive" << "minumum"); // "-a" flag
  out.insert("battery_power_mode", QJsonArray() << "maximum" << "hiadaptive" << "adaptive" << "minumum"); // "-b" flag
  out.insert("unknown_power_mode", QJsonArray() << "maximum" << "hiadaptive" << "adaptive" << "minumum"); // "-n" flag
  out.insert("adaptive_lower_threshold_percent", "int min=1 max=100"); // "-i" flag
  out.insert("adaptive_raise_threshold_percent", "int min=1 max=100"); // "-r" flag
  out.insert("min_cpu_freq", "int frequency(hz)>0"); // "-m" flag
  out.insert("max_cpu_freq", "int frequency(hz)>0"); // "-M" flag
  out.insert("polling_interval_ms", "int milliseconds>0"); // "-p" flag
  return out;
}
 
QJsonObject powerd::readOptions(){
 
  QString val = General::getConfFileValue(_POWERD_CONF, "powerd_args=" );
  if(val.isEmpty()){ General::getConfFileValue(_POWERD_SYS_CONF, "powerd_args=" ); }
  if(val.isEmpty()){ General::getConfFileValue(_POWERD_DEFAULT_CONF, "powerd_args=" ); }
  val = val.simplified();
  if(val.startsWith("\"")){ val.remove(0,1); }
  if(val.endsWith("\"")){ val.chop(1); }
  QStringList args = val.split(" ");
  //qDebug() << " - Arguments:" << args;
  QJsonObject out;
  //Now parse the arguments and list them in the output object
  int index = args.indexOf("-a");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("ac_power_mode", args[index+1] );
  }else{
    QString ac = General::getConfFileValue(_POWERD_CONF, "powerd_ac_mode=" );
    if(ac.isEmpty()){ ac = "adaptive"; }
    out.insert("ac_power_mode",ac);
  }
  index = args.indexOf("-b");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("battery_power_mode", args[index+1] );
  }else{
    QString bat = General::getConfFileValue(_POWERD_CONF, "powerd_battery_mode=" );
    if(bat.isEmpty()){ bat = "adaptive"; }
    out.insert("battery_power_mode",bat);
  }
  index = args.indexOf("-n");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("unknown_power_mode", args[index+1] );
  }else{
    out.insert("unknown_power_mode", "adaptive");
  }
  index = args.indexOf("-i");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("adaptive_lower_threshold_percent", args[index+1] );
  }else{
    out.insert("adaptive_lower_threshold_percent", "50");
  }
  index = args.indexOf("-r");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("adaptive_raise_threshold_percent", args[index+1] );
  }else{
    out.insert("adaptive_raise_threshold_percent", "75");
  }
  index = args.indexOf("-m");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("min_cpu_freq", args[index+1] );
  }else{
    out.insert("min_cpu_freq", "-1");
  }
  index = args.indexOf("-M");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("max_cpu_freq", args[index+1] );
  }else{
    out.insert("max_cpu_freq", "-1");
  }
  index = args.indexOf("-p");
  if(index>=0 && index < (args.length()-1) ){
    out.insert("polling_interval_ms", args[index+1] );
  }else{
    out.insert("polling_interval_ms", "250");
  }
  return out;
}

QJsonObject powerd::setOptions(QJsonObject obj){
  QJsonObject Cobj = readOptions();

  //Overlay the user's settings on top of the current settings
  QStringList keys = Cobj.keys();
  bool foundchange = false;
  for(int i=0; i<keys.length(); i++){
    if(obj.contains(keys[i])){ foundchange = true; Cobj.insert(keys[i], obj.value(keys[i])); } //This will replace any current setting with the user-requested one
  }
  if(!foundchange){ return QJsonObject(); } //invalid inputs - nothing to do

  //Assemble the argument list based on the options
  QStringList args;
  for(int i=0; i<keys.length(); i++){
    QString val = Cobj.value(keys[i]).toString();
    if(keys[i]=="ac_power_mode"){ General::setConfFileValue(_POWERD_CONF, "powerd_ac_mode=", "powerd_ac_mode=\""+val+"\"" ); }
    else if(keys[i]=="battery_power_mode"){ General::setConfFileValue(_POWERD_CONF, "powerd_battery_mode=", "powerd_ac_mode=\""+val+"\"" ); }
    else if(keys[i]=="unknown_power_mode"){ args << "-n" << val; }
    else if(keys[i]=="adaptive_lower_threshold_percent"){ args << "-i" << val; }
    else if(keys[i]=="adaptive_raise_threshold_percent"){ args << "-r" << val; }
    else if(keys[i]=="min_cpu_freq" && val.toInt()>=0){ args << "-m" << val; }
    else if(keys[i]=="max_cpu_freq" && val.toInt()>=0){ args << "-M" << val; }
    else if(keys[i]=="polling_interval_ms"){ args << "-p" << val; }
  }
  //Save the arguments into the settings file
  QString device = obj.value("device").toString();
  General::setConfFileValue(_POWERD_CONF, "powerd_args=", "powerd_args=\""+args.join(" ")+"\"" );
  //Restart the moused daemon for this device
  General::RunQuickCommand("service moused."+device+" restart");
  return Cobj; //return the object for the settings we just used
}

//Service status for devices (enable/disable device)
QJsonObject powerd::listStatus(){
  ServiceManager SMGR;
  Service serv = SMGR.GetService("powerd");
  QJsonObject out;
  out.insert("running", SMGR.isRunning(serv) ? "true" : "false" );
  out.insert("enabled", SMGR.isEnabled(serv) ? "true" : "false" );
  return out;
}

QJsonObject powerd::startService(){
  ServiceManager SMGR;
  Service serv = SMGR.GetService("powerd");
  bool ok = SMGR.Start(serv);
  QJsonObject out;
  out.insert("started", ok ? "true" : "false");
  return out;
}

QJsonObject powerd::stopService(){
  ServiceManager SMGR;
  Service serv = SMGR.GetService("powerd");
  bool ok = SMGR.Stop(serv);
  QJsonObject out;
  out.insert("stopped", ok ? "true" : "false");
  return out;
}

QJsonObject powerd::enableService(){
  ServiceManager SMGR;
  Service serv = SMGR.GetService("powerd");
  bool ok = SMGR.Enable(serv);
  QJsonObject out;
  out.insert("enabled", ok ? "true" : "false");
  return out;
}

QJsonObject powerd::disableService(){
  ServiceManager SMGR;
  Service serv = SMGR.GetService("powerd");
  bool ok = SMGR.Disable(serv);
  QJsonObject out;
  out.insert("disabled", ok ? "true" : "false");
  return out;
}
