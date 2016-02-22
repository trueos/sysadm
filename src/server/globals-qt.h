// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_SERVER_QT_GLOBALS_H
#define _PCBSD_SYSADM_SERVER_QT_GLOBALS_H

//General-purpose includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QSettings>

#include <QCoreApplication>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QProcess>
#include <QProcessEnvironment>

#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>
#include <QSslConfiguration>

#include <QWebSocketServer>
#include <QWebSocketCorsAuthenticator>
#include <QWebSocket>
#include <QTcpServer>
#include <QSslSocket>
#include <QTcpSocket>

#include <QThread>
#include <QFileSystemWatcher>
#include <QQueue>
#include <QRegExp>

#include <QList>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QtDebug>

// SSL Version/File defines
#define SSLVERSION QSsl::TlsV1_0OrLater
#define SSLCERTFILE "/usr/local/etc/sysadm/restserver.crt"
#define SSLKEYFILE "/usr/local/etc/sysadm/restserver.key"

#define SSLCERTFILEWS "/usr/local/etc/sysadm/wsserver.crt"
#define SSLKEYFILEWS "/usr/local/etc/sysadm/wsserver.key"

// Server Settings defines
#define DISPATCH_QUEUE "/usr/local/etc/sysadm/dispatch_queue"

inline QString DisplayPriority(int pri){
  //ensure bounds
  if(pri<0){ pri = 0; }
  if(pri>10){ pri = 10; }
  //Return display string
  if(pri<3){ return (QString::number(pri)+" - Information"); }
  else if(pri<6){ return (QString::number(pri)+" - Warning"); }
  else if(pri<9){ return (QString::number(pri)+" - Critical"); }
  else{ return (QString::number(pri)+" - Urgent"); }
}

#endif
