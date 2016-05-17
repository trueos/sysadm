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
QSettings *CONFIG = new QSettings("sysadm","bridge");
AuthorizationManager *AUTHSYSTEM = new AuthorizationManager();

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
void showUsage(){
qDebug() << "sysadm-bridge usage:";
qDebug() << "Starting the bridge:";
qDebug() << "    \"sysadm-bridge [-port <portnumber>]\"";
qDebug() << "CLI flags for configuring the bridge:";
qDebug() << "  \"-h\" or \"help\": Show this help text";
qDebug() << "  \"-import_ssl_file <nickname> <filepath>\": Loads a .crt or .key file and enables the public key for authorization access later";
qDebug() << "  \"-import_ssl_pubkey <nickname> <key>\": Enables the public key for authorization access later";
qDebug() << "  \"-list_ssl\":  Show all known SSL keys";
qDebug() << "  \"-remove_ssl <nickname>\": Removes a public key from allowing authorization access";
qDebug() << "  \"-set <variable>=<value>\": Used for adjusting individual settings for the bridge";
qDebug() << "  - Possible variables:";
qDebug() << "    \"blacklist/block_minutes\" (integer): Number of minutes a system remains on the automatic blacklist";
qDebug() << "    \"blacklist/fails_to_block\" (integer): Number of times a system must fail authentication to be placed on blacklist";
}

int main( int argc, char ** argv )
{
  qDebug() << "Using Config file:" << CONFIG->fileName();
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
      }
 // -------------------------
      else if( QString(argv[i])=="-h" || QString(argv[i]).contains("help") ){ showUsage(); return 0; }
 // -------------------------
      else if( (QString(argv[i])=="-port" || QString(argv[i])=="-p") && (i+1<argc)){ i++; port = QString(argv[i]).toUInt(); }
 // -------------------------
      else if( QString(argv[i])=="-set" && i+1<argc){ settingchange = true; }
 // -------------------------
      else if( QString(argv[i])=="-import_ssl_file" && i+2<argc){
        i++; QString id = QString(argv[i]);
	i++; QFile file(argv[i]);
 	if(!file.open(QIODevice::ReadOnly)){ qDebug() << "Could not open file:" << file.fileName(); }
        else{
          QString enc_key;
          if(file.fileName().endsWith(".crt")){ QSslCertificate cert(&file); enc_key = QString(cert.publicKey().toPem().toBase64()); }
          else if(file.fileName().endsWith(".key")){ QSslKey key(&file); enc_key = QString(key.toPem().toBase64());  }
          if(enc_key.isEmpty()){ qDebug() << "Could not read key (need .crt or .key file)"; }
          else{
            qDebug() << "Registered Key:" << id << enc_key << "(base64)";
            QStringList dupkeys = CONFIG->allKeys().filter("RegisteredCerts/"+id+"/");
            for(int i=0; i<dupkeys.length(); i++){ CONFIG->remove(dupkeys[i]); }
            CONFIG->setValue("RegisteredCerts/"+id+"/"+enc_key, "Date Registered: "+QDateTime::currentDateTime().toString(Qt::ISODate) );
          }
	}
        settingchange=true;
 // -------------------------
      }else if( QString(argv[i])=="-import_ssl_pubkey" && i+2<argc){
        i++;  QString id = QString(argv[i]);
        i++;  QString enc_key = QByteArray(argv[i]).toBase64();
        qDebug() << "Registered Key:" << id << enc_key << "(base64)";
        QStringList dupkeys = CONFIG->allKeys().filter("RegisteredCerts/"+id+"/");
        for(int i=0; i<dupkeys.length(); i++){ CONFIG->remove(dupkeys[i]); }
        CONFIG->setValue("RegisteredCerts/"+id+"/"+enc_key, "Date Registered: "+QDateTime::currentDateTime().toString(Qt::ISODate) );
        settingchange=true;
 // -------------------------
      }else if( QString(argv[i])=="-list_ssl" ){
        qDebug() << "Known SSL Keys (base64)";
        settingchange = true;
        QStringList keys = QStringList(CONFIG->allKeys());//.filter("RegisteredCerts/");
        for(int i=0; i<keys.length(); i++){
          qDebug() << keys[i].section("/",1,1) << keys[i].section("/",2,-1) << CONFIG->value(keys[i]).toString();
        }
 // -------------------------
      }else if( QString(argv[i])=="-remove_ssl" && i+1<argc){
        i++; QString id = QString(argv[i]);
        settingchange = true;
        QStringList dupkeys = CONFIG->allKeys().filter("RegisteredCerts/"+id+"/");
        for(int i=0; i<dupkeys.length(); i++){ qDebug() << "Removed Key:" << dupkeys[i].section("/",2,-1); CONFIG->remove(dupkeys[i]); }
 // -------------------------
      }else{
        qDebug() << "Unknown Option:" << argv[i];
      }
 // -------------------------
    } //end loop over argc
    if(settingchange){ CONFIG->sync(); return 0; }

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
