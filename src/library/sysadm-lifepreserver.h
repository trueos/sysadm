//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_LIFEPRESERVER_H
#define __PCBSD_LIB_UTILS_LIFEPRESERVER_H

#include "sysadm-global.h"

namespace sysadm{

class LifePreserver{
public:
	// List schedule snapshots
	static QList<QStringList> listCron(); 
};
	
} //end of pcbsd namespace

#endif
