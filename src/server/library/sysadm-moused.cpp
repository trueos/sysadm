//===========================================
//  TrueOS source code
//  Copyright (c) 2017, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-moused.h"
#include "sysadm-global.h"
#include "sysadm-systemmanager.h"
#include "globals.h"

#define _MOUSED_CONF QString("/etc/conf.d/moused")
#define _MOUSED_SYS_CONF QString("/etc/rc.conf")
#define _MOUSED_DEFAULT_CONF QString("/etc/defaults/rc.conf")

//Setup the Tap To Click (TTC) sysctl definitions
#define _MOUSED_TTC_ENABLED QString("hw.psm.tap_enabled")
#define _MOUSED_TTC_NORMAL QString("hw.psm.tap_timeout")
#define _MOUSED_TTC_SYNAPTICS QString("hw.psm.synaptics.taphold_timeout")
#define _MOUSED_TTC_ENABLE_SYNAPTICS QString("hw.psm.synaptics_support")

//Additional Synaptics controls
#define _MOUSED_SYNAPTICS_TOUCHPAD_OFF QString("hw.psm.synaptics.touchpad_off")
#define _MOUSED_SYNAPTICS_TWOFINGER_SCROLL QString("hw.psm.synaptics.two_finger_scroll")

using namespace sysadm;

