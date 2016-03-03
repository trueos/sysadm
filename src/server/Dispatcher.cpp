// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#include "Dispatcher.h"

#include "DispatcherParsing.h"

#include "globals.h"


// ================================
//  DProcess Class (Internal)
// ================================
DProcess::DProcess(QObject *parent) : QProcess(parent){
    //Setup the process
    bool notify = false;
    uptimer = new QTimer(this);
      uptimer->setSingleShot(true);
      uptimer->setInterval(1000); //1 second intervals
    connect(uptimer, SIGNAL(timeout()), this, SLOT(emitUpdate()) );
    this->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    this->setProcessChannelMode(QProcess::MergedChannels);
    connect(this, SIGNAL(readyRead()), this, SLOT(updateLog()) );
}

DProcess::~DProcess(){
  if( this->state()!=QProcess::NotRunning ){
    this->terminate();
  }
}
  
void DProcess::startProc(){
  cmds.removeAll(""); //make sure no empty commands
  if(cmds.isEmpty()){ 
    proclog.insert("state","finished");
    proclog.insert("time_finished", QDateTime::currentDateTime().toString(Qt::ISODate));
    proclog.remove("current_cmd");
    emit ProcFinished(ID, proclog);
    return; 
  }
  cCmd = cmds.takeFirst();
  success = false; //not finished yet
  if(proclog.isEmpty()){
    //first cmd started
    proclog.insert("time_started", QDateTime::currentDateTime().toString(Qt::ISODate));
    proclog.insert("cmd_list",QJsonArray::fromStringList(cmds));
    proclog.insert("process_id",ID);
    proclog.insert("state","running");
    //rawcmds = cmds; rawcmds.prepend(cCmd);
    //setup internal connections
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cmdFinished(int, QProcess::ExitStatus)) );
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(cmdError(QProcess::ProcessError)) );
  }
  proclog.insert("current_cmd",cCmd);
  //qDebug() << "Proc Starting:" << ID << cmd;
  this->start(cCmd);
}

bool DProcess::isRunning(){
  return (this->state()!=QProcess::NotRunning);	
}

bool DProcess::isDone(){
  return (!this->isRunning() && !proclog.isEmpty());
}

QJsonObject DProcess::getProcLog(){
  //Now return the current version of the log
  return proclog;
}

void DProcess::cmdError(QProcess::ProcessError err){
  //qDebug() << "Process Error:" << err;
  cmdFinished(-1, QProcess::NormalExit);
}

void DProcess::cmdFinished(int ret, QProcess::ExitStatus status){
  if(uptimer->isActive()){ uptimer->stop(); } //use the finished signal instead
  //determine success/failure
  success = (status==QProcess::NormalExit && ret==0);
  //update the log before starting another command
  proclog.insert(cCmd, proclog.value(cCmd).toString().append(this->readAllStandardOutput()) );
  proclog.insert("return_codes/"+cCmd, QString::number(ret));
  
  //Now run any additional commands
  //qDebug() << "Proc Finished:" << ID << success << proclog;
  if(success && !cmds.isEmpty()){ 
    emit ProcUpdate(ID, proclog);
    startProc();
  }else{
    proclog.insert("state","finished");
    proclog.remove("current_cmd");
    proclog.insert("time_finished", QDateTime::currentDateTime().toString(Qt::ISODate));
    emit ProcFinished(ID, proclog);
  }
}

void DProcess::updateLog(){
  proclog.insert(cCmd, proclog.value(cCmd).toString().append(this->readAllStandardOutput()) );
  if(!uptimer->isActive()){ uptimer->start(); }
}

void DProcess::emitUpdate(){
  emit ProcUpdate(ID, proclog);
}

// ================================
// Dispatcher Class
// ================================
Dispatcher::Dispatcher(){
  qRegisterMetaType<Dispatcher::PROC_QUEUE>("Dispatcher::PROC_QUEUE");
  connect(this, SIGNAL(mkprocs(Dispatcher::PROC_QUEUE, DProcess*)), this, SLOT(mkProcs(Dispatcher::PROC_QUEUE, DProcess*)) );
}

Dispatcher::~Dispatcher(){
	
}

void Dispatcher::start(QString queuefile){
  //load any previously-unrun processes
  // TO DO
}

