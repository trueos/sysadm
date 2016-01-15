// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_EVENT_WATCHER_SYSTEM_H
#define _PCBSD_SYSADM_EVENT_WATCHER_SYSTEM_H

#include "globals-qt.h"

#define DISPATCHWORKING QString("/var/tmp/appcafe/dispatch-queue.working")

class EventWatcher : public QObject{
	Q_OBJECT
public:
	//Add more event types here as needed
	enum EVENT_TYPE{ DISPATCHER };
	
	EventWatcher();
	~EventWatcher();

	void start();
	
	QString lastEvent(EVENT_TYPE type);
	
private:
	QFileSystemWatcher *watcher;
	QHash<EVENT_TYPE, QString> HASH;

	QString readFile(QString path);

public slots:
	
private slots:
	//File watcher signals
	void WatcherUpdate(QString);

signals:
	void NewEvent(EVENT_TYPE ev, QString msg); //type/message
};
#endif
