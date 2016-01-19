//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details

#include "sysadm-firewall.h"
#include "sysadm-general.h"
#include <QtCore>
#include <algorithm>

using namespace sysadm;

Firewall::Firewall()
{
    readServicesFile();
    LoadOpenPorts();

    firewallService = serviceManager.GetService("ipfw");
}

Firewall::~Firewall()
{
    delete portStrings;
}

PortInfo Firewall::LookUpPort(int port, QString type)
{
    //Make sure that the port is valid
    if (port < 0 || port > 65535)
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
    returnValue.Port = port;

    //make sure that the portType is cased in lower to match the service file and
    //then store it in the returnValue, since there isn't a huge point in checking
    //the validitiy of the type since /etc/services lists more than udp/tcp
    type = type.toLower();
    returnValue.Type =  type;

    //Check to see if it's a recommended port
    returnValue.Recommended = false;
    for(int recommendedPort : recommendedPorts)
    {
        if (port == recommendedPort)
        {
            returnValue.Recommended = true;
        }
    }

   //Check to see if the port number is listed. The format in the file
   // is portname/portType. ex.: 22/tcp

   QStringList portList = portStrings->filter(QString::number(port) + "/" + type);
   if(portList.size() > 0)
   {
       //grab the first one, there may be duplicates due to colliding ports in the /etc/services file
       //but those are listed after the declaration for what the port officially should be used for
       QString line = portList.at(0);

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

void Firewall::OpenPort(int port, QString type)
{
    openports.append(LookUpPort(port,type));
    SaveOpenPorts();
}

void Firewall::OpenPort(QVector<PortInfo> ports)
{
    for(PortInfo port : ports)
    {
        openports.append(port);
    }
    SaveOpenPorts();
}

void Firewall::ClosePort(int port, QString type)
{
    openports.removeAll(LookUpPort(port,type));
    SaveOpenPorts();
}

void Firewall::ClosePort(QVector<PortInfo> ports)
{
    for(PortInfo port : ports)
    {
        openports.removeAll(port);
    }
    SaveOpenPorts();
}

QVector<PortInfo> Firewall::OpenPorts()
{
    return openports;

}

bool Firewall::IsRunning()
{
    return General::sysctlAsInt("net.inet.ip.fw.enable") == 1;
}

void Firewall::Start()
{
    serviceManager.Enable(firewallService);
    serviceManager.Start(firewallService);
}

void Firewall::Stop()
{
    serviceManager.Enable(firewallService);
    serviceManager.Stop(firewallService);
}

void Firewall::Restart()
{
    serviceManager.Enable(firewallService);
    serviceManager.Restart(firewallService);
}

void Firewall::Enable()
{
    serviceManager.Enable(firewallService);
}

void Firewall::Disable()
{
    serviceManager.Disable(firewallService);
}

void Firewall::RestoreDefaults()
{
    const QString ipfwrules = "/etc/ipfw.rules";
    const QString ipfwrulesprevious = ipfwrules + ".previous";

    const QString ipfwopenports = "/etc/ipfw.openports";
    const QString ipfwopenportsprevious = ipfwopenports + ".previous";

    //QFile has a rename command that acts like move, however it won't
    //clobber a file if it already exists, so we have to check if there
    //already is a .previous and if so delete it before moving the file
    //to .previous

    //move the files out of the way
    if(QFile::exists(ipfwrulesprevious))
        QFile::remove(ipfwrulesprevious);
    QFile::rename(ipfwrules,ipfwrulesprevious);

    if(QFile::exists(ipfwopenportsprevious))
        QFile::remove(ipfwopenportsprevious);
    QFile::rename(ipfwopenports,ipfwopenportsprevious);


    //refresh/restart the rules files
    QStringList args;
    args << "/usr/local/share/pcbsd/scripts/reset-firewall";
    General::RunCommand("sh",args);

    LoadOpenPorts();
}

void Firewall::readServicesFile()
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
        if(line[0] == '#' || line.simplified().isEmpty())
            continue;

        //remove all of the extraneous whitespace in the line
        line = line.simplified();

        portStrings->append(line);
    }
    services->close();
    delete services;
}

void Firewall::LoadOpenPorts()
{
  openports.clear();
  QFile file("/etc/ipfw.openports");
  if( file.open(QIODevice::ReadOnly) ){
    QTextStream in(&file);
    while( !in.atEnd() ){
      QString line = in.readLine();
      if(line.startsWith("#") || line.simplified().isEmpty()){ continue; }
      //File format: "<type> <port>" (nice and simple)
      openports.append(LookUpPort(line.section(" ",1,1).toInt(),line.section(" ",0,0)));
    }
    file.close();
  }
  //order them in ascending order by port then port type
  std::sort(openports.begin(),openports.end());
}

void Firewall::SaveOpenPorts()
{
    //Convert to file format
      std::sort(openports.begin(), openports.end()); //make sure they are still sorted by port
      QStringList fileout;
      for(PortInfo port : openports){
        fileout.append("#" + port.Keyword + ": " + port.Description + "\n" +
                   port.Type +" "+QString::number(port.Port));
      }
      //Always make sure that the file always ends with a newline
      if(!fileout.isEmpty()){ fileout << ""; }
      //Save to file
      QFile file("/etc/ipfw.openports");
      if( file.open(QIODevice::WriteOnly | QIODevice::Truncate) ){
        QTextStream out(&file);
        out << fileout.join("\n");
        file.close();
      }
      //Re-load/start rules (just in case - it is a smart script)
      if(IsRunning())
      {
          QStringList args;
          args << "/usr/local/share/pcbsd/scripts/reset-firewall";
          General::RunCommand("sh",args);
      }
}

