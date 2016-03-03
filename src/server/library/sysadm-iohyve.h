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
#include "globals.h"

namespace sysadm{

class Iohyve{
public:
	static QJsonObject createGuest(QJsonObject);
	static QJsonObject deleteGuest(QJsonObject);
	static QJsonObject fetchISO(QJsonObject, DProcess *);
	static QJsonObject installGuest(QJsonObject);
	static QJsonObject isSetup();
	static QJsonObject listDisks(QJsonObject);
	static QJsonArray listISOs();
	static QJsonObject listVMs();
	static QJsonObject renameISO(QJsonObject);
	static QJsonObject rmISO(QJsonObject);
	static QJsonObject setupIohyve(QJsonObject);
	static QJsonObject startGuest(QJsonObject);
	static QJsonObject stopGuest(QJsonObject);

};
	
} //end of pcbsd namespace

#endif
