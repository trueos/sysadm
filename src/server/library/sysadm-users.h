//===========================================
//  TrueOS source code
//  Copyright (c) 2016, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __TRUEOS_LIB_UTILS_USERMANAGER_H
#define __TRUEOS_LIB_UTILS_USERMANAGER_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class UserManager{
public:
	//List all the users currently registered on the system
	static bool listUsers(QJsonObject* out, bool showall, QString user = "");

	//User Management
	static bool addUser(QJsonObject* out, QJsonObject inputs);
	static bool removeUser(QString username, bool deletehomedir = true);
	static bool modifyUser(QJsonObject* out, QJsonObject inputs);

	//List all the groups current registered on the system
	static bool listGroups(QJsonObject* out, QString user = "");

	//Group Management
	static bool addGroup(QJsonObject* out, QJsonObject input);
	static bool removeGroup(QString name);
	static bool modifyGroup(QJsonObject* out, QJsonObject input);

	//List all the devices currently available to be used for a PersonaCrypt User
	static QStringList getAvailablePersonaCryptDevices();

	//PersonaCrypt Modification functions
	static bool InitializePersonaCryptDevice(QString username, QString pass, QString device);
	static bool ImportPersonaCryptKey(QString keyfile);
};
	
} //end of sysadm namespace

#endif
