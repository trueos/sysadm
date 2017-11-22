//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-systemmanager.h"
#include "sysadm-global.h"
//need access to the global DISPATCHER object
#include "globals.h"

using namespace sysadm;


//Battery Availability
QJsonObject SysMgmt::batteryInfo(){
  QJsonObject retObject;
  bool ok;
  QString tmp; //temporary string for use later

  //Battery Charging State
  QStringList vals = General::RunCommand("apm -alt").split("\n");
  int state =-1;
  if(vals.length()>=1){ state = vals[0].toInt(&ok); }
  //Battery Charge State is 255 if no battery available
  if (ok && state == 255 ){
    retObject.insert("battery", "false");
    return retObject;
  }
  retObject.insert("battery", "true");
  // Charging state
  if ( ok && state == 0 )
    retObject.insert("status", "offline");
  else if ( ok && state == 1 )
    retObject.insert("status", "charging");
  else if ( ok && state == 2 )
    retObject.insert("status", "backup");
  else
    retObject.insert("status", "unknown");
  // Charging Level
  int level = -1;
  ok = false;
  if(vals.length()>=2){ level = vals[1].toInt(&ok); }
  if ( ok ) {
    tmp.setNum(level);
    retObject.insert("level", tmp);
  } else {
    retObject.insert("level", "-1");
  }
  // Time Left (seconds)
  int timeleft = -1;
  ok = false;
  if(vals.length()>=3){ timeleft= vals[2].toInt(&ok); }
  if ( ok ) {
    tmp.setNum(timeleft);
    retObject.insert("timeleft", tmp);
  } else {
    retObject.insert("timeleft", "-1");
  }

  return retObject;
}

// KPM 1-21-2016
// This needs to be looked at, I'm not 100% sure it is returning correct busy %
// We probably want to supply more info as well, such as user,nice,system,interrupt,idle
QJsonObject SysMgmt::cpuPercentage() {
  QJsonObject retObject;
  QString tmp;

  //Calculate the percentage based on the kernel information directly - no extra utilities
  QStringList result = General::RunCommand("sysctl -n kern.cp_times").split(" ");
  static QStringList last = QStringList();
  if(last.isEmpty()){
    //need two ticks before it works properly
    sleep(1);
    last = result;
    result = General::RunCommand("sysctl -n kern.cp_times").split(" ");
  }
  double tot = 0;
  double roundtot;
  int cpnum = 0;
  for(int i=4; i<result.length(); i+=5){
     //The values come in blocks of 5 per CPU: [user,nice,system,interrupt,idle]
     cpnum++; //the number of CPU's accounted for (to average out at the end)
     //qDebug() <<"CPU:" << cpnum;
     long sum = 0;
     //Adjust/all the data to correspond to diffs from the previous check
     for(int j=0; j<5; j++){
       QString tmp = result[i-j];
       result[i-j] = QString::number(result[i-j].toLong()-last[i-j].toLong());
       //need the difference between last run and this one
       sum += result[i-j].toLong();
       last[i-j] = tmp; //make sure to keep the original value around for the next run
     }
     QJsonObject vals;
     roundtot = 100.0L - ( (100.0L*result[i].toLong())/sum );
     tmp.setNum(qRound(roundtot));
     vals.insert("busy", tmp );
     tmp.setNum(cpnum);
     retObject.insert("cpu" + tmp, vals);
     //Calculate the percentage used for this CPU (100% - IDLE%)
     tot += 100.0L - ( (100.0L*result[i].toLong())/sum ); //remember IDLE is the last of the five values per CPU
   }

  // Add the total busy %
  tmp.setNum(qRound(tot/cpnum));
  retObject.insert("busytotal", tmp);
  return retObject;
}

QJsonObject SysMgmt::cpuTemps() {
  // Make sure coretemp is loaded
  if ( General::RunCommand("kldstat").indexOf("coretemp") == -1 )
    General::RunCommand("kldload coretemp");

  QJsonObject retObject;
  QStringList temps;
  temps = General::RunCommand("sysctl -ai").split("\n").filter(".temperature:");
  temps.sort();
  for(int i=0; i<temps.length(); i++){
    if(temps[i].contains(".acpi.") || temps[i].contains(".cpu")){
      retObject.insert(temps[i].section(":", 0, 0).section(".", 1,2).replace(".", "").simplified(), temps[i].section(":", 1,5).simplified());
    }else{
      //non CPU temperature - skip it
      temps.removeAt(i); i--;
    }
  }
  return retObject;
}

