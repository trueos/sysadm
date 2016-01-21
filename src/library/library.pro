CONFIG   += c++11
QT       += core network

TARGET=sysadm
target.path = /usr/local/lib

#DESTDIR= $$_PRO_FILE_PWD_

TEMPLATE	= lib
LANGUAGE	= C++
VERSION		= 1.0.0

HEADERS	+= 	sysadm-global.h \
                sysadm-general.h \
                sysadm-lifepreserver.h \
                sysadm-network.h \
                sysadm-firewall.h \
                sysadm-servicemanager.h\
                sysadm-systeminfo.h\
                sysadm-update.h \
                sysadm-usermanager.h

SOURCES	+=	NetDevice.cpp \
                sysadm-general.cpp \
                sysadm-lifepreserver.cpp \
                sysadm-network.cpp \
                sysadm-firewall.cpp \
                sysadm-servicemanager.cpp \
                sysadm-systeminfo.cpp \
                sysadm-update.cpp \
                sysadm-usermanager.cpp

include.path=/usr/local/include/
include.files=sysadm-*.h

INSTALLS += target include

QMAKE_LIBDIR = /usr/local/lib/qt5 /usr/local/lib
