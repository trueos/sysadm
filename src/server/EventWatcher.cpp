// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#include "EventWatcher.h"

#include "globals.h"

// === PUBLIC ===
EventWatcher::EventWatcher(){
  qRegisterMetaType<EventWatcher::EVENT_TYPE>("EventWatcher::EVENT_TYPE");
	
  starting = true;
  watcher = new QFileSystemWatcher(this);
  filechecktimer = new QTimer(this);
    filechecktimer->setSingleShot(false);
    filechecktimer->setInterval(3600000); //1-hour checks (also run on new event notices)
  connect(watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(WatcherUpdate(const QString&)) );
  connect(watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(WatcherUpdate(const QString&)) );
  connect(filechecktimer, SIGNAL(timeout()), this, SLOT( CheckLogFiles()) );
}

EventWatcher::~EventWatcher(){
}

void EventWatcher::start(){
  starting = true;
  // - Life Preserver Events
  WatcherUpdate(LPLOG); //load it initially (will also add it to the watcher);
  WatcherUpdate(LPERRLOG); //load it initially (will also add it to the watcher);
  
  filechecktimer->start();
  starting = false;
}

EventWatcher::EVENT_TYPE EventWatcher::typeFromString(QString typ){
  if(typ=="dispatcher"){ return DISPATCHER; }
  else if(typ=="life-preserver"){ return LIFEPRESERVER; }
  else{ return BADEVENT; }
}

QString EventWatcher::typeToString(EventWatcher::EVENT_TYPE typ){
  if(typ==DISPATCHER){ return "dispatcher"; }
  else if(typ==LIFEPRESERVER){ return "life-preserver"; }
  else{ return ""; }
}

QJsonValue EventWatcher::lastEvent(EVENT_TYPE type){
  CheckLogFiles();
  if(HASH.contains(type)){ return HASH.value(type); }
  else{ qDebug() << "No saved event:" << type; return QJsonValue(); }
}

// === PRIVATE ===

void EventWatcher::sendLPEvent(QString system, int priority, QString msg){
  QJsonObject obj;
  obj.insert("message",msg);
  obj.insert("priority", DisplayPriority(priority) );
  obj.insert("class" , system);
  HASH.insert(LIFEPRESERVER, obj);
  //qDebug() << "New LP Event Object:" << obj;
  LogManager::log(LogManager::EV_LP, obj);
  if(!starting){ emit NewEvent(LIFEPRESERVER, obj); }
}

// === General Purpose Functions
QString EventWatcher::readFile(QString path){
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){ return ""; }
  QTextStream in(&file);
  QString contents = in.readAll();
  file.close();
  if(contents.endsWith("\n")){ contents.chop(1); }
  return contents;  
}

double EventWatcher::displayToDoubleK(QString displayNumber){
  QStringList labels; 
    labels << "K" << "M" << "G" << "T" << "P" << "E";
  QString clab = displayNumber.right(1); //last character is the size label
	displayNumber.chop(1); //remove the label from the number
  double num = displayNumber.toDouble();
  //Now format the number properly
  bool ok = false;
  clab = clab.toUpper();
  for(int i=0; i<labels.length(); i++){
    if(labels[i] == clab){ ok = true; break; }
    else{ num = num*1024; } //get ready for the next size
  }
  if(!ok){ num = -1; } //could not determine the size
  return num;
}

// === PUBLIC SLOTS ===
//Slots for the global Dispatcher to connect to
void EventWatcher::DispatchStarting(QString ID){
  QJsonObject obj;
  obj.insert("process_id", ID);
  obj.insert("state", "running");
  LogManager::log(LogManager::EV_DISPATCH, obj);	
  emit NewEvent(DISPATCHER, obj);
}

void EventWatcher::DispatchFinished(QJsonObject obj){
  LogManager::log(LogManager::EV_DISPATCH, obj);
  emit NewEvent(DISPATCHER, obj);	
}

// === PRIVATE SLOTS ===
void EventWatcher::WatcherUpdate(const QString &path){
  if(!starting){ qDebug() << "Event Watcher Update:" << path; }
  if(path==LPLOG){
    //Main Life Preserver Log File
    ReadLPLogFile();
  }else if(path==LPERRLOG){
    //Life Preserver Error log
    ReadLPErrFile();
  }else if(path==tmpLPRepFile){
    //Life Preserver Replication Log (currently-running replication)
    ReadLPRepFile();
  }else{
    //This file should no longer be watched (old replication file?)
    if(watcher->files().contains(path) || watcher->directories().contains(path)){
      watcher->removePath(path);
    }
  }
  QTimer::singleShot(10,this, SLOT(CheckLogFiles()) ); //check for any other missing files
}

