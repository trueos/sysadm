#include "sysadm-servicemanager.h"
#include "sysadm-general.h"

#include <QFile>
#include <QDir>

using namespace sysadm;
ServiceManager::ServiceManager(QString chroot, QString ip)
{
    this->chroot = chroot;
    this->ip = ip;
    //loadServices();
}

Service ServiceManager::GetService(QString serviceName)
{
  if(!services.isEmpty()){
    for(int i=0; i<services.length(); i++){
        if(services[i].Name == serviceName)
            return services[i];
    }
  }else{
    return loadServices(serviceName);
  }
  return Service(); //no service found
}

QList<Service> ServiceManager::GetServices()
{
    if(services.isEmpty()){ loadServices(); }
    return services;
}

QList<bool> ServiceManager::isRunning(QList<Service> services){
   //return list in the same order as the input list
  QList<bool> out;
  for(int i=0; i<services.length(); i++){
    out << false; //TO-DO - need to figure out a way to detect process status
  }
  return out;
}

bool ServiceManager::isRunning(Service service){ //single-item overload
  QList<bool> run = isRunning( QList<Service>() << service);
  if(!run.isEmpty()){ return run.first(); }
  else{ return false; }
}

QList<bool> ServiceManager::isEnabled(QList<Service> services){
   //return list in the same order as the input list
  QList<bool> out;
  loadRCdata();  
  //Now go through the list of services and report which ones are enabled
  for(int i=0; i<services.length(); i++){
    bool enabled = false;
    if(rcdata.contains(services[i].Tag)){ enabled = rcdata.value(services[i].Tag)=="\"YES\""; }
    out << enabled;
  }
  return out;
}

bool ServiceManager::isEnabled(Service service){ //single-item overload
  QList<bool> use = isEnabled( QList<Service>() << service);
  if(!use.isEmpty()){ return use.first(); }
  else{ return false; }
}

void ServiceManager::Start(Service service)
{
    // Start the process
    QString prog;
    QStringList args;
    bool once = !isEnabled(service);
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args << "start";
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "one" : "" )+QString("start");
    }
    General::RunCommand(prog,args);
}

void ServiceManager::Stop(Service service)
{
    // Start the process
    QString prog;
    QStringList args;
    bool once = !isEnabled(service);
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args << "stop";
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "one" : "" )+QString("stop");
    }
    General::RunCommand(prog,args);
}

void ServiceManager::Restart(Service service)
{
    QString prog;
    QStringList args;
    bool once = !isEnabled(service);
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args <<(once ? "one" : "" )+ QString("restart");
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "one" : "" )+QString("restart");
    }
    General::RunCommand(prog,args);
}

void ServiceManager::Enable(Service service)
{
    General::setConfFileValue( chroot + "/etc/rc.conf", service.Tag, service.Tag + "=\"YES\"", -1);
}

void ServiceManager::Disable(Service service)
{
    General::setConfFileValue( chroot + "/etc/rc.conf", service.Tag, service.Tag + "=\"NO\"", -1);
}

