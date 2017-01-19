//===========================================
//  TrueOS source code
//  Copyright (c) 2017, TrueOS Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __TRUEOS_SYSADM_UTILS_MOUSED_H
#define __TRUEOS_SYSADM_UTILS_MOUSED_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class moused{
public:
	static QJsonObject listDevices();					//List all individually-configurable devices

	//Service options for individual devices
	static QJsonObject listOptions(); 					//list all the options and possible values
	static QJsonObject readOptions(QJsonObject);	//current values for device
	static QJsonObject setOptions(QJsonObject); 	//change values for device

	//Service status for devices (enable/disable device)
	static QJsonObject listActiveDevices();
	static QJsonObject enableDevice(QJsonObject);
	static QJsonObject disableDevice(QJsonObject);

};
	
} //end of namespace

#endif
