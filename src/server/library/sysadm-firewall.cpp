//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details

#include "sysadm-firewall.h"
#include "sysadm-servicemanager.h"
#include "sysadm-general.h"

#include <QtCore>
#include <algorithm>

using namespace sysadm;

Firewall::Firewall(){

}

Firewall::~Firewall(){

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

    if(portStrings.isEmpty()){ readServicesFile(); }

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

   QStringList portList = portStrings.filter( QRegExp("\\s"+QString::number(port)+"/"+type+"\\s") );
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
       if(lineList.size() > 2){
           returnValue.Description = lineList.mid(2,-1).join(" ");
       }
   }

   return returnValue;

}


QList<PortInfo> Firewall::allPorts(){
  if(portStrings.isEmpty()){ readServicesFile(); }
  QList<PortInfo> output;
  for(int i=0; i<portStrings.length(); i++){
    //Line Format (9/9/16): "<keyword><bunch of whitespace><port>/<type><whitespace><description>
    QStringList line = portStrings[i].split(" ", QString::SkipEmptyParts);
    if(line.length()<2){ continue; } //invalid line
    PortInfo info;
    info.Keyword = line[0];
    info.Port = line[1].section("/",0,0).toInt();
    info.Type = line[1].section("/",1,1);
    if(line.length()>2){
      info.Description = line.mid(2,-1).join(" ");
    }
    output << info;
  }
  return output;
}


void Firewall::OpenPort(int port, QString type)
{
    QList<PortInfo> openports = LoadOpenPorts();
      openports.append(LookUpPort(port,type));
    SaveOpenPorts(openports);
}

void Firewall::OpenPort(QList<PortInfo> ports)
{
   QList<PortInfo> openports = LoadOpenPorts();
    for(PortInfo port : ports)
    {
        openports.append(port);
    }
    SaveOpenPorts(openports);
}

void Firewall::ClosePort(int port, QString type)
{
    QList<PortInfo> openports = LoadOpenPorts();
    openports.removeAll(LookUpPort(port,type));
    SaveOpenPorts(openports);
}

void Firewall::ClosePort(QList<PortInfo> ports)
{
    QList<PortInfo> openports = LoadOpenPorts();
    for(PortInfo port : ports)
    {
        openports.removeAll(port);
    }
    SaveOpenPorts(openports);
}

QList<PortInfo> Firewall::OpenPorts()
{
    return LoadOpenPorts();
}

bool Firewall::IsRunning()
{
    return General::sysctlAsInt("net.inet.ip.fw.enable") == 1;
}

bool Firewall::IsEnabled(){
    ServiceManager serviceManager;
    return serviceManager.isEnabled( serviceManager.GetService("ipfw") );
}

void Firewall::Start()
{
    ServiceManager serviceManager;
    serviceManager.Start( serviceManager.GetService("ipfw") );
}

void Firewall::Stop()
{
    ServiceManager serviceManager;
    serviceManager.Stop( serviceManager.GetService("ipfw") );
}

void Firewall::Restart()
{
    ServiceManager serviceManager;
    serviceManager.Restart( serviceManager.GetService("ipfw") );
}

void Firewall::Enable()
{
    ServiceManager serviceManager;
    serviceManager.Enable( serviceManager.GetService("ipfw") );
}

void Firewall::Disable()
{
    ServiceManager serviceManager;
    serviceManager.Disable( serviceManager.GetService("ipfw") );
}

bool Firewall::RestoreDefaults()
{
  if(QFile::exists("/usr/local/share/trueos/scripts/reset-firewall")){
    //refresh/restart the rules files
    QStringList args;
    args << "/usr/local/share/trueos/scripts/reset-firewall";
    General::RunCommand("sh",args);
    return true;
  }
  return false;
}

void Firewall::readServicesFile()
{
    portStrings.clear();

    // /etc/services contains a file that lists the various port numbers
    // and their descriptions
    QFile services("/etc/services");
    if(services.open(QFile::ReadOnly) ){
      QTextStream stream(&services);
      while(!stream.atEnd()){
        QString line = stream.readLine().simplified();;
        //jump down past the comments or empty lines
        if(line.startsWith("#") || line.isEmpty()){ continue; }
        //Skip any non-[tcp/udp] port types
        if( !line.contains("/tcp") && !line.contains("/udp") ){ continue; }
        line = line.replace("\t"," "); //uses a weird mixture of spaces and tabs for separation - just use spaces instead
        portStrings << line;
      }
      services.close();
    }
}

QList<PortInfo> Firewall::LoadOpenPorts()
{
  //qDebug() << "Read open ports";
  QList<PortInfo> openports;
  QFile file("/etc/ipfw.openports");
  if( file.open(QIODevice::ReadOnly) ){
    QTextStream in(&file);
    while( !in.atEnd() ){
      QString line = in.readLine();
      if(line.startsWith("#") || line.simplified().isEmpty()){ continue; }
      //File format: "<type> <port>" (nice and simple)
      //qDebug() << "Found Port:" << line;
      openports << LookUpPort(line.section(" ",1,1).toInt() ,line.section(" ",0,0));
    }
    file.close();
  }
  //qDebug() << "Finished with open ports";
  return openports;
}

void Firewall::SaveOpenPorts(QList<PortInfo> openports)
{
    //Convert to file format
      std::sort(openports.begin(), openports.end()); //make sure they are still sorted by port
      QStringList fileout;
      for(PortInfo port : openports){
        if(port.Port<0){ continue; } //invalid port
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
      if(IsRunning()){ Restart(); }
         /* QStringList args;
          args << "/usr/local/share/pcbsd/scripts/reset-firewall";
          General::RunCommand("sh",args);
      }*/
}
