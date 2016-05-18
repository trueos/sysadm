// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#include <QCoreApplication>
#include <QWebSocketServer>
#include <QWebSocketCorsAuthenticator>
#include <QWebSocket>

#include <QSslKey>
#include <QSslCertificate>
#include <QSslError>
#include <QSslConfiguration>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QTimer>
#include <QDateTime>

#include <QFile>
#include <QDir>
#include <QHash>
#include <QObject>
#include <QUuid>

// SSL Version/File defines
#define SSLVERSION QSsl::TlsV1_0OrLater
#define SSLCERTFILE QString("bridgeserver.crt")
#define SSLKEYFILE QString("bridgeserver.key")

#define SETTINGSFILE "/var/db/sysadm-bridge.ini"

#define SYSADM_BRIDGE
#include "AuthorizationManager.h"

extern QSettings* CONFIG;
extern AuthorizationManager* AUTHSYSTEM;
extern QString SSLFILEDIR;