void EventWatcher::CheckLogFiles(){
  //Make sure all the proper files are being watched
  QStringList watched; watched << watcher->files() << watcher->directories();
  if(!watched.contains(LPLOG) && QFile::exists(LPLOG)){ watcher->addPath(LPLOG); WatcherUpdate(LPLOG); }
  if(!watched.contains(LPERRLOG) && QFile::exists(LPERRLOG)){ watcher->addPath(LPERRLOG); WatcherUpdate(LPERRLOG); }
  if(!watched.contains(tmpLPRepFile) && QFile::exists(tmpLPRepFile)){ watcher->addPath(tmpLPRepFile);  WatcherUpdate(tmpLPRepFile); }
  //qDebug() << "watched:" << watcher->files() << watcher->directories();
}

// == Life Preserver Event Functions
void EventWatcher::ReadLPLogFile(){
  //Open/Read any new info in the file
  QFile LPlogfile(LPLOG);
  if( !LPlogfile.open(QIODevice::ReadOnly) ){ return; } //could not open file
  QTextStream STREAM(&LPlogfile);
  qint64 LPlog_pos = CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-log-pos",0).toLongLong();
  if(LPlog_pos>0 && QFileInfo(LPlogfile).created() < CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-log-lastread").toDateTime() ){ 
    STREAM.seek(LPlog_pos); 
  }
  QStringList info = STREAM.readAll().split("\n");
  //Now save the file pointer for later
  CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-log-pos",STREAM.pos());
  CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-log-lastread",QDateTime::currentDateTime());
  LPlogfile.close();
  //Now parse the new info line-by-line
  for(int i=0; i<info.length(); i++){
    if(info[i].isEmpty()){ continue; }
    QString log = info[i];
    if(!starting){ qDebug() << "Read LP Log File Line:" << log; }
    //Divide up the log into it's sections
    QString timestamp = log.section(":",0,2).simplified();
    QString time = timestamp.section(" ",3,3).simplified();
    QString message = log.section(":",3,3).toLower().simplified();
    QString dev = log.section(":",4,4).simplified(); //dataset/snapshot/nothing

    //Now decide what to do/show because of the log message
    if(message.contains("creating snapshot", Qt::CaseInsensitive)){
      dev = message.section(" ",-1).simplified();
      QString msg = QString(tr("New snapshot of %1")).arg(dev);
      //Setup the status of the message
      HASH.insert(110,"SNAPCREATED");
      HASH.insert(111,dev); //dataset
      HASH.insert(112, msg ); //summary
      HASH.insert(113, QString(tr("Creating snapshot for %1")).arg(dev) );
      HASH.insert(114, timestamp); //full timestamp
      HASH.insert(115, time); // time only
      sendLPEvent("snapshot", 1, timestamp+": "+msg);
    }else if(message.contains("Starting replication", Qt::CaseInsensitive)){
      //Setup the file watcher for this new log file
      //qDebug() << " - Found Rep Start:" << dev << message;
      tmpLPRepFile = dev;
       CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-pos",0);
      dev = message.section(" on ",1,1,QString::SectionSkipEmpty);
      //qDebug() << " - New Dev:" << dev << "Valid Pools:" << reppools;
      //Make sure the device is currently setup for replication
      //if( !reppools.contains(dev) ){ FILE_REPLICATION.clear(); continue; }
      QString msg = QString(tr("Starting replication for %1")).arg(dev);
        //Set the appropriate status variables
        HASH.insert(120,"STARTED");
        HASH.insert(121, dev); //zpool
        HASH.insert(122, tr("Replication Starting") ); //summary
        HASH.insert(123, msg ); //Full message
        HASH.insert(124, timestamp); //full timestamp
        HASH.insert(125, time); // time only
        HASH.insert(126,tr("Replication Log")+" <"+tmpLPRepFile+">"); //log file
        sendLPEvent("replication", 1, timestamp+": "+msg);
    }else if(message.contains("finished replication task", Qt::CaseInsensitive)){
      //Done with this replication - close down the rep file watcher
        tmpLPRepFile.clear();
	CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-pos",0);
      dev = message.section(" -> ",0,0).section(" ",-1).simplified();
      //Make sure the device is currently setup for replication
      //if( reppools.contains(dev) ){
	QString msg = QString(tr("Finished replication for %1")).arg(dev);
        //Now set the status of the process
        HASH.insert(120,"FINISHED");
        HASH.insert(121,dev); //dataset
        HASH.insert(122, tr("Finished Replication") ); //summary
        HASH.insert(123, msg );
        HASH.insert(124, timestamp); //full timestamp
        HASH.insert(125, time); // time only
        HASH.insert(126, ""); //clear the log file entry
        sendLPEvent("replication", 1, timestamp+": "+msg);
    }else if( message.contains("FAILED replication", Qt::CaseInsensitive) ){
        tmpLPRepFile.clear();
	CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-pos",0);
      //Now set the status of the process
      dev = message.section(" -> ",0,0).section(" ",-1).simplified();
      //Make sure the device is currently setup for replication
	//Update the HASH
        QString file = log.section("LOGFILE:",1,1).simplified();
        QString tt = QString(tr("Replication Failed for %1")).arg(dev) +"\n"+ QString(tr("Logfile available at: %1")).arg(file);
        HASH.insert(120,"ERROR");
        HASH.insert(121,dev); //dataset
        HASH.insert(122, tr("Replication Failed") ); //summary
        HASH.insert(123, tt );
        HASH.insert(124, timestamp); //full timestamp
        HASH.insert(125, time); // time only      
        HASH.insert(126, tr("Replication Error Log")+" <"+file+">" );
        sendLPEvent("replication", 7, timestamp+": "+tt);
    }
	  
  }
}

