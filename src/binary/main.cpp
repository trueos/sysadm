#include <QCoreApplication>
#include <QDebug>

#include <sysadm-lifepreserver.h>
#include <sysadm-network.h>

int main( int argc, char ** argv )
{
  //Run a simple test of all the sysadm backend functions
  qDebug() << "**** Test Network Devices ****";
  QStringList devs = sysadm::NetDevice::listNetDevices();
  qDebug() <<"Devices:" << devs;
  for(int i=0; i<devs.length(); i++){
    sysadm::NetDevice D(devs[i]);
    qDebug() << "Check Device:" << D.device();
    qDebug() << " - name:" << D.devName();
    qDebug() << " - number:" << D.devNum();
    qDebug() << " - IPv4:" << D.ipAsString();
    qDebug() << " - IPv6:" << D.ipv6AsString();
    qDebug() << " - netmask:" << D.netmaskAsString();
    qDebug() << " - description:" << D.desc();
    qDebug() << " - MAC:" << D.macAsString();
    qDebug() << " - Uses DHCP:" << D.usesDHCP();
    //qDebug() << " - Media Type:" << D.mediaTypeAsString();
    qDebug() << " - Media Status:" << D.mediaStatusAsString();
    bool iswifi = D.isWireless();
    qDebug() << " - Is Wireless:" << iswifi;
    if(iswifi){
      qDebug() << " -- Wifi Parent:" << D.getWifiParent();
    }
    qDebug() << " - Is Active:" << D.isUp();
    qDebug() << " - Packets Rx:" << D.packetsRx() << "Errors:" << D.errorsRx();
    qDebug() << " - Packets Tx:" << D.packetsTx() << "Errors:" << D.errorsTx();
  }
  //Now run tests on the other network functionality
  qDebug() << "**** Test Network Entries ****";
  //QList<NetworkEntry> entries = sysadm::Network::listNetworkEntries();
  for(int i=0; i<devs.length(); i++){
    //qDebug() << "Entry:" << entries[i].name << "Aliases:"<< entries[i].aliases << "Number:" << entries[i].netnum;
    sysadm::NetDevSettings set = sysadm::Network::deviceRCSettings(devs[i]);
    qDebug() << "RC Settings:";
    qDebug() << " - Device:" << set.device;	
    qDebug() << " - Associated Device:" << set.asDevice;
    qDebug() << " - Use DHCP:" << set.useDHCP;
    qDebug() << " - Static IPv4:" << set.staticIPv4;
    qDebug() << " - Static IPv6:" << set.staticIPv6;
    qDebug() << " - Static Netmask:" << set.staticNetmask;
    qDebug() << " - Static Gateway:" << set.staticGateway;
    qDebug() << " - Allow Secure Wifi:" << set.wifisecurity;
  }

  return 0;
}
