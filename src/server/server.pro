TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off release
QT = core network websockets concurrent

HEADERS	+= globals.h globals-qt.h \
		WebServer.h \
		WebSocket.h \
		syscache-client.h \
		RestStructs.h \
		AuthorizationManager.h \ 
		SslServer.h \
		EventWatcher.h \
		LogManager.h \
		Dispatcher.h \
		DispatcherParsing.h
		
SOURCES	+= main.cpp \
		WebServer.cpp \
		WebSocket.cpp \
		WebBackend.cpp \
		WebBackendSlots.cpp \
		syscache-client.cpp \
		AuthorizationManager.cpp \
		EventWatcher.cpp \
		LogManager.cpp \
		Dispatcher.cpp

#Now pull in the the subsystem library classes and such
include("library/library.pri");

TARGET=sysadm-server
target.path=/usr/local/bin


INSTALLS += target


QMAKE_LIBDIR = /usr/local/lib/qt5 /usr/local/lib
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lpam -lutil -lssl -lcrypto
