// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_WEB_SOCKET_H
#define _PCBSD_REST_WEB_SOCKET_H

#include <QWebSocket>
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
	~WebSocket();

	QString ID();
	void setLastDispatch(QString); //used on initialization only

private:
	QTimer *idletimer;
	QWebSocket *SOCKET;
	QString SockID, SockAuthToken, lastDispatchEvent;
	AuthorizationManager *AUTHSYSTEM;
	bool SendAppCafeEvents;

	//Main connection comminucations procedure
	void EvaluateREST(QString); //Text -> Rest/JSON struct
	void EvaluateRequest(const RestInputStruct&); // Parse Rest/JSON (does auth/events)

	//Simplification functions
	QString JsonValueToString(QJsonValue);
	QStringList JsonArrayToStringList(QJsonArray);
	void SetOutputError(QJsonObject *ret, QString id, int err, QString msg);

	//Backend request/reply functions (contained in WebBackend.cpp)
	void EvaluateBackendRequest(QString name, const QJsonValue in_args, QJsonObject *out);

private slots:
	void checkIdle(); //see if the currently-connected client is idle
	void SocketClosing();

	//Currently connected socket signal/slot connections
	void EvaluateMessage(const QByteArray&); 
	void EvaluateMessage(const QString&);

public slots:
	void AppCafeStatusUpdate(QString msg = "");

signals:
	void SocketClosed(QString); //ID

};

#endif
