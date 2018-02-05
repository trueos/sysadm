TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_off release
QT = core network websockets concurrent sql

HEADERS	+= globals.h globals-qt.h \
		WebServer.h \
		WebSocket.h \
		RestStructs.h \
		AuthorizationManager.h \ 
		SslServer.h \
		EventWatcher.h \
		LogManager.h \
		Dispatcher.h
		
SOURCES	+= main.cpp \
		WebServer.cpp \
		WebSocket.cpp \
		RestStructs.cpp \
		WebBackend.cpp \
		AuthorizationManager.cpp \
		EventWatcher.cpp \
		LogManager.cpp \
		Dispatcher.cpp \
		DispatcherParsing.cpp

#Now pull in the the subsystem library classes and such
include("library/library.pri");

TARGET=sysadm-binary
target.path=/usr/local/bin

scripts.path=/usr/local/bin/
scripts.extra=install sysadm-server $(INSTALL_ROOT)/usr/local/bin/

INSTALLS += target scripts


QMAKE_LIBDIR = /usr/local/lib/qt5 /usr/local/lib
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lpam -lutil -lssl -lcrypto

  #Some conf to redirect intermediate stuff in separate dirs
  UI_DIR=./.build/ui/
  MOC_DIR=./.build/moc/
  OBJECTS_DIR=./.build/obj
  RCC_DIR=./.build/rcc
  QMAKE_DISTCLEAN += -r ./.build