// ==== ExternalDevicePaths() ====
QJsonObject SysMgmt::externalDevicePaths() {
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

// Kill a process
QJsonObject SysMgmt::killProc(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("pid") ) {
    retObject.insert("error", "Missing required key 'pid'");
    return retObject;
  }
  // Save the target
  QString pid, sig, tmp;
  pid = jsin.value("pid").toString();

  if ( keys.contains("signal") ) {
    tmp  = jsin.value("signal").toString();
    if ( tmp == "HUP" ) {
      sig = "-1";
    } else if ( tmp == "INT" ) {
      sig = "-2";
    } else if ( tmp == "QUIT" ) {
      sig = "-3";
    } else if ( tmp == "ABRT" ) {
      sig = "-6";
    } else if ( tmp == "KILL" ) {
      sig = "-9";
    } else if ( tmp == "ALRM" ) {
      sig = "-14";
    } else if ( tmp == "TERM" ) {
      sig = "-15";
    } else {
      retObject.insert("error", "Invalid signal! (INT|QUIT|ABRT|KILL|ALRM|TERM)");
      return retObject;
    }
  } else {
    sig = "-9";
  }

  // This could be switched to kill(2) at some point
  General::RunCommand("kill " + sig + " "  + pid);

  return jsin;
}

// Return information about memory
QJsonObject SysMgmt::memoryStats() {
  QJsonObject retObject;

  QString tmp;
  long pageSize;
  bool ok;

  // Get the page size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_page_size").simplified();
  tmp.toLong(&ok);
  if ( ok )
    pageSize = tmp.toLong();
  else
    return retObject;

  // Get the free size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_free_count").simplified();
  tmp.toLong(&ok);
  if ( ok )
    retObject.insert("free", tmp.setNum((tmp.toLong() * pageSize) / 1024 / 1024));

  // Get the inactive size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_inactive_count").simplified();
  tmp.toLong(&ok);
  if ( ok )
    retObject.insert("inactive", tmp.setNum((tmp.toLong() * pageSize) / 1024 / 1024));

  // Get the cache size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_cache_count").simplified();
  tmp.toLong(&ok);
  if ( ok )
    retObject.insert("cache", tmp.setNum((tmp.toLong() * pageSize) / 1024 / 1024));

  // Get the wired size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_wire_count").simplified();
  tmp.toLong(&ok);
  if ( ok )
    retObject.insert("wired", tmp.setNum((tmp.toLong() * pageSize) / 1024 / 1024));

  // Get the active size
  tmp = General::RunCommand("sysctl -n vm.stats.vm.v_active_count").simplified();
  tmp.toLong(&ok);
  if ( ok )
    retObject.insert("active", tmp.setNum((tmp.toLong() * pageSize) / 1024 / 1024));

  return retObject;
}

// Return a json list of process information
QJsonObject SysMgmt::procInfo() {
  QJsonObject retObject;
  QStringList output;
  output = General::RunCommand("top -t -n -a all").split("\n");
  bool inSection = false;
  for(int i=0; i<output.length(); i++){
    if (output.at(i).contains("PID") && output.at(i).contains("USERNAME")){
      inSection = true;
      continue;
    }
    if (!inSection)
      continue;
    if ( output.at(i).isEmpty())
      continue;

    // Now, lets break down the various elements of process information
    QJsonObject values;
    QString line = output.at(i).simplified();
    QString pid = line.section(" ", 0, 0);
    values.insert("username", line.section(" ", 1, 1));
    values.insert("thr", line.section(" ", 2, 2));
    values.insert("pri", line.section(" ", 3, 3));
    values.insert("nice", line.section(" ", 4, 4));
    values.insert("size", line.section(" ", 5, 5));
    values.insert("res", line.section(" ", 6, 6));
    values.insert("state", line.section(" ", 7, 7));
    values.insert("cpu", line.section(" ", 8, 8));
    values.insert("time", line.section(" ", 9, 9));
    values.insert("wcpu", line.section(" ", 10, 10));
    values.insert("command", line.section(" ", 11, -1));

    // Add the PID object
    retObject.insert(pid, values);
  }
  return retObject;
}

// Get a sysctl
QJsonObject SysMgmt::getSysctl(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList sysctl;
  if(jsin.value("sysctl").isArray()){ sysctl = General::JsonArrayToStringList(jsin.value("sysctl").toArray()); }
  else if(jsin.value("sysctl").isString()){ sysctl << jsin.value("sysctl").toString(); }
  if ( sysctl.isEmpty() ) {
    retObject.insert("error", "Missing required key 'sysctl'");
    return retObject;
  }

  QStringList output = General::RunCommand("sysctl", sysctl).split("\n");
  for(int i=0; i<output.length(); i++){
    if( !output[i].contains(":") || output[i].contains("unknown oid") ){ continue; }
    retObject.insert(output[i].section(":",0,0), output[i].section(":",1,-1).simplified());
  }
  return retObject;
}

