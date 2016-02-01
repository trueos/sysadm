#include "LogManager.h"
	
#define TMPBREAK "<!!line-break!!>"
//Overall check/creation of the log directory
void LogManager::checkLogDir(){
  if(!QFile::exists(LOGDIR)){
    QDir dir(LOGDIR);
    dir.mkpath(LOGDIR);
  }
}
	
//Main Log write function (all the overloaded versions end up calling this one)
void LogManager::log(QString file, QStringList msgs, QDateTime time){
  if(file.isEmpty()){ return; }
  QFile LOG(file);
  if( !LOG.open(QIODevice::WriteOnly, QIODevice::Append) ){ return; } //error writing to file
  QTextStream str(&LOG);
  for(int i=0; i<msgs.length(); i++){
    msgs[i].replace("\n",TMPBREAK);
    str << "["+time.toString(Qt::ISODate)+"]"+msgs[i]+"\n";
  }
  LOG.close();
}

//Main Log read function (all the overloaded versions end up calling this one)
QStringList LogManager::readLog(QString file, QDateTime starttime, QDateTime endtime){
  //Read the whole file into memory as quickly as possible
  QStringList all;
  QFile LOG(file);
  if( !LOG.open(QIODevice::ReadOnly) ){ return QStringList(); } //error opening file
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
  QDir logdir(LOGDIR);
  //  - get list of all this type of log
  QStringList files = logdir.entryList(QStringList() << flagToPath(file).arg("*"), QDir::Files, QDir::Name);
  // - filter out the dates we need (earlier first)
  QString tmp = flagToPath(file).arg(starttime.date().toString(Qt::ISODate));
  while(!files.isEmpty()){
    //Note we can do basic compare due to identical filenames *except* for some numbers near the end
    if( QString::compare(files[0], tmp)<0 ){ files.removeAt(0); }
    else{ break; }
  }
  // - now filter out any later dates than we need
  if(endtime.date() < QDate::currentDate()){
    tmp = flagToPath(file).arg(endtime.date().toString(Qt::ISODate));	  
    while(!files.isEmpty()){
      //Note we can do basic compare due to identical filenames *except* for some numbers near the end
      if( QString::compare(files.last(), tmp)>0 ){ files.removeAt(files.length()-1); }
      else{ break; }
    }
  }
  
  //Now load each file in order (oldest->newest) and filter out the necessary logs
  QStringList logs;
  for(int i=0; i<files.length(); i++){
    logs << readLog(LOGDIR+"/"+files[i], starttime, endtime);
  }
  return logs;
}