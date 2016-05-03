TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off release
QT = core network websockets

HEADERS	+= globals.h \
		BridgeServer.h \
		BridgeConnection.h
		
SOURCES	+= main.cpp \
		BridgeServer.cpp \
		BridgeConnection.cpp



TARGET=sysadm-bridge
target.path=/usr/local/bin

scripts.path=/usr/local/bin/
scripts.extra=install sysadm-bridge-start $(INSTALL_ROOT)/usr/local/bin/

INSTALLS += target scripts


QMAKE_LIBDIR = /usr/local/lib/qt5 /usr/local/lib
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lpam -lutil -lssl -lcrypto