void Dispatcher::stop(){
  //save any currently-unrun processes for next time the server starts
  // TO DO
}

//Overloaded Main Calling Functions (single command, or multiple in-order commands)
DProcess* Dispatcher::queueProcess(QString ID, QString cmd){
  return queueProcess(NO_QUEUE, ID, QStringList() << cmd);
}
DProcess* Dispatcher::queueProcess(QString ID, QStringList cmds){
  return queueProcess(NO_QUEUE, ID, cmds);	
}
DProcess* Dispatcher::queueProcess(Dispatcher::PROC_QUEUE queue, QString ID, QString cmd){
  return queueProcess(queue, ID, QStringList() << cmd);
}
DProcess* Dispatcher::queueProcess(Dispatcher::PROC_QUEUE queue, QString ID, QStringList cmds){
  //This is the primary queueProcess() function - all the overloads end up here to do the actual work
  //For multi-threading, need to emit a signal/slot for this action (object creations need to be in same thread as parent)
  qDebug() << "Queue Process:" << queue << ID << cmds;
  DProcess *P = createProcess(ID, cmds);
  //connect(this, SIGNAL(mkprocs(Dispatcher::PROC_QUEUE, DProcess*)), this, SLOT(mkProcs(Dispatcher::PROC_QUEUE, DProcess*)) );
  emit mkprocs(queue, P);
  return P;
}

// === PRIVATE ===
//Simplification routine for setting up a process
DProcess* Dispatcher::createProcess(QString ID, QStringList cmds){
  DProcess *P = new DProcess();
    P->moveToThread(this->thread());
    P->cmds = cmds;
    P->ID = ID;
  return P;
}

// === PRIVATE SLOTS ===
void Dispatcher::mkProcs(Dispatcher::PROC_QUEUE queue, DProcess *P){
  //qDebug() << "mkProcs()"; 
  //P->moveToThread(this->thread());
  connect(P, SIGNAL(ProcFinished(QString, QJsonObject)), this, SLOT(ProcFinished(QString, QJsonObject)) );
  connect(P, SIGNAL(ProcUpdate(QString, QJsonObject)), this, SLOT(ProcUpdated(QString, QJsonObject)) );
  QList<DProcess*> list;
  if(!HASH.contains(queue)){ HASH.insert(queue, list); } //insert an empty list
  HASH[queue] << P; //add this proc to the end of the list
  CheckQueues();
}

void Dispatcher::ProcFinished(QString ID, QJsonObject log){
  //Find the process with this ID and close it down (with proper events)
  qDebug() << " - Got Proc Finished Signal:" << ID;
  LogManager::log(LogManager::DISPATCH, log);
  //First emit any subsystem-specific event, falling back on the raw log
  QJsonObject ev = CreateDispatcherEventNotification(ID,log);
  if(!ev.isEmpty()){
    emit DispatchEvent(ev);
  }else{
    emit DispatchEvent(log);
  }

  QTimer::singleShot(20,this, SLOT(CheckQueues()) );
}

void Dispatcher::ProcUpdated(QString ID, QJsonObject log){
  //See if this needs to generate an event
  QJsonObject ev = CreateDispatcherEventNotification(ID,log);
  if(!ev.isEmpty()){
    emit DispatchEvent(ev);
  }
}

void Dispatcher::CheckQueues(){
for(int i=0; i<enum_length; i++){
    PROC_QUEUE queue = static_cast<PROC_QUEUE>(i);
    if(HASH.contains(queue)){
      QList<DProcess*> list = HASH[queue];
      for(int j=0; j<list.length(); j++){
	if(j>0 && queue!=NO_QUEUE){ break; } //done with this - only first item in these queues should run at a time
	if( !list[j]->isRunning() ){
	  if(list[j]->isDone() ){
	    list.takeAt(j)->deleteLater();
	    j--;
	  }else{
	    //Need to start this one - has not run yet
	    qDebug() << "Call Start Proc:" << list[j]->ID;
	    emit DispatchStarting(list[j]->ID);
	    list[j]->startProc();
	    //QTimer::singleShot(0,list[j], SLOT(startProc()) );
	  }
	}
      } //end loop over list
    }
    
  } //end loop over queue types	
}