Service ServiceManager::loadServices(QString name)
{
    QString tmp;
    bool valid;
    Service service;

    QStringList stringDirs;
    stringDirs << chroot + "/etc/rc.d" << chroot + "/usr/local/etc/rc.d";

    for ( QString dir: stringDirs)
    {

        QDir directory( dir );

        directory.setFilter( QDir::Files );
        directory.setSorting( QDir::Name );

        if ( directory.count() == 0 )
            return Service();

        for (unsigned int i = 0; i < directory.count(); i++ )
        {
	    if(!name.isEmpty() && directory[i]!=name){ continue; } //not the right service - go to the next one
            service = Service();

            QFile file( dir + "/" + directory[i] );
            if ( file.open( QIODevice::ReadOnly ) )
            {
                valid=false;
                service.Directory=directory[i]; //filename only
                service.Path = dir+"/"+directory[i]; //full path w/ filename
                QTextStream stream( &file );
                stream.setCodec("UTF-8");
                QString line;
                while ( !stream.atEnd() )
                {
                    line = stream.readLine(); // line of text excluding '\n'

                    if ( line.indexOf("name=") == 0)
                    {
                        valid=true;
                        tmp = line.replace("name=", "");
                        service.Name = tmp.replace('"', "");
                    }
                    if ( line.indexOf("rcvar=") == 0)
                    {
                        if ( tmp.isEmpty() )
                            continue;

                        tmp = line.replace("rcvar=", "");
                        tmp = tmp.replace('"', "");
                        tmp = tmp.replace("'", "");
                        tmp = tmp.replace("`", "");
                        tmp = tmp.replace("$(set_rcvar)", "");
                        tmp = tmp.replace("$set_rcvar", "");
                        tmp = tmp.replace("set_rcvar", "");
                        tmp = tmp.replace("${name}", "");
                        tmp = tmp.replace("_enable", "");
                        tmp = tmp.replace(" ", "");

                        if (tmp.isEmpty())
                            service.Tag = service.Name + "_enable";
                        else
                            service.Tag = tmp;

                        if ( service.Tag.indexOf("_enable") == -1 )
                            service.Tag=service.Tag + "_enable";
                        break;
                    }
                    if (line.simplified().startsWith("desc=")) {
                      service.Description = line.section("=\"",1,-1).section("\"",0,0);
                    }
                }
                file.close();

                if ( !valid || service.Tag.isEmpty() )
                    continue;

                QString cDir = dir;
                if ( ! chroot.isEmpty() )
                    cDir.replace(chroot, "");
                if ( service.Tag.indexOf("$") == 0 )
                    service.Tag = service.Directory + "_enable";
                if ( service.Name.indexOf("$") == 0 )
                    service.Name = service.Directory;
               if(!name.isEmpty() ){ return service; } //found the requested service - return it
                services << service;
                //qDebug() << "Added Service:" << cDir << service.Directory << service.Name << service.Tag;
            }
        }
    }
  return Service();
}

void ServiceManager::loadRCdata(){
  //Read all the rc.conf files in highest-priority order
  rcdata.clear();
  QDir dir("/etc");
  QStringList confs = dir.entryList(QStringList() << "rc.conf*", QDir::Files, QDir::Name | QDir::Reversed);
  //qDebug() << "Conf file order:" << confs;
  for(int i=0; i<confs.length(); i++){
    QFile file(dir.absoluteFilePath(confs[i]));
    if( file.open(QIODevice::ReadOnly) ){
      //qDebug() << "Read File:" << confs[i];
      bool insection = true;
      QTextStream stream(&file);
      while(!stream.atEnd()){
        QString info = stream.readLine();
        //qDebug() << "Read Line:" << info;
        if(info.contains("=") && insection){
          rcdata.insert(info.section("=",0,0).simplified(), info.section("=",1,-1));
          //qDebug() << "Got data entry:" << info;
        }else if(info.simplified()=="fi"){ 
          insection= true;
        }else if(info.simplified().startsWith("if [ ") ){
          QStringList args = info.section("]",0,0).section("[",1,-1).split("\""); //odd numbers are files, even are arguments
          for(int j=0; j<args.length()-1; j+=2){
            //qDebug() << "Check if arguments:" << args[j] << args[j+1] << insection;
            if(!args[j].contains("-e")){ insection = false; break; } //don't know how to handle this - skip section
            if(args[j].contains("-o")){ insection == insection || QFile::exists(args[j+1]); }
            else if(args[j].contains("-a")){ insection == insection && QFile::exists(args[j+1]); }
            else{ insection = QFile::exists(args[j+1]); } //typically the first argument check
            //qDebug() << " - Now:" << insection;
          } //end loop over existance arguments
        }
      }//end loop over lines
      file.close();
    }
  }
}
