// ===============================
//  PC-BSD SysAdm Bridge Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> May 2016
// =================================
#ifndef _PCBSD_SYSADM_BRIDGE_SERVER_H
#define _PCBSD_SYSADM_BRIDGE_SERVER_H

#include "globals.h"

#include "BridgeConnection.h"

class BridgeServer : public QWebSocketServer{
	Q_OBJECT
public:
	BridgeServer();
	~BridgeServer();

	bool startServer(quint16 port);

public slots:
	void sendMessage(QString toID, QString msg);

private:
	QList<BridgeConnection*> OpenSockets;
	
	//Server Setup functions
	bool setupWebSocket(quint16 port);
	
	//Server Blacklist / DDOS mitigator
	bool allowConnection(QHostAddress addr);

	//Generic functions for either type of server
	QString generateID(QString name); //generate a new ID for a socket

private slots:
        // Generic Server Slots
	void NewSocketConnection(); 					//newConnection() signal
	void NewConnectError(QAbstractSocket::SocketError);	//acceptError() signal
	//Socket Blacklist function
	void BlackListConnection(QHostAddress addr);

	//Server signals/slots
	void ServerClosed(); 						//closed() signal
	void ServerError(QWebSocketProtocol::CloseCode);	//serverError() signal
	void ConnectError(QAbstractSocket::SocketError);
	//  - SSL/Authentication Signals
	void OriginAuthRequired(QWebSocketCorsAuthenticator*);	//originAuthenticationRequired() signal
	void PeerVerifyError(const QSslError&);			//peerVerifyError() signal
	void SslErrors(const QList<QSslError>&);	//sslErrors() signal
	void SocketClosed(QString ID);

signals:
	void ForwardMessage(QString, QString); //toID, msg

};

#endif
