#include "portlookup.h"
#include <QtCore>
using namespace sysadm;
PortLookUp::PortInfo PortLookUp::LookUpPort(int portNumber)
{
    //Make sure that the port is valid
    if (portNumber < 0 || portNumber > 65535)
    {
        PortInfo returnValue;
        returnValue.Port = -1;
        returnValue.Description = "Port out of bounds";
        return returnValue;
    }

    PortInfo returnValue;
    //the port number is valid so set it
    returnValue.Port = portNumber;
    
    for(int i = 0; i < recommendedPortListSize; i++)
    {
    if (portNumber == recommendedPorts[i])
	{
	    returnValue.Recommended = true;
	}
    }
    
    
    // /etc/services contains a file that lists the various port numbers
    // and their descriptions
    QFile* services = new QFile("/etc/services");
    services->open(QFile::ReadOnly);
    while(!services->atEnd())
    {
           QString line = services->readLine();

           //jump down past the comments
           if(line[0] == '#')
               continue;

	   //remove all of the extraneous whitespace in the line
           line = line.simplified();
	   //Split across spaces since it's now whitespace delimited
           QStringList lineList = line.split(' ');
	   
	   //Ports are stored in the file as number/type so take the left hand side
           int port = lineList.at(1).split('/').at(0).toInt();

	   //obviously if it's not the port we want, go to the next line
           if (port != portNumber)
               continue;
           
	   //the keyword associated with the port is the first element in a line
           returnValue.Keyword = lineList.at(0);

	   //if the size of the list is less than 3 then there is no description
           if(lineList.size() > 2)
           {
               QString description = lineList.at(2);
	       //String the description back together from the end of the list
               for(int i = 3; i < lineList.size(); i++)
               {
                   description += " " + lineList.at(i);
               }
               returnValue.Description = description;
           }
    }
    services->close();
    
    
    return returnValue;

}
 
