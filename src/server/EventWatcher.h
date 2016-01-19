// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_EVENT_WATCHER_SYSTEM_H
#define _PCBSD_SYSADM_EVENT_WATCHER_SYSTEM_H

#include "globals-qt.h"

#define DISPATCHWORKING QString("/var/tmp/appcafe/dispatch-queue.working")
#define LPLOG QString("/var/log/lpreserver/lpreserver.log")
#define LPERRLOG QString("/var/log/lpreserver/error.log")
#define LPREPLOGDIR QString("/var/log/lpreserver/")

class EventWatcher : public QObject{
	Q_OBJECT
public:
	//Add more event types here as needed
	enum EVENT_TYPE{ BADEVENT, DISPATCHER, LIFEPRESERVER};
	
	EventWatcher();
	~EventWatcher();

	void start();
	
	//Convert a string into the type flag
	static EVENT_TYPE typeFromString(QString);
	
	//Retrieve the most recent event message for a particular type of event
	QJsonValue lastEvent(EVENT_TYPE type);
	
private:
	QFileSystemWatcher *watcher;
	QHash<unsigned int, QJsonValue> HASH; 
	//HASH Note: Fields 1-99 reserved for EVENT_TYPE enum (last message of that type)
	//	Fields 100-199 reserved for Life Preserver logs (all types)
	QString tmpLPRepFile;

	QString readFile(QString path);

public slots:
	
private slots:
	//File watcher signals
	void WatcherUpdate(QString);

signals:
	void NewEvent(EventWatcher::EVENT_TYPE, QJsonValue); //type/message
};
#endif
