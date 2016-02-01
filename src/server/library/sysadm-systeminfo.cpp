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

  int timeleft = General::RunCommand("apm -t").toInt(&ok);
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
QJsonObject SysInfo::cpuPercentage() {
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

QJsonObject SysInfo::cpuTemps() {
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

// Return information about memory
QJsonObject SysInfo::memoryStats() {
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

// Return a bunch of various system information
QJsonObject SysInfo::systemInfo() {
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

