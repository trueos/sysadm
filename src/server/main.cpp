// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#include "globals.h"

#include <unistd.h>
#include <sys/types.h>

#include "WebServer.h"

#define CONFFILE "/usr/local/etc/sysadm.conf"
#define SETTINGSFILE "/var/db/sysadm.ini"

#define DEBUG 0

//Create any global classes
QSettings *CONFIG = new QSettings(SETTINGSFILE, QSettings::IniFormat);
EventWatcher *EVENTS = new EventWatcher();
Dispatcher *DISPATCHER = new Dispatcher();
bool WS_MODE = false;

//Set the defail values for the global config variables
int BlackList_BlockMinutes = 60;
int BlackList_AuthFailsToBlock = 5;
int BlackList_AuthFailResetMinutes = 10;
bool BRIDGE_ONLY = false;

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

inline QString ReadFile(QString path){
  QFile file(path);
  if( !file.open(QIODevice::ReadOnly) ){ return ""; }
  QTextStream in(&file);
  QString str = in.readAll();
  file.close();
  return str;
}

int main( int argc, char ** argv )
{
    
    //Check whether running as root
    if( getuid() != 0){
      qDebug() << "sysadm-server must be started as root!";
      return 1;
    }

    //Evaluate input arguments
    bool websocket = true;
    bool setonly = false;
    quint16 port = 0;
    for(int i=1; i<argc; i++){
      if( QString(argv[i])=="-rest" ){ websocket = false;}
      else if( QString(argv[i])=="-p" && (i+1<argc) ){ i++; port = QString(argv[i]).toUInt(); }
      else if( QString(argv[i]).startsWith("bridge_") ){
        setonly = true;
        QString opt = QString(argv[i]).section("_",1,-1);
        if(opt=="list"){
          QStringList bridges = CONFIG->allKeys().filter("bridge_connections/");
          qDebug() << "Current Bridges:";
          for(int i=0; i<bridges.length(); i++){
            qDebug() << bridges[i].section("/",1,-1) + " ("+CONFIG->value(bridges[i]).toString()+")";
          }
        }else if(opt=="add" && argc > i+2){
          QString name = QString(argv[i+1]);
          QString url = QString(argv[i+2]);
          CONFIG->setValue("bridge_connections/"+name, url);
	  qDebug() << "New Bridge Added:" << name+" ("+url+")";
          i=i+2;
        }else if(opt=="remove" && argc>i+1){
          QString name = QString(argv[i+1]);
          CONFIG->remove("bridge_connections/"+name);
          qDebug() << "Bridge Removed:" << name;
          i=i+1;
        }else{
          qDebug() << "Unknown option:" << argv[i];
          return 1;
        }
      }else{
        qDebug() << "Unknown option:" << argv[1];
        return 1;
      }
    }
    if(setonly){ CONFIG->sync(); return 0; }
    WS_MODE = websocket; //set the global variable too

    QCoreApplication a(argc, argv);
    //Now load the config file
    QStringList conf = ReadFile(CONFFILE).split("\n");
    if(!conf.filter("[internal]").isEmpty()){
      //Older QSettings file - move it to the new location
      if(QFile::exists(SETTINGSFILE)){ QFile::remove(SETTINGSFILE); } //remove the new/empty settings file
      QFile::rename(CONFFILE, SETTINGSFILE);
      CONFIG->sync(); //re-sync settings structure
      conf.clear(); //No config yet
    }
    //Load the settings from the config file
    // - port number
    if(port==0){
      if(websocket){ 
	int index = conf.indexOf(QRegExp("PORT=*",Qt::CaseSensitive,QRegExp::Wildcard));
	bool ok = false;
	if(index>=0){ port = conf[index].section("=",1,1).toInt(&ok); }
	if(port<=0 || !ok){ port = WSPORTNUMBER;  }
      }else{
	int index = conf.indexOf(QRegExp("PORT_REST=*",Qt::CaseSensitive,QRegExp::Wildcard));
	bool ok = false;
	if(index>=0){ port = conf[index].section("=",1,1).toInt(&ok); }
	if(port<=0 || !ok){ port = PORTNUMBER;  }	      
      }
    }
    // - Blacklist options
    QRegExp rg = QRegExp("BLACKLIST_BLOCK_MINUTES=*",Qt::CaseSensitive,QRegExp::Wildcard);
    if(!conf.filter(rg).isEmpty()){
      bool ok = false;
      int tmp = conf.filter(rg).first().section("=",1,1).simplified().toInt(&ok);
      if(ok){ BlackList_BlockMinutes = tmp; }
    }
    rg = QRegExp("BLACKLIST_AUTH_FAIL_LIMIT=*",Qt::CaseSensitive,QRegExp::Wildcard);
    if(!conf.filter(rg).isEmpty()){
      bool ok = false;
      int tmp = conf.filter(rg).first().section("=",1,1).simplified().toInt(&ok);
      if(ok){ BlackList_AuthFailsToBlock = tmp; }
    }
    rg = QRegExp("BLACKLIST_AUTH_FAIL_RESET_MINUTES=*",Qt::CaseSensitive,QRegExp::Wildcard);
    if(!conf.filter(rg).isEmpty()){
      bool ok = false;
      int tmp = conf.filter(rg).first().section("=",1,1).simplified().toInt(&ok);
      if(ok){ BlackList_AuthFailResetMinutes = tmp; }
    }
    rg = QRegExp("BRIDGE_CONNECTIONS_ONLY=*",Qt::CaseSensitive,QRegExp::Wildcard);
    if(!conf.filter(rg).isEmpty()){
      BRIDGE_ONLY = conf.filter(rg).first().section("=",1,1).simplified().toLower()=="true";
    }    

    //Setup the log file
    LogManager::checkLogDir(); //ensure the logging directory exists
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
    QObject::connect(DISPATCHER, SIGNAL(DispatchEvent(QJsonObject)), EVENTS, SLOT(DispatchEvent(QJsonObject)) );
    QObject::connect(DISPATCHER, SIGNAL(DispatchStarting(QString)), EVENTS, SLOT(DispatchStarting(QString)) );
      
    //Create the daemon
    qDebug() << "Starting the PC-BSD sysadm server...." << (websocket ? "(WebSocket)" : "(TCP)");
    WebServer *w = new WebServer(); 
    //Start the daemon
    int ret = 1; //error return value
    if( w->startServer(port, websocket) ){
      qDebug() << " - Configuration File:" << CONFIG->fileName();
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
