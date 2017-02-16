//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_IOCAGE_H
#define __PCBSD_LIB_UTILS_IOCAGE_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class Iocage{
public:
	//Activate/Deactivate a ZFS Pool
	static QJsonObject activateStatus(); //current activation status
	static QJsonObject activatePool(QJsonObject);
	static QJsonObject deactivatePool(QJsonObject);
	static QJsonObject cleanAll(); //delete all iocage data on the system

	//Template management
	static QJsonObject listTemplates();
	static QJsonObject listReleases();
	static QJsonObject listPlugins();
	static QJsonObject fetchReleases(QJsonObject);
	static QJsonObject fetchPlugins(QJsonObject);
	static QJsonObject cleanTemplates();
	static QJsonObject cleanReleases();

	//Jail management

	static QJsonObject execJail(QJsonObject);
	static QJsonObject df();
	static QJsonObject destroyJail(QJsonObject);
	static QJsonObject createJail(QJsonObject);
	static QJsonObject cloneJail(QJsonObject);


	static QJsonObject cleanJails();
	static QJsonObject capJail(QJsonObject);


	static QJsonObject stopJail(QJsonObject);
	static QJsonObject startJail(QJsonObject);
	//static QJsonObject getDefaultSettings();
	static QJsonObject getJailSettings(QJsonObject);
	static QJsonObject listJails();
};

} //end of namespace

#endif
