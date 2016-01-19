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
#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>

#include <QWebSocketServer>
#include <QWebSocketCorsAuthenticator>
#include <QWebSocket>
#include <QTcpServer>
#include <QSslSocket>

#include <QFileSystemWatcher>

#include <QList>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QtDebug>

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
