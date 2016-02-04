//===========================================
//  PC-BSD source code
//  Copyright (c) 2016, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_IOHYVE_H
#define __PCBSD_LIB_UTILS_IOHYVE_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class Iohyve{
public:
	static QJsonObject fetchISO(QJsonObject);
	static QJsonObject listVMs();
};
	
} //end of pcbsd namespace

#endif
