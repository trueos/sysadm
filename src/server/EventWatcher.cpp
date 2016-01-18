// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#include "EventWatcher.h"

// === PUBLIC ===
EventWatcher::EventWatcher(){
  watcher = new QFileSystemWatcher(this);
	
  connect(watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(WatcherUpdate(QString)) );
  connect(watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(WatcherUpdate(QString)) );
}

EventWatcher::~EventWatcher(){
	
}

void EventWatcher::start(){
  // - DISPATCH Events
  if(!QFile::exists(DISPATCHWORKING)){ QProcess::execute("touch "+DISPATCHWORKING); }
  qDebug() << " Dispatcher Events:" << DISPATCHWORKING;
  watcher->addPath(DISPATCHWORKING);
  WatcherUpdate(DISPATCHWORKING); //load it initially
}

EventWatcher::EVENT_TYPE EventWatcher::typeFromString(QString typ){
  if(typ=="dispatcher"){ return DISPATCHER; }
  else if(typ=="life-preserver"){ return LIFEPRESERVER; }
  else{ return BADEVENT; }
}

QJsonValue EventWatcher::lastEvent(EVENT_TYPE type){
  if(HASH.contains(type)){ return HASH.value(type); }
  else{ return ""; }
}

// === PRIVATE ===
QString EventWatcher::readFile(QString path){
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){ return ""; }
  QTextStream in(&file);
  QString contents = in.readAll();
  file.close();
  if(contents.endsWith("\n")){ contents.chop(1); }
  return contents;  
}

// === PRIVATE SLOTS ===
void EventWatcher::WatcherUpdate(QString path){
  if(path==DISPATCHWORKING){
    //Read the file contents
    QString stat = readFile(DISPATCHWORKING);
    if(stat.simplified().isEmpty()){ stat = "idle"; }
    qDebug() << "Dispatcher Update:" << stat;
    HASH.insert(DISPATCHER,stat); //save for later
    //Forward those contents on to the currently-open sockets
    emit NewEvent(DISPATCHER, stat);
  }else if(path==LPLOG){
    //Main Life Preserver Log File
	  
  }else if(path==LPERRLOG){
    //Life Preserver Error log
	  
  }else if(path==tmpLPRepFile){
    //Life Preserver Replication Log (currently-running replication)
	  
  }
  
  //Make sure this file/dir is not removed from the watcher
  if(!watcher->files().contains(path) && !watcher->directories().contains(path)){
    watcher->addPath(path); //re-add it to the watcher. This happens when the file is removed/re-created instead of just overwritten
  }
}