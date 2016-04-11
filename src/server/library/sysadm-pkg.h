//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_PKG_H
#define __PCBSD_LIB_UTILS_PKG_H

#include <QJsonObject>
#include "sysadm-global.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlQuery>

namespace sysadm{

class PKG{
public:
	//Information fetch routines
	static QJsonObject pkg_info(QStringList origins, QString repo, QString category = "", bool fullresults = true);
	static QStringList pkg_search(QString repo, QString searchterm, QStringList searchexcludes, QString category = "");
	static QJsonArray list_categories(QString repo);
	static QJsonArray list_repos();

	//pkg modification routines (dispatcher events for notifications)
	static QJsonObject pkg_install(QStringList origins, QString repo);
	static QJsonObject pkg_remove(QStringList origins, bool recursive = true);
	static QJsonObject pkg_lock(QStringList origins); 	//local/installed pkgs only
	static QJsonObject pkg_unlock(QStringList origins); 	//local/installed pkgs only

	//pkg administration routines
	static QJsonObject pkg_update(bool force = false); 	//update databases
	static QJsonObject pkg_check_upgrade();			//Check for updates to pkgs
	static QJsonObject pkg_upgrade(); 				//upgrade all pkgs (use sysadm/updates if possible instead)
	static QJsonObject pkg_audit();					//List details of vulnerable packages
	
};
	
} //end of sysadm namespace

#endif