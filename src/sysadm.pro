TEMPLATE = subdirs
CONFIG += recursive

SUBDIRS+= server bridge

#Standard FreeBSD RC
rcd.path = /usr/local/etc/rc.d/
rcd.files = rc.d/sysadm rc.d/sysadm-rest rc.d/sysadm-bridge

#OpenRC for TrueOS
initd.path = /usr/local/etc/init.d/
initd.files = init.d/sysadm init.d/sysadm-rest init.d/sysadm-bridge

#jobd.path = /usr/local/etc/job.d/
#jobd.extra = cp job.d/* $(INSTALL_ROOT)/usr/local/etc/job.d/

#wsdaemon.path = /usr/local/etc/job.d/
#wsdaemon.extra = cp job.d/org.pcbsd.sysadm.json $(INSTALL_ROOT)/usr/local/etc/job.d/org.pcbsd.sysadm.json

#rstdaemon.path = /usr/local/etc/job.d/
#rstdaemon.extra = cp job.d/org.pcbsd.sysadm-rest.json $(INSTALL_ROOT)/usr/local/etc/job.d/org.pcbsd.sysadm-rest.json

conf.path = /usr/local/etc/
conf.extra = cp conf/sysadm.conf ${INSTALL_ROOT}/usr/local/etc/sysadm.conf.dist

INSTALLS += rcd initd conf 
