TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= server bridge

rcd.path = /usr/local/etc/rc.d/
rcd.files = rc.d/sysadm rc.d/sysadm-rest rc.d/sysadm-bridge

#dconf.path = /usr/local/etc/job.d/
#dconf.extra = cp job.d/* $(INSTALL_ROOT)/usr/local/etc/job.d/

#wsdaemon.path = /usr/local/etc/job.d/
#wsdaemon.extra = cp job.d/org.pcbsd.sysadm.json $(INSTALL_ROOT)/usr/local/etc/job.d/org.pcbsd.sysadm.json

#rstdaemon.path = /usr/local/etc/job.d/
#rstdaemon.extra = cp job.d/org.pcbsd.sysadm-rest.json $(INSTALL_ROOT)/usr/local/etc/job.d/org.pcbsd.sysadm-rest.json

conf.path = /usr/local/etc/
conf.extra = cp conf/sysadm.conf ${INSTALL_ROOT}/usr/local/etc/sysadm.conf.dist

INSTALLS += rcd conf 
