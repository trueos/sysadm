// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "globals.h"

#include <unistd.h>
#include <sys/types.h>

#include "WebServer.h"

#define DEBUG 1

//Create any global classes
QSettings *CONFIG = new QSettings("PCBSD","sysadm");
EventWatcher *EVENTS = new EventWatcher();
Dispatcher *DISPATCHER = new Dispatcher();

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
    QCoreApplication a(argc, argv);
    //Check whether running as root
    if( getuid() != 0){
      qDebug() << "sysadm-server must be started as root!";
      return 1;
    }
    LogManager::checkLogDir(); //ensure the logging directry exists
    
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
      
    //Connect the background classes
    QObject::connect(DISPATCHER, SIGNAL(DispatchFinished(QJsonObject)), EVENTS, SLOT(DispatchFinished(QJsonObject)) );
    QObject::connect(DISPATCHER, SIGNAL(DispatchStarting(QString)), EVENTS, SLOT(DispatchStarting(QString)) );
      
    //Create the daemon
    qDebug() << "Starting the PC-BSD sysadm server...." << (websocket ? "(WebSocket)" : "(TCP)");
    WebServer *w = new WebServer(); 
    //Start the daemon
    int ret = 1; //error return value
    if( w->startServer(port, websocket) ){
      QThread TBACK, TBACK2;
      EVENTS->moveToThread(&TBACK);
      DISPATCHER->moveToThread(&TBACK2);
      TBACK.start();
      TBACK2.start();
      QTimer::singleShot(0,EVENTS, SLOT(start()) );
      //Now start the main event loop
      ret = a.exec();
      qDebug() << "Server Stopped:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      //TBACK.stop();
    }else{
      qDebug() << "[FATAL] Server could not be started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      qDebug() << " - Tried port:" << port;
    }
    //Cleanup any globals
    delete CONFIG;
    logfile.close();
    
    //Return
    return ret;
}
