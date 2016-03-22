
CONFIG 	+=  c++11

HEADERS	+= 	$${PWD}/sysadm-global.h \
                $${PWD}/sysadm-general.h \
                $${PWD}/sysadm-beadm.h \
                $${PWD}/sysadm-iocage.h \
                $${PWD}/sysadm-iohyve.h \
                $${PWD}/sysadm-lifepreserver.h \
                $${PWD}/sysadm-network.h \
                $${PWD}/sysadm-firewall.h \
                $${PWD}/sysadm-servicemanager.h\
                $${PWD}/sysadm-systemmanager.h\
                $${PWD}/sysadm-update.h \
                $${PWD}/sysadm-usermanager.h \
		$${PWD}/sysadm-zfs.h

SOURCES	+=	$${PWD}/NetDevice.cpp \
                $${PWD}/sysadm-general.cpp \
                $${PWD}/sysadm-beadm.cpp \
                $${PWD}/sysadm-iocage.cpp \
                $${PWD}/sysadm-iohyve.cpp \
                $${PWD}/sysadm-lifepreserver.cpp \
                $${PWD}/sysadm-network.cpp \
                $${PWD}/sysadm-firewall.cpp \
                $${PWD}/sysadm-servicemanager.cpp \
                $${PWD}/sysadm-systemmanager.cpp \
                $${PWD}/sysadm-update.cpp \
                $${PWD}/sysadm-usermanager.cpp \
		$${PWD}/sysadm-zfs.cpp

