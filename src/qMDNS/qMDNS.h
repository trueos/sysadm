/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of qMDNS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

// Modified May 2017 By Ken Moore <ken@ixsystems.com> 
//  to provide caching/results of the network at any point in time
//  available under the 3-clause BSD license (or MIT license if preferred)

#include <QObject>
#include <QTimer>
#include <QHostInfo>
#include <QHash>
#include <QDateTime>

class QHostInfo;
class QUdpSocket;
class QHostAddress;

/**
 * \brief Implements a simple mDNS responder using Qt
 *
 * This implementation is able perform mDNS queries and mDNS responses in any
 * operating system supported by the Qt network module.
 *
 * You can obtain the IP of an mDNS device using the \c lookup() function,
 * the \c hostFound() signal will be emitted whenever this class interprets
 * a mDNS response packet and obtains valid information about a remote host.
 *
 * You can change the name that the local computer uses to identify itself
 * in the mDNS network using the \c setHostName() function.
 *
 * \todo Implement NSEC block code generation in the \c sendResponse() packet
 */
class qMDNS : public QObject {
    Q_OBJECT

  signals:
    void hostFound (const QHostInfo& info);

  public:
    static qMDNS* getInstance();

    QString hostName() const;
    QString getAddress (const QString& string);

	QList<QHostInfo> knownHosts(); // KM: return list of known/available hosts (those active within the last 15 minutes)

  protected:
    explicit qMDNS();
    ~qMDNS();

  public slots:
    void setTTL (const quint32 ttl);
    void lookup (const QString& name);
    void setHostName (const QString& name);

  private slots:
    void onReadyRead();
    void readQuery (const QByteArray& data);
    void sendPacket (const QByteArray& data);
    void readResponse (const QByteArray& data);
    void sendResponse (const quint16 query_id);
	void NewHostFound(QHostInfo info); // KM: internal slot for caching the new host information
	void cleanCache(); //KM: internal slot for cleaning out old cached info on a regular basis

  private:
    QString getHostNameFromResponse (const QByteArray& data);
    QString getIPv4FromResponse (const QByteArray& data, const QString& host);
    QStringList getIPv6FromResponse (const QByteArray& data, const QString& host);
    QList<QHostAddress> getAddressesFromResponse (const QByteArray& data,
                                                  const QString& host);

  private:
    quint32 m_ttl;
    QString m_hostName;
    QUdpSocket* m_IPv4Socket;
    QUdpSocket* m_IPv6Socket;
	QHash<QDateTime, QHostInfo> *knownHash; //KM: internal cache for the host addresses that were found
	QTimer *cacheCleanTimer; //KM: Timer for cleaning up old entries in the cache
};
