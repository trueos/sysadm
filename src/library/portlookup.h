//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details

#ifndef PORTLOOKUP_H
#define PORTLOOKUP_H
#include <QString>
namespace sysadm
{
namespace PortLookUp
{
    static int recommendedPorts[] = {5,22};
    const int recommendedPortListSize = 2;
    
    struct PortInfo{
        int Port;
        QString Keyword;
        QString Description;
        bool Recommended;
    };

    /**
     * @description Returns a structure containing information about the port
     * including its port type, keyword, description, and whether it's a
     * recommended port
     *
     * @parameter portNumber a port number between 1 and 2^16
     *
     * @ErrorConditions Port Number is set to -1 and a description of the error is stored in the description variable
     */
    static PortInfo LookUpPort(int portNumber);
}
}
#endif // PORTLOOKUP_H
 
