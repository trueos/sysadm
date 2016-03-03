// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#include "Dispatcher.h"

#include "globals.h"


// ================================
//  DProcess Class (Internal)
// ================================
DProcess::DProcess(QObject *parent) : QProcess(parent){
    //Setup the process
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
  if(cmds.isEmpty()){ 
    t_finished = QDateTime::currentDateTime();
    emit ProcFinished(ID); 
    return; 
  }
  QString cmd = cmds.takeFirst();
  success = false; //not finished yet
  if(!proclog.isEmpty()){ proclog.append("\n"); }
  else{ //first cmd started
    t_started = QDateTime::currentDateTime();
    rawcmds = cmds; rawcmds.prepend(cmd);
    //setup internal connections
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cmdFinished(int, QProcess::ExitStatus)) );
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(cmdError(QProcess::ProcessError)) );
  } 
  proclog.append("[Running Command: "+cmd+" ]");
  //qDebug() << "Proc Starting:" << ID << cmd;
  this->start(cmd);
}

bool DProcess::isRunning(){
  return (this->state()!=QProcess::NotRunning);	
}

bool DProcess::isDone(){
  return (!this->isRunning() && !proclog.isEmpty());
}

QString DProcess::getProcLog(){
  //Now return the current version of the log
  return proclog;
}

void DProcess::cmdError(QProcess::ProcessError err){
  //qDebug() << "Process Error:" << err;
  cmdFinished(-1, QProcess::NormalExit);
}

void DProcess::cmdFinished(int ret, QProcess::ExitStatus status){
  //determine success/failure
  success = (status==QProcess::NormalExit && ret==0);
  //update the log before starting another command
  proclog.append( this->readAllStandardOutput() );
  //Now run any additional commands
  //qDebug() << "Proc Finished:" << ID << success << proclog;
  if(success && !cmds.isEmpty()){ startProc(); }
  else if(success){
    t_finished = QDateTime::currentDateTime();
    emit ProcFinished(ID);
    emit Finished(ID, ret, proclog);
  }else{
    if(status==QProcess::NormalExit){
      proclog.append("\n[Command Failed: " + QString::number(ret)+" ]");
    }else{
      proclog.append("\n[Command Failed: Process Crashed ]");
    }
    t_finished = QDateTime::currentDateTime();
    emit ProcFinished(ID);
    emit Finished(ID, ret, proclog);
  }
}

void DProcess::updateLog(){
  proclog.append( this->readAllStandardOutput() );
  emit ProcessOutput(proclog);
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
  connect(P, SIGNAL(ProcFinished(QString)), this, SLOT(ProcFinished(QString)) );
  QList<DProcess*> list;
  if(!HASH.contains(queue)){ HASH.insert(queue, list); } //insert an empty list
  HASH[queue] << P; //add this proc to the end of the list
  CheckQueues();
}

void Dispatcher::ProcFinished(QString ID){
  //Find the process with this ID and close it down (with proper events)
  qDebug() << " - Got Proc Finished Signal:" << ID;
  /*bool found = false;
  for(int i=0; i<enum_length && !found; i++){
    Dispatcher::PROC_QUEUE queue = static_cast<Dispatcher::PROC_QUEUE>(i);
    if(HASH.contains(queue)){
      QList<DProcess*> list = HASH[queue];
      for(int l=0; l<list.length() && !found; l++){
        if(list[l]->ID==ID){ 
	  qDebug() << "Delete Proc on Finished:" << list[l]->ID;
	  QJsonObject obj;
	  obj.insert("log",list[l]->getProcLog());
	  obj.insert("success", list[l]->success ? "true" : "false" );
	  obj.insert("proc_id", ID);
	  obj.insert("cmd_list", QJsonArray::fromStringList( list[l]->rawcmds ) );
	  obj.insert("time_started", list[l]->t_started.toString(Qt::ISODate) );
	  obj.insert("time_finished", list[l]->t_finished.toString(Qt::ISODate) );
	  emit DispatchFinished(obj);
	  list.takeAt(l)->deleteLater();
	  LogManager::log(LogManager::DISPATCH, obj);
	  found = true;
	  l--;
	}
      } //end loop over queue list
    }
  }//end loop over queue enumeration*/
  QTimer::singleShot(20,this, SLOT(CheckQueues()) );
}

void Dispatcher::CheckQueues(){
  qDebug() << " - Check Queues";
  for(int i=0; i<enum_length; i++){
    PROC_QUEUE queue = static_cast<PROC_QUEUE>(i);
    if(HASH.contains(queue)){
      QList<DProcess*> list = HASH[queue];
      for(int j=0; j<list.length(); j++){
	if(j>0 && queue!=NO_QUEUE){ break; } //done with this - only first item in these queues should run at a time
	if( !list[j]->isRunning() ){
	  if(list[j]->isDone() ){
	    qDebug() << "Delete Proc:" << list[j]->ID;
	    QJsonObject obj;
	    obj.insert("log",list[j]->getProcLog());
	    obj.insert("success", list[j]->success ? "true" : "false" );
	    obj.insert("proc_id", list[j]->ID);
	    obj.insert("cmd_list", QJsonArray::fromStringList( list[j]->rawcmds ) );
	    obj.insert("time_started", list[j]->t_started.toString(Qt::ISODate) );
	    obj.insert("time_finished", list[j]->t_finished.toString(Qt::ISODate) );
	    emit DispatchFinished(obj);
	    LogManager::log(LogManager::DISPATCH, obj);
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
