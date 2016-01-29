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
	  
    //setup internal connections
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cmdFinished(int, QProcess::ExitStatus)) );
}

DProcess::~DProcess(){
  if(this->state()!=QProcess::NotRunning)){
    this->terminate();
  }
}
  
void DProcess::startProc(){
  if(cmds.isEmpty()){ 
    finished = QDateTime::currentDateTime();
    emit ProcFinished(ID); 
    return; 
  }
  QString cmd = cmds.takeFirst();
  success = false; //not finished yet
  if(!proclog.isEmpty()){ proclog.append("\n"); }
  else{ started = QDateTime::currentDateTime(); } //first cmd started
  proclog.append("[Running Command: "+cmd+" ]");
  this->start(cmd);
}

bool DProcess::isRunning(){
  return (this->state()!=QProcess::NotRunning);	
}

QString DProcess::getProcLog(){
  //First update the internal log as needed
  proclog.append( this->readAllStandardOutput() );
  //Now return the current version of the log
  return proclog;
}

void DProcess::cmdFinished(int ret, QProcess::ExitStatus status){
  //determin success/failure
  success = (status==QProcess::NormalExit && ret==0);
  //update the log before starting another command
  proclog.append( this->readAllStandardOutput() );
  //Now run any additional commands
  if(success){ startProc(); }//will emit the finished signal as needed if no more commands
  else{
    if(status==QProcess::NormalExit){
      proclog.append("\n[Command Failed: " + QString::number(ret)+" ]");
    }else{
      proclog.append("\n[Command Failed: Process Crashed ]");
    }
    finished = QDateTime::currentDateTime();
    emit ProcFinished(ID);
  }
}

// ================================
// Dispatcher Class
// ================================
Dispatcher::Dispatcher(){
	
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
void Dispatcher::queueProcess(QString ID, QString cmd){
  queueProcess(NO_QUEUE, ID, QStringList() << cmd);
}
void Dispatcher::queueProcess(QString ID, QStringList cmds){
  queueProcess(NO_QUEUE, ID, cmds);	
}
void Dispatcher::queueProcess(Dispatcher::PROC_QUEUE queue, QString ID, QString cmd){
  queueProcess(queue, ID, QStringList() << cmd);
}
void Dispatcher::queueProcess(Dispatcher::PROC_QUEUE queue, QString ID, QStringList cmds){
  //This is the primary queueProcess() function - all the overloads end up here to do the actual work
  DProcess *P = createProcess(ID, cmds);
  QList<DProcess*> list;
  if(!HASH.contains(queue)){ HASH.insert(queue, list); } //insert an empty list
  HASH[queue] << P; //add this proc to the end of the list
  if(queue==NO_QUEUE || HASH[queue].length()==1){ P->startProc(); } //go ahead and start it now
  else{ CheckQueues(); }
}

// === PRIVATE ===
//Simplification routine for setting up a process
DProcess* Dispatcher:: createProcess(QString ID, QStringList cmds){
  DProcess* P = new DProcess(this);
    P->cmds = cmds;
    P->ID = ID;
    connect(P, SIGNAL(ProcFinished(QString)), this, SLOT(ProcFinished(QString)) );
  return P;
}

// === PRIVATE SLOTS ===
void Dispatcher::ProcFinished(QString ID){
  //Find the process with this ID and close it down (with proper events)
  for int i=0; i<enum_length; i++){
    PROC_QUEUE queue = static_cast(PROC_QUEUE>(i);
    if(HASH.contains(queue)){
      QList<DProcess*> list = HASH[queue];
      bool found = false;
      for(int l=0; l<list.length() && !found; l++){
        if(list[l].ID==ID){ 
	  QJsonObject obj;
	  obj.insert("log",list[l].procLog());
	  obj.insert("success", list[l].success ? "true" : "false" );
	  obj.insert("proc_id", ID);
	  obj.insert("cmd_list", QJsonArray::fromStringList( list[l].rawcmds );
	  obj.insert("time_started", list[l].started.toString(QT::ISODate) );
	  obj.insert("time_finished", list[l].finished.toString(QT::ISODate) );
	  delete list.takeAt(l);
	  LogManager::log(LogManager::EV_DISPATCH, obj);
	  found = true;
	}
      } //end loop over queue list
    }
  }//end loop over queue enumeration
  CheckQueues();
}

void Dispatcher::CheckQueues(){
	
}