void EventWatcher::ReadLPErrFile(){
	
}

void EventWatcher::ReadLPRepFile(){
  QString stat = CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-s","").toString();
  QString repTotK = CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-totk","").toString();
  QString lastSize = CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-lastsize","").toString();
 //Open/Read any new info in the file
  QFile LPlogfile(LPLOG);
  if( !LPlogfile.open(QIODevice::ReadOnly) ){ return; } //could not open file
  QTextStream STREAM(&LPlogfile);
   qint64 LPrep_pos = CONFIG->value("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-pos",0).toLongLong();
  if(LPrep_pos<=0 || !STREAM.seek(LPrep_pos) ){
    //New file location
    stat.clear();
    repTotK.clear();
    lastSize.clear();	  
  }
  QStringList info = STREAM.readAll().split("\n");
  CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-pos",STREAM.pos());
  LPlogfile.close();
  //Now parse the new info line-by-line
  for(int i=0; i<info.length(); i++){
    QString line = info[i];
    if(line.contains("estimated size is")){ repTotK = line.section("size is ",1,1,QString::SectionSkipEmpty).simplified(); } //save the total size to replicate
    else if(line.startsWith("send from ")){}
    else if(line.startsWith("TIME ")){}
    else if(line.startsWith("warning: ")){} //start of an error
    else{ stat = line; } //only save the relevant/latest status line
  }
  if(!stat.isEmpty()){
    //qDebug() << "New Status Message:" << stat;	    
    //Divide up the status message into sections
    stat.replace("\t"," ");
    QString dataset = stat.section(" ",2,2,QString::SectionSkipEmpty).section("/",0,0).simplified();
    QString cSize = stat.section(" ",1,1,QString::SectionSkipEmpty);
    //Now Setup the tooltip
    if(cSize != lastSize){ //don't update the info if the same size info
      QString percent;
      if(!repTotK.isEmpty() && repTotK!="??"){
        //calculate the percentage
        double tot = displayToDoubleK(repTotK);
        double c = displayToDoubleK(cSize);
        if( tot!=-1 & c!=-1){
          double p = (c*100)/tot;
	  p = int(p*10)/10.0; //round to 1 decimel places
	  percent = QString::number(p) + "%";
        }
      }
      if(repTotK.isEmpty()){ repTotK = "??"; }
      //Format the info string
      QString status = cSize+"/"+repTotK;
      if(!percent.isEmpty()){ status.append(" ("+percent+")"); }
      QString txt = QString(tr("Replicating %1: %2")).arg(dataset, status);
      lastSize = cSize; //save the current size for later
      //Now set the current process status
      HASH.insert(120,"RUNNING");
      HASH.insert(121,dataset);
      HASH.insert(122,txt);
      HASH.insert(123,txt);
      emit sendLPEvent("replication", 0, txt);
    }
  }
  //Save the internal values
  CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-stat",stat);
  if(repTotK!="??"){CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-totk",repTotK); }
  CONFIG->setValue("internal/"+QString(WS_MODE ? "ws" : "tcp")+"/lp-rep-lastsize",lastSize);
}
