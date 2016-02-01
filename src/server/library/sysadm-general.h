//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#ifndef __PCBSD_LIB_UTILS_GENERAL_H
#define __PCBSD_LIB_UTILS_GENERAL_H

#include "sysadm-global.h"

namespace sysadm{

class General{
public:
    //Non-event-blocking versions of QProcess::execute() or system()
    //Note: environment changes should be listed as such: [<variable>=<value>]
    // - Both success/log of output
    static QString RunCommand(bool &success, QString command, QStringList arguments = QStringList(), QString workdir = "", QStringList env = QStringList() );
    // - Log output only
    static QString RunCommand(QString command, QStringList arguments = QStringList(), QString workdir = "", QStringList env = QStringList() );
    // - success output only
    static bool RunQuickCommand(QString command, QStringList arguments = QStringList(), QString workdir = "", QStringList env = QStringList() );

    //File Access Functions
    static QStringList readTextFile(QString filename);
    static bool writeTextFile(QString filename, QStringList contents, bool overwrite = true);

    /**
     * @brief getConfFileValue get the value associated with a key in a config file
     * @param fileName the file to read from
     * @param Key the key to look for
     * @param occur which occurance of the key to return, 1 = the first occurance
     * @return the value associated with the key
     */
    static QString getConfFileValue( QString fileName, QString Key, int occur =1);

    /**
     * @brief setConfFileValue set a value in a config file
     * @param fileName the file to write to
     * @param oldKey the key to look for
     * @param newKey the new key with it's value
     * @param occur which occurance in a file to write the key to, if set to -1 it removes all duplicates
     * @return success or failure to write the key
     */
    static bool setConfFileValue( QString fileName, QString oldKey, QString newKey, int occur = -1 );


    //Retrieve a text-based sysctl
    static QString sysctl(QString var);
    //Retrieve a number-based sysctl
    static long long sysctlAsInt(QString var);

};

} //end of pcbsd namespace

#endif
