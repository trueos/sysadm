#include "portlookup.h"
#include <QtCore>

using namespace sysadm;
PortLookUp::PortInfo PortLookUp::LookUpPort(int portNumber, QString portType)
{
    //Make sure that the port is valid
    if (portNumber < 0 || portNumber > 65535)
    {
        PortInfo returnValue;
        returnValue.Port = -1;
        returnValue.Description = "Port out of bounds";
        return returnValue;
    }

    //Check to see if things have been initialized
    if(portStrings == NULL)
        readServicesFile();


    PortInfo returnValue;
    //the port number is valid so set it
    returnValue.Port = portNumber;

    //Check to see if it's a recommended port
    returnValue.Recommended = false;
    for(int i = 0; i < recommendedPortsSize; i++)
    {
        if (portNumber == recommendedPorts[i])
        {
            returnValue.Recommended = true;
        }
    }

   //Check to see if the port number is listed. The format in the file
   // is portname/portType. ex.: 22/tcp

   QStringList port = portStrings->filter(QRegExp("/\\b("+QString::number(portNumber)+"\\/"+portType.toLower()+")/g"));

   if(port.size() > 0)
   {
       //grab the first one, there may be duplicates due to colliding ports in the /etc/services file
       //but those are listed after the declaration for what the port officially should be used for
       QString line = port.at(0);

       //Split across spaces since it's whitespace delimited
       QStringList lineList = line.split(' ');

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
    
   return returnValue;

}

PortLookUp::PortLookUp()
{
    readServicesFile();
}

PortLookUp::~PortLookUp()
{
    delete portStrings;
}

void PortLookUp::readServicesFile()
{
    portStrings = new QStringList();

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

        portStrings->append(line);
    }
    services->close();
    delete services;
}
 
