// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_SERVER_GLOBALS_H
#define _PCBSD_SYSADM_SERVER_GLOBALS_H

#include "globals-qt.h"

#include "LogManager.h"

//Global variables/classes (intially created in main.cpp)
extern QSettings *CONFIG;
extern bool WS_MODE;

#include "EventWatcher.h"
extern EventWatcher *EVENTS;
#include "Dispatcher.h"
extern Dispatcher *DISPATCHER;

//Special defines
#define WSPORTNUMBER 12150 	// WebSocket server default port
#define PORTNUMBER 12151		// TCP server default port

#endif
