TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release
QT = core network websockets concurrent

HEADERS	+= globals.h globals-qt.h \
		WebServer.h \
		WebSocket.h \
		syscache-client.h \
		dispatcher-client.h \
		RestStructs.h \
		AuthorizationManager.h \ 
		SslServer.h \
		EventWatcher.h
		
SOURCES	+= main.cpp \
		WebServer.cpp \
		WebSocket.cpp \
		WebBackend.cpp \
		syscache-client.cpp \
		dispatcher-client.cpp \
		AuthorizationManager.cpp \
		EventWatcher.cpp


TARGET=sysadm-server
target.path=/usr/local/bin


INSTALLS += target


QMAKE_LIBDIR = ../library /usr/local/lib/qt5 /usr/local/lib
INCLUDEPATH += ../library /usr/local/include
LIBS += -L../library -L/usr/local/lib -lpam -lutil -lsysadm
