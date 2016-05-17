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
#define USELOG 1 //turn this to "0" for dumping all output to the CLI (for debugging)

//Create any global classes/settings
QSettings *CONFIG = new QSettings(SETTINGSFILE, QSettings::IniFormat);
AuthorizationManager *AUTHSYSTEM = new AuthorizationManager();

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
    quint16 port = 12149; //port number
    bool settingchange = false;
    for(int i=1; i<argc; i++){
      if(settingchange){
        QString info = QString(argv[i]);
	if(!info.contains("=")){ continue; } //invalid format
	QString var = info.section("=",0,0); QString val = info.section("=",1,-1);
	qDebug() << "Changing bridge setting:" << info;
        if(var=="blacklist/block_minutes"){ CONFIG->setValue("blacklist_settings/block_minutes",val.toInt()); }
        else if(var=="blacklist/fails_to_block"){ CONFIG->setValue("blacklist_settings/fails_to_block",val.toInt()); }
        //else if(var=="blacklist_settings/block_minutes"){ CONFIG->setValue("blacklist_settings/block_minutes",val.toInt()); }
      }
      else if( (QString(argv[i])=="-port" || QString(argv[i])=="-p") && (i+1<argc)){ i++; port = QString(argv[i]).toUInt(); }
      else if( QString(argv[i])=="-set" && i+1<argc){ settingchange = true; }
      else if( QString(argv[i])=="-import_ssl_file" && i+1<argc){
        i++;
        QFile file(argv[i]);
 	if(!file.open(QIODevice::ReadOnly)){ qDebug() << "Could not open file:" << file.fileName(); }
        else{
          QString enc_key;
          if(file.fileName().endsWith(".crt")){ QSslCertificate cert(&file); enc_key = QString(cert.publicKey().toPem().toBase64()); }
          else if(file.fileName().endsWith(".key")){ QSslKey key(&file); enc_key = QString(key.toPem().toBase64());  }
          if(enc_key.isEmpty()){ qDebug() << "Could not read key (need .crt or .key file)"; }
          else{
            qDebug() << "Registered Key:" << enc_key << "(base64)";
            CONFIG->setValue("RegisteredCerts/cli-import/"+enc_key, "Date Registered: "+QDateTime::currentDateTime().toString(Qt::ISODate) );
          }
	}
        settingchange=true;
      }else if( QString(argv[i])=="-import_ssl_pubkey" && i+1<argc){
        i++;
        QString enc_key = QByteArray(argv[i]).toBase64();
        CONFIG->setValue("RegisteredCerts/cli-import/"+enc_key, "Date Registered: "+QDateTime::currentDateTime().toString(Qt::ISODate) );
        qDebug() << "Registered Key:" << enc_key << "(base64)";
        settingchange=true;
      }
    } //end loop over argc
    if(settingchange){ return 0; }

     QCoreApplication a(argc, argv);

    //Setup the log file
    if(USELOG){
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
    }
    //Create the server
    qDebug() << "Starting the PC-BSD sysadm bridge....";
    BridgeServer server;
     
    //Start the servers
    int ret = 1; //error return value
    if(!server.startServer(port)){ qDebug() << "Could not start bridge server on port:" << port; }
    else{
      //Now start the main event loop
      qDebug() << "Bridge Started:" << QDateTime::currentDateTime().toString(Qt::ISODate);
      ret = a.exec();
      qDebug() << "Bridge Stopped:" << QDateTime::currentDateTime().toString(Qt::ISODate);
    }

    //Cleanup any globals
    delete CONFIG;
    if(USELOG){ logfile.close(); }
    
    //Return
    return ret;
}
