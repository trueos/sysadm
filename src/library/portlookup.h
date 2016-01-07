//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details

#ifndef PORTLOOKUP_H
#define PORTLOOKUP_H
#include <QtCore>
namespace sysadm
{
const static int recommendedPorts[] = {22, 80};
const static int recommendedPortsSize = 2;
class PortLookUp
{
    struct PortInfo{
        int Port;
        QString Keyword;
        QString Description;
        bool Recommended;
    };
public:
    /**
     * @description Returns a structure containing information about the port
     * including its port type, keyword, description, and whether it's a
     * recommended port
     *
     * @parameter portNumber a port number between 0 and 2^16 - 1
     *
     * @ErrorConditions Port Number is set to -1 and a description of the error is stored in the description variable
     */
    PortInfo LookUpPort(int portNumber, QString portType);
    PortLookUp();
    ~PortLookUp();

private:
    void readServicesFile();
    QStringList* portStrings;
};
}
#endif // PORTLOOKUP_H
 
