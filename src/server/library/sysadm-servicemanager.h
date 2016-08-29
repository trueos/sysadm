#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QString>
#include <QList>
#include <QHash>

namespace sysadm{
struct Service{
    Service()
    {
        Name = "";
        Tag = "";
        Directory = "";
    }

    QString Name;
    QString Tag;
    QString Directory;
    QString Path;
    QString Description;
};

class ServiceManager
{
public:
    /**
     * @brief ServiceManager constructor for service manager
     * @param chroot if we're using a chroot, the chroot to use
     * @param ip if we're in a jail the ip to use
     */
    ServiceManager(QString chroot = "", QString ip = "");

    /**
     * @brief GetService gets a service by it's name
     * @param serviceName the name of the service to get, use the name that would be stored in Service.Name
     * @return the service with the service name, if the service is not found it returns a blank Service
     */
    Service GetService(QString serviceName);
    /**
     * @brief GetServices getter for the vector of services
     * @return returns the vector of services on the system
     */
    QList<Service> GetServices();

    QList<bool> isRunning(QList<Service> services); //return list in the same order as the input list
    bool isRunning(Service service); //single-item overload

    QList<bool> isEnabled(QList<Service> services); //return list in the same order as the input list
    bool isEnabled(Service service); //single-item overload

    /**
     * @brief Start starts a service
     * @param service the service to start
     */
    bool Start(Service service);
    /**
     * @brief Stop stops a service
     * @param service the service to stop
     */
    bool Stop(Service service);
    /**
     * @brief Restart restarts a service
     * @param service the service to restart
     */
    bool Restart(Service service);

    /**
     * @brief Enable enable a service
     * @param service the service to enable
     */
    void Enable(Service service);
    /**
     * @brief Disable disable a service
     * @param service the service to disable
     */
    void Disable(Service service);
private:
    QList<Service> services;
    Service loadServices(QString service = ""); //Return struct is optional - only used for a single service search

    QHash<QString, QString> rcdata; //rc.conf settings
    void loadRCdata();

    QString chroot;
    QString ip;
};
}
#endif // SERVICEMANAGER_H
