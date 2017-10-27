// ===============================
//  PC-BSD REST API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> 2015-2016
// =================================
#ifndef _PCBSD_SYSADM_DISPATCH_PROCESS_SYSTEM_H
#define _PCBSD_SYSADM_DISPATCH_PROCESS_SYSTEM_H

#include "globals-qt.h"


// == Simple Process class for running sequential commands ==
class DProcess : public QProcess{
	Q_OBJECT
public:
	DProcess(QObject *parent = 0);
	~DProcess();

	QString ID;
	QStringList cmds;

	//output variables for logging purposes
	bool success;
	//QDateTime t_started, t_finished;
	QStringList rawcmds; //copy of cmds at start of process

	//Get the current process log (can be run during/after the process runs)
	QJsonObject getProcLog();
	//Process Status
	bool isRunning();
	bool isDone();

public slots:
	void procReady(); //all the input arguments have been setup - and the proc is ready to be started
	void startProc();

private:
	QString cCmd, lognew;
	QJsonObject proclog;
	QTimer *uptimer;

private slots:
	void cmdError(QProcess::ProcessError);
	void cmdFinished(int, QProcess::ExitStatus);
	void updateLog(); //readyRead() signal
	void emitUpdate();

signals:
	void ProcFinished(QString, QJsonObject); // ID / log
	//Generic signals for subsystem usage (no direct proc access later)
	void ProcUpdate(QString, QJsonObject); // ID/log
};


class Dispatcher : public QObject{
	Q_OBJECT
public:
	enum PROC_QUEUE { NO_QUEUE = 0, PKG_QUEUE, IOCAGE_QUEUE };
#define enum_length 3 //This needs to be the number of items  in the enum above

	Dispatcher();
	~Dispatcher();

	QJsonObject listJobs();
	QJsonObject killJobs(QStringList ids);
	bool isJobActive(QString ID); //returns true if a job with this ID is running/pending

public slots:
	//Main start/stop
	void start(QString queuefile); //load any previously-unrun processes
	void stop(); //save any currently-unrun processes for next time

	//Main Calling Functions (single command, or multiple in-order commands)
	DProcess* queueProcess(QString ID, QString cmd, QString workdir = ""); //uses NO_QUEUE
	DProcess* queueProcess(QString ID, QStringList cmds, QString workdir = ""); //uses NO_QUEUE
	DProcess* queueProcess(Dispatcher::PROC_QUEUE, QString ID, QString cmd, QString workdir = "");
	DProcess* queueProcess(Dispatcher::PROC_QUEUE, QString ID, QStringList cmds, QString workdir = "");

private:
	// Queue file
	QString queue_file;

	//Internal lists
	QHash<PROC_QUEUE, QList<DProcess*> > HASH;

	//Simplification routine for setting up a process
	DProcess* createProcess(QString ID, QStringList cmds, QString workdir = "");
	QJsonObject CreateDispatcherEventNotification(QString, QJsonObject, bool);

	// Functions to do parsing out dispatcher queued tasks
	// Please keep these sorted
	void parseIohyveFetchOutput(QString outputLog, QJsonObject *out);

private slots:
	void mkProcs(Dispatcher::PROC_QUEUE, DProcess *P);
	void ProcFinished(QString ID, QJsonObject log);
	void ProcUpdated(QString ID, QJsonObject log);
	void CheckQueues();

signals:
	//Main signals
	void DispatchEvent(QJsonObject obj); //obj is the data associated with the process
	void DispatchStarting(QString ID);

	//Signals for private usage
	void mkprocs(Dispatcher::PROC_QUEUE, DProcess*);
	void checkProcs();

};

#endif
