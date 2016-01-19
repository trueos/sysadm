// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_WEB_SERVER_H
#define _PCBSD_REST_WEB_SERVER_H

#include "globals.h"

#include "WebSocket.h"
#include "AuthorizationManager.h"
#include "SslServer.h"
class WebServer : public QObject{
	Q_OBJECT
public:
	WebServer();
	~WebServer();

	bool startServer(quint16 port, bool websocket);

public slots:
	void stopServer();

private:
	QWebSocketServer *WSServer;
	SslServer *TCPServer;
	QList<WebSocket*> OpenSockets;
	AuthorizationManager *AUTH;
	
	//Server Setup functions
	bool setupWebSocket(quint16 port);
	bool setupTcp(quint16 port);

	//Generic functions for either type of server
	QString generateID(); //generate a new ID for a socket

private slots:
        // Generic Server Slots
	void NewSocketConnection(); 					//newConnection() signal
	void NewConnectError(QAbstractSocket::SocketError);	//acceptError() signal

	// (WebSocket-only) Server signals/slots
	void ServerClosed(); 						//closed() signal
	void ServerError(QWebSocketProtocol::CloseCode);	//serverError() signal
	void ConnectError(QAbstractSocket::SocketError);
	//  - SSL/Authentication Signals
	void OriginAuthRequired(QWebSocketCorsAuthenticator*);	//originAuthenticationRequired() signal
	void PeerVerifyError(const QSslError&);			//peerVerifyError() signal
	void SslErrors(const QList<QSslError>&);	//sslErrors() signal
	void SocketClosed(QString ID);

signals:
	//void DispatchStatusUpdate(QString);

};

#endif