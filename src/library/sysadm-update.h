//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_UPDATER_H
#define __PCBSD_LIB_UTILS_UPDATER_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class Update{
public:
	static QJsonObject checkUpdates();
	static QJsonObject listBranches();
};
	
} //end of pcbsd namespace

#endif
