//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_FILESYSTEM_H
#define __PCBSD_LIB_UTILS_FILESYSTEM_H

#include "sysadm-global.h"

namespace sysadm{

class FS{

public:
	static QJsonObject list_dir(QJsonObject jsin);

};

}//end of sysadm namespace

#endif
