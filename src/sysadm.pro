TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= server

#Make sure to list the library as a requirement for the others (for parallellized builds)
#binary.depends = library
#server.depends = library

rcd.path = /usr/local/etc/rc.d/
rcd.extra = cp rc.d/* $(INSTALL_ROOT)/usr/local/etc/rc.d/

INSTALLS += rcd
