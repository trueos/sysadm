// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_DISPATCH_PROCESS_SYSTEM_H
#define _PCBSD_SYSADM_DISPATCH_PROCESS_SYSTEM_H

#include "globals-qt.h"


// == Simple Process class for running sequential commands ==
// ==  INTERNAL ONLY - Do not use directly ==
class DProcess : public QProcess{
	Q_OBJECT
public:
	DProcess(QObject parent = 0);
	~DProcess();
	
	QString ID;
	QStringList cmds;

	//output variables for logging purposes
	bool success;
	QDateTime started, finished;
	QStringList rawcmds; //copy of cmds at start of process

	void startProc();

	//Get the current process log (can be run during/after the process runs)
	QString getProcLog();

	bool isRunning();

private:
	QString proclog;

private slots:
	void cmdFinished(int, QProcess::ExitStatus);

signals:
	void ProcFinished(QString ID);
};
	

class Dispatcher : public QObject{
	Q_OBJECT
public:
	enum PROC_QUEUE { NO_QUEUE = 0, PKG_QUEUE, IOCAGE_QUEUE };
#define enum_length 3 //This needs to be the number of items  in the enum above
	
	Dispatcher();
	~Dispatcher();

public slots:
	//Main start/stop
	void start(QString queuefile); //load any previously-unrun processes
	void stop(); //save any currently-unrun processes for next time

	//Main Calling Functions (single command, or multiple in-order commands)
	void queueProcess(QString ID, QString cmd); //uses NO_QUEUE
	void queueProcess(QString ID, QStringList cmds); //uses NO_QUEUE
	void queueProcess(Dispatcher::PROC_QUEUE, QString ID, QString cmd);
	void queueProcess(Dispatcher::PROC_QUEUE, QString ID, QStringList cmds);

private:
	// Queue file
	QString queue_file;
	
	//Internal lists
	QHash<PROC_QUEUE, QList<DProcess*> > QUEUES;

	//Simplification routine for setting up a process
	DProcess* createProcess(QString ID, QStringList cmds){
	  DProcess* P = new DProcess(this);
	    P->cmds = cmds;
	    P->ID = ID;
	    connect(P, SIGNAL(ProcFinished(QString)), this, SLOT(ProcFinished(QString)) );
	  return P;
	}

private slots:
	void ProcFinished(QString ID);
	void CheckQueues();

signals:
	void DispatchFinished(QString ID, bool success);
	void DispatchStarting(QString ID);
	
};

#endif