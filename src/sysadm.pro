TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= server

#Make sure to list the library as a requirement for the others (for parallellized builds)
#binary.depends = library
#server.depends = library

rcd.path = /usr/local/etc/rc.d/
rcd.extra = cp rc.d/* $(INSTALL_ROOT)/usr/local/etc/rc.d/

daemons.path = /usr/local/etc/launchd/daemons/
daemons.extra = cp daemons.d/* $(INSTALL_ROOT)/usr/local/etc/launchd/daemons/

conf.path = /usr/local/etc/
conf.extra = cp conf/sysadm.conf ${INSTALL_ROOT}/usr/local/etc/sysadm.conf.dist

INSTALLS += rcd conf daemons
