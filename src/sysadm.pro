TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= server

#Make sure to list the library as a requirement for the others (for parallellized builds)
#binary.depends = library
#server.depends = library

rcd.path = /usr/local/etc/rc.d/
rcd.extra = cp rc.d/* $(INSTALL_ROOT)/usr/local/etc/rc.d/

dconf.path = /usr/local/etc/launchd/daemons/
dconf.extra = cp daemons.d/* $(INSTALL_ROOT)/usr/local/etc/launchd/daemons/

wsdaemon.path = /usr/local/share/launchd/daemons/
wsdaemon.extra = cp daemons.d/org.pcbsd.sysadm.json.dist $(INSTALL_ROOT)/usr/local/share/launchd/daemons/org.pcbsd.sysadm.json

rstdaemon.path = /usr/local/share/launchd/daemons/
rstdaemon.extra = cp daemons.d/org.pcbsd.sysadm-rest.json.dist $(INSTALL_ROOT)/usr/local/share/launchd/daemons/org.pcbsd.sysadm-rest.json

conf.path = /usr/local/etc/
conf.extra = cp conf/sysadm.conf ${INSTALL_ROOT}/usr/local/etc/sysadm.conf.dist

INSTALLS += rcd conf wsdaemon rstdaemon dconf
