//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_PKG_H
#define __PCBSD_LIB_UTILS_PKG_H

#include <QJsonObject>
#include "sysadm-global.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlRecord>
#include <QSqlQuery>
namespace sysadm{

class PKG{
public:
	static QJsonObject pkg_info(QStringList origins, QString repo);
};
	
} //end of sysadm namespace

#endif