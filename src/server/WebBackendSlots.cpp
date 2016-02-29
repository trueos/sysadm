// ===============================
// Slots for long running Library Tasks
// Available under the 3-clause BSD License
// Written by: Kris Moore <kris@pcbsd.org> FEB 2016
// =================================
#include <WebSocket.h>
#define DEBUG 0


// Iohyve Fetch is done
void WebSocket::slotIohyveFetchDone(QString id, int retcode, QString log)
{
  //qDebug() << "Fetch Done" << id << retcode << log;
}

// Iohyve Fetch has output to read
void WebSocket::slotIohyveFetchProcessOutput(QString output)
{
  //qDebug() << "Fetch Do" << output;
}
