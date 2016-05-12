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
#define BRIDGEPORTNUMBER 12149 //Default port for a sysadm-bridge
#define WSPORTNUMBER 12150 	// WebSocket server default port
#define PORTNUMBER 12151		// TCP server default port

//Config file settings for this instance of the server
extern int BlackList_BlockMinutes;
extern int BlackList_AuthFailsToBlock;
extern int BlackList_AuthFailResetMinutes;
extern bool BRIDGE_ONLY; //bridge-only mode (no listening on a socket)

#endif
