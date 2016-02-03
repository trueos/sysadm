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
	static QJsonObject startJail(QJsonObject);
	static QJsonObject getDefaultSettings();
	static QJsonObject getJailSettings(QJsonObject);
	static QJsonObject listJails();
};

} //end of pcbsd namespace

#endif
