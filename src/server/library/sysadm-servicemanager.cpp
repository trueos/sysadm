#include "sysadm-servicemanager.h"
#include "sysadm-general.h"

#include <QFile>
#include <QDir>

using namespace sysadm;
ServiceManager::ServiceManager(QString chroot, QString ip)
{
    this->chroot = chroot;
    this->ip = ip;
    usingOpenRC = QFile::exists(chroot+"/sbin/rc-update");
    loadRCdata();
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

// look at rcdata now
QList<bool> ServiceManager::isRunning(QList<Service> services){
   //return list in the same order as the input list
  QList<bool> out;
  for(int i=0; i<services.length(); i++){
    if(!rcdata.contains(services[i].Name)){ out << false; }
    else{ out << sysadm::General::RunQuickCommand("service",QStringList() << services[i].Directory << "status"); }
  }
  return out;
}

bool ServiceManager::isRunning(Service service){ //single-item overload
  QList<bool> run = isRunning( QList<Service>() << service);
  if(!run.isEmpty()){ return run.first(); }
  else{ return false; }
}

// see if service is in rcdata and not in unuseddata
QList<bool> ServiceManager::isEnabled(QList<Service> services){
   //return list in the same order as the input list
  QList<bool> out;
  //Now go through the list of services and report which ones are enabled
  for(int i=0; i<services.length(); i++){
    bool enabled = false;
    enabled = (rcdata.contains(services[i].Name) && !unuseddata.contains(services[i].Name));
    out << enabled;
  }
  return out;
}

bool ServiceManager::isEnabled(Service service){ //single-item overload
  QList<bool> use = isEnabled( QList<Service>() << service);
  if(!use.isEmpty()){ return use.first(); }
  else{ return false; }
}

// onestart doesn't matter anymore
bool ServiceManager::Start(Service service)
{
    if(service.Directory.isEmpty()){ return false; }
    // Start the process
    QString prog;
    QStringList args;
    bool once = !isEnabled(service) && !usingOpenRC;
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args << (once ? "onestart" : "start") ;
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "onestart" : "start" );
    }
    return General::RunQuickCommand(prog,args);
}

// onestop doesn't matter anymore
bool ServiceManager::Stop(Service service)
{
    if(service.Directory.isEmpty()){ return false; }
    // Start the process
    QString prog;
    QStringList args;
    bool once = !isEnabled(service) && !usingOpenRC;
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args << (once ? "onestop" : "stop") ;
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "onestop" : "stop" );
    }
    return General::RunQuickCommand(prog,args);
}

bool ServiceManager::Restart(Service service)
{
    if(service.Directory.isEmpty()){ return false; }
    QString prog;
    QStringList args;
    bool once = !isEnabled(service) && !usingOpenRC;
    if ( chroot.isEmpty() ) {
      prog = "service";
      args << service.Directory;
      args <<(once ? "one" : "" )+ QString("restart");
    } else {
      prog = "warden";
      args << "chroot" << ip << "service" << service.Directory << (once ? "one" : "" )+QString("restart");
    }
    return General::RunQuickCommand(prog,args);
}

// Enable is rc-update add name runlevel
bool ServiceManager::Enable(Service service)
{
  if(!rcdata.contains(service.Name)){ return false; }
  return enableDisableService(service.Name, true);
}

// Disable is rc-update del name runlevel
bool ServiceManager::Disable(Service service)
{
  if(!rcdata.contains(service.Name)){ return false; }
  return enableDisableService(service.Name, false);
}