// Set a sysctl
QJsonObject SysMgmt::setSysctl(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("sysctl") || ! keys.contains("value") ) {
    retObject.insert("error", "Missing required keys 'sysctl / value'");
    return retObject;
  }
  QString sysctl, value;
  sysctl = jsin.value("sysctl").toString();
  value = jsin.value("value").toString();
  QString output = General::RunCommand("sysctl " + sysctl + "=" + value);

  retObject.insert("sysctl", sysctl);
  retObject.insert("value", value);
  retObject.insert("response", output.simplified());

  return retObject;
}

// Return list of sysctls and their values
QJsonObject SysMgmt::sysctlList() {
  QJsonObject retObject;

  // This can be cleaned up and not use CLI
  QStringList output = General::RunCommand("sysctl -aW").split("\n");
  QStringList details = General::RunCommand("sysctl -aWdt").split("\n");

  QString sysctl, type, description;
  for(int i=0; i<output.length(); i++){
    if ( output.at(i).isEmpty()){ continue; }
    QJsonObject tmp;
    sysctl = output.at(i).section(":", 0, 0);
    tmp.insert("value", output.at(i).section(":", 1, -1).simplified() );
    retObject.insert(sysctl, tmp);
  }
  for(int i=0; i<details.length(); i++){
    if ( details.at(i).isEmpty() ){ continue; }
    sysctl = details[i].section(":",0,0);
    type = details[i].section(":",1,1).simplified();
    description = details[i].section(":",2,-1).simplified();
    if(retObject.contains(sysctl)){
      QJsonObject tmp = retObject.value(sysctl).toObject();
      tmp.insert("type", type);
      if(!description.isEmpty()){ tmp.insert("description", description); }
      retObject.insert(sysctl, tmp); //replace the sysctl object into the output object
    }
  }
  return retObject;
}

// Return a bunch of various system information
QJsonObject SysMgmt::systemInfo() {
  QJsonObject retObject;

  QString arch = General::RunCommand("uname -m").simplified();
  retObject.insert("arch", arch);

  QString sysver = General::RunCommand("freebsd-version").simplified();
  retObject.insert("systemversion", sysver);

  QString kernver = General::RunCommand("uname -r").simplified();
  retObject.insert("kernelversion", kernver);

  QString kernident = General::RunCommand("uname -i").simplified();
  retObject.insert("kernelident", kernident);

  QString host = General::RunCommand("hostname").simplified();
  retObject.insert("hostname", host);

  QString uptime = General::RunCommand("uptime").simplified().section(" ", 1, 4).simplified().replace(",", "");
  retObject.insert("uptime", uptime);

  QString cputype = General::RunCommand("sysctl -n hw.model").simplified();
  retObject.insert("cputype", cputype);

  QString cpucores = General::RunCommand("sysctl -n kern.smp.cpus").simplified();
  retObject.insert("cpucores", cpucores);

  bool ok;
  QString tmp;
  QString totalmem = General::RunCommand("sysctl -n hw.realmem").simplified();
  totalmem.toLong(&ok);
  if ( ok ) {
    retObject.insert("totalmem", tmp.setNum(totalmem.toLong() / 1024 / 1024));
  }

  return retObject;
}

// Halt the box
QJsonObject SysMgmt::systemHalt() {
  QJsonObject retObject;

  QString output = General::RunCommand("shutdown -p now");
  retObject.insert("response", "true");

  return retObject;
}


// Reboot the box
QJsonObject SysMgmt::systemReboot() {
  QJsonObject retObject;

  QString output = General::RunCommand("shutdown -r now");
  retObject.insert("response", "true");

  return retObject;
}

// Get all device information about the system
QJsonObject SysMgmt::systemDevices(){
  QJsonObject pciconf_info;
  QStringList lines = General::RunCommand("pciconf -lv").split("\n");
  //qDebug() << "Raw pciconf:" << lines;
  QJsonObject cobj; QString cid;
  for(int i=0; i<lines.length(); i++){
    if(!lines[i].contains(" = ") && !cid.isEmpty()){ pciconf_info.insert(cid,cobj); cid.clear(); cobj = QJsonObject(); }
    if(lines[i].contains(" = ")){
      QString var = lines[i].section("=",0,0).simplified();
      QString val = lines[i].section("=",1,-1).simplified();
      if(val.startsWith("\'") && val.endsWith("\'")){ val.chop(1); val = val.remove(0,1); }
      //qDebug() << "PCICONF LINE:" << lines[i];
	//qDebug() << "\t\t" << var << val;
      cobj.insert(var,val);
    }else{
      //New device section
      cid = lines[i].section("@",0,0);
    }
  }
  if(!cid.isEmpty() && cobj.keys().isEmpty()){ pciconf_info.insert(cid,cobj); }
  return pciconf_info;
}

