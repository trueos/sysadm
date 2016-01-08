// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
// This class is a simple subclass of QTcpServer to use SSL connections instead
// =================================
#ifndef _PCBSD_REST_WEB_SSL_SERVER_H
#define _PCBSD_REST_WEB_SSL_SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QTcpSocket>
#include <QQueue>
#include <QSslConfiguration>
#include <QDebug>

class SslServer : public QTcpServer{
	Q_OBJECT
private: 
	QQueue<QSslSocket*> pendingConnections;

public:
	SslServer(QObject *parent=0) : QTcpServer(parent){}
	~SslServer(){}
	
	bool hasPendingConnections() const{
	  return !pendingConnections.isEmpty();
	}
	
	QSslSocket* nextPendingConnection(){
	  if( pendingConnections.isEmpty() ){ return 0; }
	  else{ return pendingConnections.dequeue(); }
	}
protected:
	void incomingConnection(qintptr socketDescriptor){
	  QSslSocket *serverSocket = new QSslSocket(this);
	  qDebug() << "New Ssl Connection:";
	  //setup any supported encruption types here
	  serverSocket->setSslConfiguration(QSslConfiguration::defaultConfiguration());
	  serverSocket->setProtocol(QSsl::SslV3); //no TLS support (all sorts of issues with that)
	  //serverSocket->setPrivateKey();
	  //serverSocket->setLocalCertificate();
	  qDebug() << " - Supported Protocols:" << serverSocket->sslConfiguration().protocol();

	  if (serverSocket->setSocketDescriptor(socketDescriptor)) {
	    pendingConnections.enqueue(serverSocket);
	    //connect(serverSocket, SIGNAL(encrypted()), this, SLOT(ready()));
	    qDebug() << " - Starting Server Encryption Handshake";
            serverSocket->startServerEncryption();
	  } else {
            delete serverSocket;
	  }		
	}
	

};
#endif
