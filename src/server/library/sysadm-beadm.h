//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_BEADM_H
#define __PCBSD_LIB_UTILS_BEADM_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class BEADM{
public:
	static QJsonObject listBEs();
	static QJsonObject renameBE(QJsonObject);
};
	
} //end of pcbsd namespace

#endif
