
CONFIG 	+=  c++11

HEADERS	+= 	$${PWD}/sysadm-global.h \
                $${PWD}/sysadm-general.h \
                $${PWD}/sysadm-beadm.h \
                $${PWD}/sysadm-filesystem.h \
                $${PWD}/sysadm-iocage.h \
                $${PWD}/sysadm-iohyve.h \
                $${PWD}/sysadm-lifepreserver.h \
                $${PWD}/sysadm-network.h \
                $${PWD}/sysadm-firewall.h \
                $${PWD}/sysadm-servicemanager.h\
                $${PWD}/sysadm-systemmanager.h\
                $${PWD}/sysadm-update.h \
                $${PWD}/sysadm-users.h \
		$${PWD}/sysadm-zfs.h \
		$${PWD}/sysadm-pkg.h \
		$${PWD}/sysadm-moused.h \
                $${PWD}/sysadm-powerd.h \
                $${PWD}/sysadm-sourcectl.h


SOURCES	+=	$${PWD}/NetDevice.cpp \
                $${PWD}/sysadm-general.cpp \
                $${PWD}/sysadm-beadm.cpp \
                $${PWD}/sysadm-filesystem.cpp \
                $${PWD}/sysadm-iocage.cpp \
                $${PWD}/sysadm-iohyve.cpp \
                $${PWD}/sysadm-lifepreserver.cpp \
                $${PWD}/sysadm-network.cpp \
                $${PWD}/sysadm-firewall.cpp \
                $${PWD}/sysadm-servicemanager.cpp \
                $${PWD}/sysadm-systemmanager.cpp \
                $${PWD}/sysadm-update.cpp \
                $${PWD}/sysadm-users.cpp \
		$${PWD}/sysadm-zfs.cpp \
		$${PWD}/sysadm-pkg.cpp \
		$${PWD}/sysadm-moused.cpp \
                $${PWD}/sysadm-powerd.cpp \
                $${PWD}/sysadm-sourcectl.cpp
