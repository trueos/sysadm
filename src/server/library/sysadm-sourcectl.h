//===========================================
//  TrueOS source code
//  Copyright (c) 2017, JT (q5sys)
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_SOURCECTL_H
#define __PCBSD_LIB_UTILS_SOURCECTL_H

#include <QJsonObject>
#include "sysadm-global.h"

namespace sysadm{

class sourcectl{
public:

	static QJsonObject downloadports();
	static QJsonObject updateports();
	static QJsonObject deleteports();
	static QJsonObject stopports();
	static QJsonObject downloadsource();
	static QJsonObject updatesource();
	static QJsonObject deletesource();
	static QJsonObject stopsource();

	static void saveSourceLog(QString);
	static void savePortsLog(QString);

};

} //end of sysadm namespace

#endif
