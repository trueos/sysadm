//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_SYSINFO_H
#define __PCBSD_LIB_UTILS_SYSINFO_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class SysInfo{
public:
	static QJsonObject batteryInfo();
	static QJsonObject cpuPercentage();
	static QJsonObject cpuTemps();
	static QJsonObject externalDevicePaths();
	static QJsonObject memoryPercentage();
	static QJsonObject systemInfo();
};
	
} //end of pcbsd namespace

#endif
