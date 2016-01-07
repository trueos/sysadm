// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_WEB_SOCKET_H
#define _PCBSD_REST_WEB_SOCKET_H

#include <QWebSocket>
#include <QSslSocket>
#include <QList>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>


#include "RestStructs.h"
#include "AuthorizationManager.h"

class WebSocket : public QObject{
	Q_OBJECT
public:
	WebSocket(QWebSocket*, QString ID, AuthorizationManager *auth);
	WebSocket(QSslSocket*, QString ID, AuthorizationManager *auth);
	~WebSocket();

	QString ID();
	void setLastDispatch(QString); //used on initialization only

private:
	QTimer *idletimer;
	QWebSocket *SOCKET;
	QSslSocket *TSOCKET;
	QString SockID, SockAuthToken, lastDispatchEvent;
	AuthorizationManager *AUTHSYSTEM;
	bool SendAppCafeEvents;

	void sendReply(QString msg);

	//Main connection comminucations procedure
	void EvaluateREST(QString); //Text -> Rest/JSON struct
	void EvaluateRequest(const RestInputStruct&); // Parse Rest/JSON (does auth/events)

	//Simplification functions
	QString JsonValueToString(QJsonValue);
	QStringList JsonArrayToStringList(QJsonArray);

	//Backend request/reply functions (contained in WebBackend.cpp)
	// -- Main subsystem parser
	RestOutputStruct::ExitCode EvaluateBackendRequest(QString namesp, QString name, const QJsonValue in_args, QJsonObject *out);
	// -- Individual subsystems
	RestOutputStruct::ExitCode EvaluateSyscacheRequest(const QJsonValue in_args, QJsonObject *out);
	RestOutputStruct::ExitCode EvaluateDispatcherRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm Network API
	RestOutputStruct::ExitCode EvaluateSysadmNetworkRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm LifePreserver API
	RestOutputStruct::ExitCode EvaluateSysadmLifePreserverRequest(const QJsonValue in_args, QJsonObject *out);
	
private slots:
	void checkIdle(); //see if the currently-connected client is idle
	void SocketClosing();

	//Currently connected socket signal/slot connections
	void EvaluateMessage(const QByteArray&); 
	void EvaluateMessage(const QString&);
	void EvaluateTcpMessage();

public slots:
	void AppCafeStatusUpdate(QString msg = "");

signals:
	void SocketClosed(QString); //ID

};

#endif