QJsonObject moused::listDevices(){
  QJsonObject out;
  //First find all the active devices
  QDir dir("/var/run");
  QStringList devsactive = dir.entryList(QStringList() << "moused-*.pid", QDir::Files, QDir::Name);
  for(int i=0; i<devsactive.length(); i++){ devsactive[i] = devsactive[i].section("-",1,-1).section(".pid",0,0); }
  //Now find *all* the devices
  dir.cd("/dev");
  QStringList devs = dir.entryList(QStringList() << "psm*" << "ums*" << "mse*", QDir::System, QDir::Name);
  //qDebug() << "Found moused devices:" << devs;
  for(int i=0; i<devs.length(); i++){
    QString dev = devs[i];
    QString sysctlprefix = "dev."+dev.left(dev.length()-1)+"."+dev.right(1)+".";
    //qDebug() << " - Device:" << dev << "Sysctl Prefix:" << sysctlprefix;
    QJsonObject devObj;
      devObj.insert("device", dev);
      devObj.insert("description", General::sysctl(sysctlprefix+"%desc"));
      devObj.insert("parent", General::sysctl(sysctlprefix+"%parent"));
      devObj.insert("driver", General::sysctl(sysctlprefix+"%driver"));
      devObj.insert("active", (devsactive.contains(dev)) ? "true" : "false");
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
  out.insert("mouse_scroll_invert", QJsonArray() << "true" << "false");
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

  QString val = General::getConfFileValue(_MOUSED_CONF, "moused_args_"+device+"=" );
  if(val.isEmpty()){ General::getConfFileValue(_MOUSED_SYS_CONF, "moused_flags=" ); }
  if(val.isEmpty()){ General::getConfFileValue(_MOUSED_DEFAULT_CONF, "moused_flags=" ); }
  val = val.simplified();
  if(val.startsWith("\"")){ val.remove(0,1); }
  if(val.endsWith("\"")){ val.chop(1); }
  QStringList args = val.split(" ");
  qDebug() << " - Arguments:" << args;
  QJsonObject out;
  //Now parse the arguments and list them in the output object
  out.insert("emulate_button_3", args.contains("-3") ? "true" : "false");
  int index = args.indexOf("-m");
  bool righthand = true;
  bool scrollinvert = false;
  while(index>=0 && args.length() > (index+1) ){
    if(args[index+1].startsWith("1=")){ righthand = (args[index+1] == "1=1"); }
    else if(args[index+1].startsWith("4=")){ scrollinvert = (args[index+1] == "4=5"); }
    index = args.indexOf("-m", index+1);
  }
  out.insert("mouse_scroll_invert", scrollinvert ? "true" : "false" );
  out.insert("hand_mode", righthand ? "right" : "left");
  out.insert("virtual_scrolling", args.contains("-V") ? "true" : "false" );

  index = args.indexOf("-A");
   val = "1.0";
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
    else if(keys[i]=="mouse_scroll_invert" && val=="true"){ args << "-m" << "4=5" << "-m" << "5=4"; }
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

//Service status for devices (enable/disable device)
QJsonObject moused::listActiveDevices(){
  QDir dir("/var/run");
  QJsonObject out;
  QStringList devsactive = dir.entryList(QStringList() << "moused-*.pid", QDir::Files, QDir::Name);
  for(int i=0; i<devsactive.length(); i++){
    devsactive[i] = devsactive[i].section("-",1,-1).section(".pid",0,0);
  }
  out.insert("active_devices", QJsonArray::fromStringList(devsactive));
  return out;
}

QJsonObject moused::enableDevice(QJsonObject obj){
  if(!obj.contains("device")){ return QJsonObject(); }
  QString device = obj.value("device").toString();
  if(!QFile::exists("/dev/"+device)){ return QJsonObject(); }
  General::RunQuickCommand("service moused."+device+" start");
  QJsonObject out;
  out.insert("started", device);
  return out;
}

QJsonObject moused::disableDevice(QJsonObject obj){
  if(!obj.contains("device")){ return QJsonObject(); }
  QString device = obj.value("device").toString();
  if(!QFile::exists("/dev/"+device)){ return QJsonObject(); }
  General::RunQuickCommand("service moused."+device+" stop");
  QJsonObject out;
  out.insert("stopped", device);
  return out;
}

QJsonObject moused::tapToClick(){
  QJsonObject out;
  QJsonObject tmp;
  tmp.insert("sysctl", QJsonArray() << _MOUSED_TTC_NORMAL << _MOUSED_TTC_SYNAPTICS << _MOUSED_TTC_ENABLED << _MOUSED_TTC_ENABLE_SYNAPTICS);
  tmp = SysMgmt::getSysctl(tmp);
  bool usesynaptics = false;
  int timeout = -1;
  if(tmp.contains(_MOUSED_TTC_ENABLE_SYNAPTICS)){
    usesynaptics = (tmp.value(_MOUSED_TTC_ENABLE_SYNAPTICS).toString().toInt()==1) && tmp.contains(_MOUSED_TTC_SYNAPTICS);
  }

  if(usesynaptics){
    QString level = tmp.value(_MOUSED_TTC_SYNAPTICS).toString();
    if(level.isEmpty()){ level = tmp.value(_MOUSED_TTC_NORMAL).toString(); }
    out.insert("enabled", level.toInt()>0 ? "true" : "false" );
    out.insert("timeout", level);
  }else{
    int enabled = tmp.value(_MOUSED_TTC_ENABLED).toString().toInt();
    if(enabled<0){ out.insert("enabled", "unavailable"); }
    else{ out.insert("enabled", (enabled==1) ? "true" : "false" ); }
    out.insert("timeout", tmp.value(_MOUSED_TTC_NORMAL).toString() );
  }
  out.insert("using_synaptics", usesynaptics ? "true" : "false");
  return out;
}

QJsonObject moused::setTapToClick(QJsonObject jsin){
  QJsonObject out;
  //Check the inputs first
  if(!jsin.contains("enable") && !jsin.contains("timeout")){
    return out;
  }
  //Find out which sysctls need to be set (only some systems have the synaptics option)
  QJsonObject tmp;
  tmp.insert("sysctl", QJsonArray() << _MOUSED_TTC_NORMAL << _MOUSED_TTC_SYNAPTICS  << _MOUSED_TTC_ENABLED << _MOUSED_TTC_ENABLE_SYNAPTICS);
  tmp = SysMgmt::getSysctl(tmp); //this will only return valid sysctls - can use it for quick filtering/detection
  QStringList sysctls = tmp.keys();
  bool usesynaptics = false;
  if(tmp.contains(_MOUSED_TTC_ENABLE_SYNAPTICS)){ usesynaptics = (tmp.value(_MOUSED_TTC_ENABLE_SYNAPTICS).toString().toInt()==1) && tmp.contains(_MOUSED_TTC_SYNAPTICS); }
  bool canenable = (tmp.value(_MOUSED_TTC_ENABLED).toString()!="-1" );
  //Update the timeout as needed
  if(jsin.contains("timeout")){
    int ms = jsin.value("timeout").toInt(-1); //-1 is the default non-valid number
    if(ms<0){ ms = jsin.value("timeout").toString().toInt(); } //try a string->integer instead
    if(ms>=0){
      tmp = QJsonObject(); //clear it for re-use
      tmp.insert("value",QString::number(ms) );
      if(sysctls.contains(_MOUSED_TTC_NORMAL)){ tmp.insert("sysctl",_MOUSED_TTC_NORMAL); SysMgmt::setSysctl(tmp); }
      if(sysctls.contains(_MOUSED_TTC_SYNAPTICS)){ tmp.insert("sysctl",_MOUSED_TTC_SYNAPTICS); SysMgmt::setSysctl(tmp); }
      out.insert("timeout", QString::number(ms) );
    }
  }
  //Enable/Disable as needed
  if(jsin.contains("enable") && canenable ){
    bool enable = (jsin.value("enable").toString().toLower()=="true");
    tmp = QJsonObject(); //clear it for re-use
    if(usesynaptics){
      if(!sysctls.contains("timeout")){ //if we just set this, don't overwrite it
        tmp.insert("value", enable ? "125000" : "0"); //default values for enable/disable
        tmp.insert("sysctl", _MOUSED_TTC_SYNAPTICS);
      }
    }else{
      tmp.insert("value", enable ? "1" : "0");
      tmp.insert("sysctl", _MOUSED_TTC_ENABLED);
    }
    //Now make the actual change
    if(!tmp.isEmpty()){
      SysMgmt::setSysctl(tmp);
      out.insert("enabled", enable ? "true" : "false");
    }
  }
  //Restart the moused daemon if we made any changes
  if(!out.isEmpty()){ General::RunQuickCommand("service moused restart"); }
  return out;
}

QJsonObject moused::synapticsSettings(){
  QJsonObject tmp;
  tmp.insert("sysctl", QJsonArray() << _MOUSED_SYNAPTICS_TOUCHPAD_OFF << _MOUSED_SYNAPTICS_TWOFINGER_SCROLL << _MOUSED_TTC_ENABLE_SYNAPTICS);
  SysMgmt::getSysctl(tmp);
  bool touch_off = false;
  bool two_finger_scroll = false;
  bool enabled = false;
  if(tmp.contains(_MOUSED_SYNAPTICS_TOUCHPAD_OFF)){ touch_off = (tmp.value(_MOUSED_SYNAPTICS_TOUCHPAD_OFF).toString().toInt()==1); }
  if(tmp.contains(_MOUSED_SYNAPTICS_TWOFINGER_SCROLL)){ two_finger_scroll = (tmp.value(_MOUSED_SYNAPTICS_TWOFINGER_SCROLL).toString().toInt()==1); }
  if(tmp.contains(_MOUSED_TTC_ENABLE_SYNAPTICS)){ enabled = (tmp.value(_MOUSED_TTC_ENABLE_SYNAPTICS).toString().toInt()==1); }
  QJsonObject out;
  out.insert("disable_touchpad", touch_off ? "true" : "false");
  out.insert("enable_two_finger_scroll", two_finger_scroll ? "true" : "false");
  out.insert("enable_synaptics", enabled ? "true" : "false");
  return out;
}
