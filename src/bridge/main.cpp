// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#include "globals.h"

#include <unistd.h>
#include <sys/types.h>

#include "BridgeServer.h"

#define DEBUG 0

//Create any global classes
QSettings *CONFIG = new QSettings(SETTINGSFILE, QSettings::IniFormat);


//Set the defail values for the global config variables
/*int BlackList_BlockMinutes = 60;
int BlackList_AuthFailsToBlock = 5;
int BlackList_AuthFailResetMinutes = 10;*/

//Create the default logfile
QFile logfile;
void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg){
  QString txt;
  switch(type){
  case QtDebugMsg:
  	  txt = msg;
  	  break;
  case QtWarningMsg:
  	  txt = QString("WARNING: %1").arg(msg);
          txt += "\n Context: "+QString(context.file)+" Line: "+QString(context.line)+" Function: "+QString(context.function);
  	  break;
  case QtCriticalMsg:
  	  txt = QString("CRITICAL: %1").arg(msg);
	  txt += "\n Context: "+QString(context.file)+" Line: "+QString(context.line)+" Function: "+QString(context.function);
  	  break;
  case QtFatalMsg:
  	  txt = QString("FATAL: %1").arg(msg);
	  txt += "\n Context: "+QString(context.file)+" Line: "+QString(context.line)+" Function: "+QString(context.function);
  	  break;
  }

  QTextStream out(&logfile);
  out << txt;
  if(!txt.endsWith("\n")){ out << "\n"; }
}

int main( int argc, char ** argv )
{
    //Evaluate input arguments
    quint16 cport = 12148; //client-side port number
    quint16 sport = 12149; //server-side port number
    bool settingchange = false;
    for(int i=1; i<argc; i++){
      if(settingchange){
        QString info = QString(argv[i]);
	if(!info.contains("=")){ continue; } //invalid format
	QString var = info.section("=",0,0); QString val = info.section("=",1,-1);
	qDebug() << "Changing bridge setting:" << info;
        if(var=="blacklist/blockmins"){ CONFIG->setValue("blacklist_settings/blockmins",val.toInt()); }
      }
      else if( QString(argv[i])=="-clientport" && (i+1<argc)){ i++; cport = QString(argv[i]).toUInt(); }
      else if( QString(argv[i])=="-serverport" && (i+1<argc) ){ i++; sport = QString(argv[i]).toUInt(); }
      else if( QString(argv[i])=="-set" && i+1<argc){ settingchange = true; }
    }
    if(settingchange){ return 0; }

     QCoreApplication a(argc, argv);

    //Setup the log file
    logfile.setFileName("/var/log/sysadm-bridge.log");
    if(DEBUG){ qDebug() << "Log File:" << logfile.fileName(); }
      if(QFile::exists(logfile.fileName()+".old")){ QFile::remove(logfile.fileName()+".old"); }
      if(logfile.exists()){ QFile::rename(logfile.fileName(), logfile.fileName()+".old"); }
      //Make sure the parent directory exists
      if(!QFile::exists("/var/log")){
        QDir dir;
        dir.mkpath("/var/log");
      }
      logfile.open(QIODevice::WriteOnly | QIODevice::Append);
      qInstallMessageHandler(MessageOutput);
      
    //Create the two servers and connect them
    qDebug() << "Starting the PC-BSD sysadm bridge....";
    BridgeServer cserver;  //client side
    BridgeServer sserver;  //server side
      QObject::connect(&cserver, SIGNAL(ForwardMessage(QString, QString)), &sserver, SLOT(SendMessage(QString, QString)) );
      QObject::connect(&sserver, SIGNAL(ForwardMessage(QString, QString)), &cserver, SLOT(SendMessage(QString, QString)) );

    //Start the servers
    int ret = 1; //error return value
    bool badstart = false;
    badstart = !cserver.startServer(cport);
    if(badstart){ qDebug() << "Could not start client-side server on port:" << cport; }
    else{ badstart = !sserver.startServer(sport); }
    if(badstart){ qDebug() << "Could not start server-side server on port:" << sport; }
    else{
      //Now start the main event loop
      qDebug() << "Bridge Started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      ret = a.exec();
      qDebug() << "Bridge Stopped:" << QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    if(cserver.isListening()){ cserver.close(); }
    if(sserver.isListening()){ sserver.close(); }

    //Cleanup any globals
    delete CONFIG;
    logfile.close();
    
    //Return
    return ret;
}
