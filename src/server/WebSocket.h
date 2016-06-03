// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_WEB_SOCKET_H
#define _PCBSD_REST_WEB_SOCKET_H

#include "globals.h"

#include "RestStructs.h"
#include "AuthorizationManager.h"

struct bridge_data{
  QByteArray enc_key;
  QString auth_tok;
  QList<EventWatcher::EVENT_TYPE> sendEvents;
};

class WebSocket : public QObject{
	Q_OBJECT
public:
	WebSocket(QObject *parent, QWebSocket*, QString ID, AuthorizationManager *auth);
	WebSocket(QObject *parent, QSslSocket*, QString ID, AuthorizationManager *auth);
	WebSocket(QObject *parent, QString url, QString ID, AuthorizationManager *auth); //sets up a bridge connection (websocket only)
	~WebSocket();

	QString ID();
	void closeConnection();
	bool isActive(); //check if the connection is still active/valid

private:
	QTimer *idletimer, *connCheckTimer;
	QWebSocket *SOCKET;
	QSslSocket *TSOCKET;
	QString SockID, SockAuthToken, SockPeerIP;
	AuthorizationManager *AUTHSYSTEM;
	QList<EventWatcher::EVENT_TYPE> ForwardEvents;
	bool connecting; //flag for whether the connection is still being established

	//Data handling for bridged connections (1 connection for multiple clients)
	QHash<QString, bridge_data> BRIDGE; //ID/data
	bool isBridge;

	// Where we store incoming Tcp data
	QString incomingbuffer;
	void ParseIncoming();

	//Main connection communications procedure
	void EvaluateREST(QString); //STAGE 1 response: Text -> Rest/JSON struct
	void EvaluateRequest(const RestInputStruct&); //STAGE 2 response: Parse Rest/JSON (does auth/events)
	//Response handling 
	void EvaluateResponse(const RestInputStruct&);

	//Simplification functions
	QString JsonValueToString(QJsonValue);
	QStringList JsonArrayToStringList(QJsonArray);

	//Backend request/reply functions (contained in WebBackend.cpp)
	// -- Subsystem listing routine
	RestOutputStruct::ExitCode AvailableSubsystems(bool fullaccess, QJsonObject *out);
	// -- Main subsystem parser
	RestOutputStruct::ExitCode EvaluateBackendRequest(const RestInputStruct&, QJsonObject *out);


	// -- Individual subsystems
	// -- Server Settings Modification API
	RestOutputStruct::ExitCode EvaluateSysadmSettingsRequest(const QJsonValue in_args, QJsonObject *out);
	// -- Server Log retrieval system
	RestOutputStruct::ExitCode EvaluateSysadmLogsRequest(bool allaccess, const QJsonValue in_args, QJsonObject *out);
	// -- rpc dispatcher API
	RestOutputStruct::ExitCode EvaluateDispatcherRequest(bool allaccess, const QJsonValue in_args, QJsonObject *out);
	// -- sysadm beadm API
	RestOutputStruct::ExitCode EvaluateSysadmBEADMRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm FS API
	RestOutputStruct::ExitCode EvaluateSysadmFSRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm iocage API
	RestOutputStruct::ExitCode EvaluateSysadmIocageRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm iohyve API
	RestOutputStruct::ExitCode EvaluateSysadmIohyveRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm Network API
	RestOutputStruct::ExitCode EvaluateSysadmNetworkRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm LifePreserver API
	RestOutputStruct::ExitCode EvaluateSysadmLifePreserverRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm info API
	RestOutputStruct::ExitCode EvaluateSysadmSystemMgmtRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm Update API
	RestOutputStruct::ExitCode EvaluateSysadmUpdateRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm ZFS API
	RestOutputStruct::ExitCode EvaluateSysadmZfsRequest(const QJsonValue in_args, QJsonObject *out);
	// -- sysadm PKG API
	RestOutputStruct::ExitCode EvaluateSysadmPkgRequest(const QJsonValue in_args, QJsonObject *out);
	
private slots:
	void sendReply(QString msg);
	void checkConnection(); //see if the current connection is still open/valid
	void checkIdle(); //see if the currently-connected client is idle
	void checkAuth(); //see if the currently-connected client has authed yet
	void SocketClosing();

	//Currently connected socket signal/slot connections
	void EvaluateMessage(const QByteArray&); //initial message input (raw bytes - WebSocket)
	void EvaluateMessage(const QString&); //initial message input (text - WebSocket)
	void EvaluateTcpMessage(); //initial message input (text - REST)

	//SSL signal handling
	void nowEncrypted(); //the socket/connection is now encrypted
	void peerError(const QSslError&); //peerVerifyError() signal
	void SslError(const QList<QSslError>&); //sslErrors() signal

	// Library Slots
	void slotIohyveFetchDone(QString, int, QString);
	void slotIohyveFetchProcessOutput(QString);
	
	//Bridge Connection Handling
	void startBridgeAuth();

public slots:
	void EventUpdate(EventWatcher::EVENT_TYPE, QJsonValue = QJsonValue() );

signals:
	void SocketClosed(QString); //ID
	void SendMessage(QString); //Internal - connected to sendReply(QString)
};

#endif
