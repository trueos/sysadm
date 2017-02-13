//===========================================
//  TrueOS source code
//  Copyright (c) 2017, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __TRUEOS_SYSADM_UTILS_POWERD_H
#define __TRUEOS_SYSADM_UTILS_POWERD_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class powerd{
public:

	//Service options for individual devices
	static QJsonObject listOptions(); 					//list all the options and possible values
	static QJsonObject readOptions();	//current values for device
	static QJsonObject setOptions(QJsonObject); 	//change values for device

	//Service status for devices (enable/disable device)
	static QJsonObject listStatus();
	static QJsonObject startService();
	static QJsonObject stopService();
	static QJsonObject enableService();
	static QJsonObject disableService();

};
	
} //end of namespace

#endif