// get list of files from /etc/init.d and /usr/local/etc/init.d
// Service::Name is pure filename
// Service::Tag is nto used anymore
// Service::Description is from file "name=", "desc=", "description="
// rc-status --nocolor --servicelist indicates "isRunning"
// rc-status --nocolor --unused indicates services not enabled
//
Service ServiceManager::loadServices(QString name)
{
    QString tmp;
    bool valid;
    Service service;

    // OpenRC directories are /etc/init.d and /usr/local/etc/init.d
    QStringList stringDirs;
    if(usingOpenRC){
      //OpenRC
      stringDirs << chroot + "/etc/init.d" << chroot + "/usr/local/etc/init.d";
    }else{
      //FreeBSD rc.d
      stringDirs << chroot + "/etc/rc.d" << chroot + "/usr/local/etc/rc.d";
    }

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
	      valid=true; //false;
		service.Name = directory[i];  // filename is the service name now
                service.Directory=directory[i]; //filename only
                service.Path = dir+"/"+directory[i]; //full path w/ filename
                QTextStream stream( &file );
                stream.setCodec("UTF-8");
                QString line;
                while ( !stream.atEnd() )
                {
                    line = stream.readLine(); // line of text excluding '\n'

                    if (line.simplified().startsWith("description=") ||
			line.simplified().startsWith("desc=") ||
			(line.simplified().startsWith("name=") && service.Description.isEmpty()) ) {
                      service.Description = line.section("=\"",1,-1).section("\"",0,0);
                    }
                }
                file.close();

                if ( !valid )
                    continue;

                QString cDir = dir;
                if ( ! chroot.isEmpty() )
                    cDir.replace(chroot, "");
                if ( service.Name.indexOf("$") == 0 )
                    service.Name = service.Directory;
               if(!name.isEmpty() ){ return service; } //found the requested service - return it
                services << service;
                //qDebug() << "Added Service:" << cDir << service.Directory << service.Name << service.Tag;
            }
        }
    }
    loadRunlevels();
  return Service();
}

// rc-status --nocolor --servicelist to get all services
void ServiceManager::loadRCdata(){
  rcdata.clear();
  // output is spNAMEsp...[spSTATUSsp]\n
  QStringList info = sysadm::General::RunCommand("rc-status --nocolor --servicelist").split("\n");
  for(int i=0; i<info.length(); i++){
    if (info[i].length()){
      QString name = info[i].section(" ",0,0,QString::SectionSkipEmpty);
      QString status = info[i].section(" ",-2,-2,QString::SectionSkipEmpty);
      rcdata.insert( name, status);
      //qDebug() << "load rc data " << name << " status " << status;
    }
  }
  loadUnusedData();
}

// rc-status --nocolor --unused to find services not in any runlevel
void ServiceManager::loadUnusedData() {
  //Read all the rc.conf files in highest-priority order
  unuseddata.clear();
  // output is spNAMEsp...[spSTATUSsp]\n
  // with runlevel lines mixed
  QStringList info = sysadm::General::RunCommand("rc-status --nocolor --unused").split("\n");
  for(int i=0; i<info.length(); i++){
    if (info[i].length()){
      QString name = info[i].section(" ",0,0,QString::SectionSkipEmpty);
      QString status = info[i].section(" ",-2,-2,QString::SectionSkipEmpty);
      unuseddata.insert( name, status);
      //qDebug() << "load unused data " << name << " status " << status;
    }
  }
}

// get the services enabled for all runlevels so we can update services
void ServiceManager::loadRunlevels() {
  if(!usingOpenRC){ return; }
  QStringList info = sysadm::General::RunCommand("rc-status --nocolor --all").split("\n");
  QString runlevel = "default";
  for (int i=0; i<info.length(); i++) {
    if (info[i].contains("Runlevel")) {
      runlevel = info[i].section(": ", -1,-1);
      continue;
    }
    QString name = info[i].section(" ",0,0,QString::SectionSkipEmpty);
    // search services for name and update runlevel
    for (int j=0; j < services.length(); j++){
      if (services[j].Name == name) {
	services[j].Runlevel = runlevel;
	//qDebug() << "Updating " << services[j].Name << " runlevel " << runlevel;
      }
    }
  }
}

bool ServiceManager::enableDisableService(QString name, bool enable) {
  // if not, use default
  QString prog = usingOpenRC ? "rc-update" : "sysrc";
  QStringList args;
  if(usingOpenRC){
    if (enable){ args << "add"; }
    else{ args << "delete"; }
    args << name;
  }else{
    args << name+"_enable=\""+ (enable ? "YES" : "NO") + "\"";
  }
  //qDebug() << prog << " " << args;
  bool ret = sysadm::General::RunQuickCommand(prog,args);
  loadUnusedData();
  return ret;
}
