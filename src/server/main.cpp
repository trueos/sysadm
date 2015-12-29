// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <unistd.h>
#include <sys/types.h>

#include "WebServer.h"

#ifndef PREFIX
#define PREFIX QString("/usr/local/")
#endif

#define DEBUG 1
#define WSPORTNUMBER 12142 	// WebSocket server default port
#define PORTNUMBER 12142		// TCP server default port

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
    QCoreApplication a(argc, argv);
    //Check whether running as root
    if( getuid() != 0){
      qDebug() << "sysadm-server must be started as root!";
      return 1;
    }
    //Evaluate input arguments
    bool websocket = false;
    quint16 port = 0;
    for(int i=1; i<argc; i++){
      if( QString(argv[i])=="-ws" ){ websocket = true; }
      else if( QString(argv[i])=="-p" && (i+1<argc) ){ i++; port = QString(argv[i]).toUInt(); }
    }
    if(port==0){
      if(websocket){ port = WSPORTNUMBER; }
      else{ port = PORTNUMBER; }
    }
    //Setup the log file
    if(!websocket){ logfile.setFileName("/var/log/sysadm-server-tcp.log"); }
    else{ logfile.setFileName("/var/log/sysadm-server-ws.log"); }
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
      
    //Create and start the daemon
    qDebug() << "Starting the PC-BSD sysadm server...." << (websocket ? "(WebSocket)" : "(TCP)");
    WebServer *w = new WebServer(); 
    if( w->startServer(port, websocket) ){
      //Now start the event loop
      int ret = a.exec();
      qDebug() << "Server Stopped:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      logfile.close();
      return ret;
    }else{
      qDebug() << "[FATAL] Server could not be started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      qDebug() << " - Tried port:" << port;
      logfile.close();
      return 1;
    }
}
