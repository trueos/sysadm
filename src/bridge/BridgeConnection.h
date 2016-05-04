// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#ifndef _PCBSD_SYSADM_BRIDGE_SOCKET_H
#define _PCBSD_SYSADM_BRIDGE_SOCKET_H

#include "globals.h"

class BridgeConnection : public QObject{
	Q_OBJECT
public:
	BridgeConnection(QWebSocket*, QString ID);
	~BridgeConnection();

	QString ID();
	void forwardMessage(QString msg);
	bool isServer();

private:
	QTimer *idletimer;
	QWebSocket *SOCKET;
	QString SockID, SockAuthToken, SockPeerIP;
	bool serverconn;

	//Simplification functions
	QString JsonValueToString(QJsonValue);
	QStringList JsonArrayToStringList(QJsonArray);

	void InjectMessage(QString msg);
	void HandleAPIMessage(QString msg);

private slots:
	void checkIdle(); //see if the currently-connected client is idle
	void checkAuth(); //see if the currently-connected client has authed yet
	void SocketClosing();

	//Currently connected socket signal/slot connections
	void EvaluateMessage(const QByteArray&); 
	void EvaluateMessage(const QString&);

	//SSL signal handling
	void nowEncrypted(); //the socket/connection is now encrypted
	void peerError(const QSslError&); //peerVerifyError() signal
	void SslError(const QList<QSslError>&); //sslErrors() signal
	
public slots:

signals:
	void SocketClosed(QString); //ID
	void SocketMessage(QString, QString);
};

#endif
