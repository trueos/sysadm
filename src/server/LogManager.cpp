// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2016
// =================================
#include "LogManager.h"
#include "globals.h"

#define TMPBREAK "<!!line-break!!>"
//Overall check/creation of the log directory
void LogManager::checkLogDir(){
  //Determing the log dir based on type of server
  QString logd = LOGDIR; //base log dir
    if(WS_MODE){ logd.append("/websocket"); }
    else{ logd.append("/restserver"); }
  //Check/create the dir
  if(!QFile::exists(logd)){
    QDir dir(logd);
    dir.mkpath(logd);
  }
  int daysold = CONFIG->value("prune_log_days_old",90); //90 days by default
  if(daysold>0){
    LogManager::pruneLogs(QDate::currentDate().addDays(-daysold));
  }
}

//Manual prune of logs older than designated date
void LogManager::pruneLogs(QDate olderthan){
  QString logd = LOGDIR; //base log dir
    if(WS_MODE){ logd.append("/websocket"); }
    else{ logd.append("/restserver"); }
  QDir dir(logd);
  QStringList files = dir.entryList(QStringList() << "*.log", QDir::Files, QDir::Name);
  //qDebug() << " - Got files:" << files << "Filter:" << tmp;
  for(int i=0; i<files.length(); i++){
   QDate fdate = QDate::fromString( files[i].section(".log",0,0).section("-",-3,-1), Qt::ISODate);
    //qDebug() << "Check File Date:" << fdate << olderthan;
    if( fdate < olderthan && fdate.isValid()){ 
      dir.remove(files[i]);
    }
  }
}

//Main Log write function (all the overloaded versions end up calling this one)
void LogManager::log(QString file, QStringList msgs, QDateTime time){
  if(file.isEmpty()){ return; }
  else if(!file.startsWith("/")){
    //relative path - put it in the main log dir
    if(WS_MODE){ file.prepend(LOGDIR+"/websocket/"); }
    else{ file.prepend(LOGDIR+"/restserver/"); }
  }
  //qDebug() << "Log to File:" << file << msgs;
  if(file.isEmpty()){ return; }
  QFile LOG(file);
  //if(!LOG.exists()){ system( QString("touch "+file).toLocal8Bit() ); }
  if( !LOG.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text) ){ qDebug() << " - Could not write to log:" << file; return; } //error writing to file
  QTextStream stream(&LOG);
  for(int i=0; i<msgs.length(); i++){
    msgs[i].replace("\n",TMPBREAK);
    stream << QString("["+time.toString(Qt::ISODate)+"]"+msgs[i]+"\n");
    //qDebug() << "logged line:" << msgs[i];
  }
  LOG.close();
  //qDebug() << "Finished saving to log";
}

//Main Log read function (all the overloaded versions end up calling this one)
QStringList LogManager::readLog(QString file, QDateTime starttime, QDateTime endtime){
  //Read the whole file into memory as quickly as possible
  QStringList all;
  QFile LOG(file);
  if( !LOG.open(QIODevice::ReadOnly | QIODevice::Text) ){ return QStringList(); } //error opening file
  QTextStream str(&LOG);
  all = str.readAll().split("\n");
  LOG.close();
  //Now filter out the entries outside the requested range of timestamps
  // - start at the beginning and check starttime
  while( !all.isEmpty() ){
   if( starttime <= QDateTime::fromString( all.first().section("]",0,0).section("[",1,1), Qt::ISODate) ){
      break;
   }else{
      all.removeAt(0);
   }
  }
  // - now check the end and work backwards for endtime
  while( !all.isEmpty() ){
    if( endtime > QDateTime::fromString( all.first().section("]",0,0).section("[",1,1), Qt::ISODate) ){
      break;
   }else{
      all.removeAt(all.length()-1);
   }
  }
  // - now double check that none of the temporary line break replacements get through
  for(int i=0; i<all.length(); i++){
    if(all[i].contains(TMPBREAK)){ all[i].replace(TMPBREAK,"\n"); }
  }
  return all;
}

QStringList LogManager::readLog(LogManager::LOG_FILE file, QDateTime starttime, QDateTime endtime){
  //First get a list of all the various log files which encompass this time range
  //qDebug() << "Try to read log:" << flagToPath(file);
  QDir logdir(LOGDIR+ (WS_MODE ? "/websocket" : "/restserver") );
  //  - get list of all this type of log
  QStringList files = logdir.entryList(QStringList() << flagToPath(file).arg("*"), QDir::Files, QDir::Name);
  // - filter out the dates we need (earlier first)
  QString tmp = flagToPath(file).arg(starttime.date().toString(Qt::ISODate));
  //qDebug() << " - Got files:" << files << "Filter:" << tmp;
  while(!files.isEmpty()){
    //Note we can do basic compare due to identical filenames *except* for some numbers near the end
    if( QString::compare(files[0], tmp)<0 && files[0]!=tmp){ files.removeAt(0); }
    else{ break; }
  }
  //qDebug() << " - After filtering earlier:" << files;
  // - now filter out any later dates than we need
  if(endtime.date() < QDate::currentDate()){
    tmp = flagToPath(file).arg(endtime.date().toString(Qt::ISODate));	  
    //qDebug() << " - Next Filter:" << tmp;
    while(!files.isEmpty()){
      //Note we can do basic compare due to identical filenames *except* for some numbers near the end
      if( QString::compare(files.last(), tmp)>0 && files.last()!=tmp ){ files.removeAt(files.length()-1); }
      else{ break; }
    }
    //qDebug() << " - After filter:" << files;
  }
  
  //Now load each file in order (oldest->newest) and filter out the necessary logs
  QStringList logs;
  for(int i=0; i<files.length(); i++){
    logs << readLog(logdir.filePath(files[i]), starttime, endtime);
  }
  //qDebug() << "Read Logs:" << logs;
  return logs;
}
