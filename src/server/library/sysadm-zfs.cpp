//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-zfs.h"
#include "sysadm-general.h"

using namespace sysadm;

QJsonObject ZFS::zpool_list(){
  QJsonObject zpools;
  bool ok = false;
  QStringList info = sysadm::General::RunCommand(ok, "zpool list").split("\n");
  if(!ok || info.length()<2){ return zpools; } //nothing to return
  
  //Line Format (3/2/16): Name/Size/Alloc/Free/Expandsz/Frag/Cap/Dedup/Health/Altroot
  for(int i=1; i<info.length(); i++){ //first line is headers
    if(info[i].isEmpty()){ continue; }
    info[i].replace("\t"," ");
    QString pool = info[i].section(" ",0,0,QString::SectionSkipEmpty);
    QString total = info[i].section(" ",1,1,QString::SectionSkipEmpty);
    QString used = info[i].section(" ",2,2,QString::SectionSkipEmpty);
    QString free = info[i].section(" ",3,3,QString::SectionSkipEmpty);
    QString expandsz = info[i].section(" ",4,4,QString::SectionSkipEmpty);
    QString frag = info[i].section(" ",5,5,QString::SectionSkipEmpty);
    QString cap = info[i].section(" ",6,6,QString::SectionSkipEmpty);
    QString dedup = info[i].section(" ",7,7,QString::SectionSkipEmpty);
    QString health = info[i].section(" ",8,8,QString::SectionSkipEmpty);
    QString altroot = info[i].section(" ",9,9,QString::SectionSkipEmpty);
	
      //Insert the stats for this pool into the object
      QJsonObject zstats;
      zstats.insert("size", total);
      zstats.insert("alloc", used);
      zstats.insert("free", free);
      zstats.insert("frag", frag);
      zstats.insert("expandsz",expandsz);
      zstats.insert("dedup", dedup);
      zstats.insert("altroot", altroot);
      zstats.insert("capacity", cap);
      zstats.insert("health", health);
      zpools.insert(pool, zstats);
    } //end loop over zpool list lines
    return zpools;
}