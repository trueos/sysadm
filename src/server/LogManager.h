// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2016
// =================================
//This is just a collection of static functions for reading/writing to various log files
// =================================
#ifndef _PCBSD_SYSADM_LOG_MANAGER_CLASS_H
#define _PCBSD_SYSADM_LOG_MANAGER_CLASS_H

#include "globals-qt.h"

// ==========================================
//     SUMMARY OF LOG FILES AND FORMATTING
//===========================================
//  Event Files (EV_*): JSON input/output (full event)
//  HOST: String input/output (simple messages)
//  DISPATCH: Full log of dispatcher processes output (JSON)
//===========================================
// LogFile Format: "[datetimestamp]<message>"
//===========================================
#define LOGDIR QString("/var/log/sysadm")


class LogManager{
public:
	//Enumeration of common log files (will automatically use proper file)
	// === ADD NEW FILE SUPPORT  HERE ===
	enum LOG_FILE {HOST = 0, DISPATCH, EV_DISPATCH, EV_LP, EV_STATE};
	//Conversion function for flag->path
	static QString flagToPath(LogManager::LOG_FILE flag){
	  QString filepath;
	  if(flag==HOST){ filepath.append("hostinfo"); }
	  else if(flag==DISPATCH){ filepath.append("dispatcher"); }
	  else if(flag==EV_DISPATCH){ filepath.append("events-dispatcher"); }
	  else if(flag==EV_LP){ filepath.append("events-lifepreserver"); }
	  else if(flag==EV_STATE){ filepath.append("events-state"); }
	  else{ return ""; } //invalid file given
	  //Now add the datestamp to the filename - so logs are saved/pruned daily
	  filepath.append("-%1.log"); 
	  return filepath;
	}
	
	//Overall check/create/prune of the log directory (run this occasionally - such as every 24-48 hours)
	static void checkLogDir();
	//Manual prune of logs older than designated date
	static void pruneLogs(QDate olderthan);
	
	// === LOG TO FILE FUNCTIONS ===
	//The normal log routines (Few versions)
	// These functions will properly format the file so it can be read out and sorted later

	//Log a list of strings (same timestamp for all lines)
	static void log(QString file, QStringList msgs, QDateTime time = QDateTime::currentDateTime());
	static void log(LogManager::LOG_FILE file, QStringList msgs, QDateTime time = QDateTime::currentDateTime()){
	  log(flagToPath(file).arg(time.date().toString(Qt::ISODate)), msgs, time);
	}
	
	//Log a simple text string
	static void log(QString file, QString msg, QDateTime time = QDateTime::currentDateTime()){
	  log(file, QStringList() << msg, time);
	}
	static void log(LogManager::LOG_FILE file, QString msg, QDateTime time = QDateTime::currentDateTime()){
	  log(flagToPath(file).arg(time.date().toString(Qt::ISODate)), QStringList() << msg, time);
	}
	
	//Log a JSON Object
	static void log(QString file, QJsonObject obj, QDateTime time = QDateTime::currentDateTime()){
	  log(file, QStringList() << QJsonDocument(obj).toJson(QJsonDocument::Compact), time);
	}
	static void log(LogManager::LOG_FILE file, QJsonObject obj, QDateTime time = QDateTime::currentDateTime()){
	  log(flagToPath(file).arg(time.date().toString(Qt::ISODate)), QStringList() << QJsonDocument(obj).toJson(QJsonDocument::Compact), time);
	}
	
	//Log a JSON Array
	static void log(QString file, QJsonArray array, QDateTime time = QDateTime::currentDateTime()){
	  log(file, QStringList() << QJsonDocument(array).toJson(QJsonDocument::Compact), time);
	}
	static void log(LogManager::LOG_FILE file, QJsonArray array, QDateTime time = QDateTime::currentDateTime()){
	  log(flagToPath(file).arg(time.date().toString(Qt::ISODate)), QStringList() << QJsonDocument(array).toJson(QJsonDocument::Compact), time);
	}	
	
	// === READ FROM LOG FUNCTIONS ===
	static QStringList readLog(QString file, QDateTime starttime, QDateTime endtime=QDateTime::currentDateTime());
	static QStringList readLog(LogManager::LOG_FILE file, QDateTime starttime, QDateTime endtime=QDateTime::currentDateTime());
};

#endif
