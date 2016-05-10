// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_SERVER_REST_STRUCTS_H
#define _PCBSD_REST_SERVER_REST_STRUCTS_H
#include "globals-qt.h"

#define CurHttpVersion QString("HTTP/1.1")

//NOTE: The input structure parsing assumes a JSON input body
//NOTE: Common VERB's are:
/*	GET - Read a resource (no changes made)
	PUT - Insert/update a resource (makes changes - nothing automatically assigned for a new resource)
	POST - Insert/update a resource (makes changes - automatically assigns data for new resource as necessary)
	DELETE - Remove a resource (makes changes)
	OPTIONS - List the allowed options on a resource (no changes made)
	HEAD - List the response headers only (no changes made)
*/
class RestInputStruct{
public:
	//REST Variables
	QString VERB, URI, HTTPVERSION;
	//JSON input variables
	QString name, namesp, id, auth;
	QJsonValue args;
	QString bridgeID;

	//Raw Text
	QStringList Header; //REST Headers
	QString Body; //Everything else
	//User Permissions level
	bool fullaccess;

	RestInputStruct(QString message = "", bool isRest = false);
	~RestInputStruct();
		
	void ParseBodyIntoJson();

};

class RestOutputStruct{
public:
	enum ExitCode{OK, CREATED, ACCEPTED, NOCONTENT, RESETCONTENT, PARTIALCONTENT, PROCESSING, BADREQUEST, UNAUTHORIZED, FORBIDDEN, NOTFOUND };
	RestInputStruct in_struct;
	ExitCode CODE;
	QStringList Header; //REST output header lines
	QJsonValue out_args;
	
	RestOutputStruct(){
	  CODE = BADREQUEST; //default exit code
	}
	~RestOutputStruct(){}
		
	QString assembleMessage(); //normal operation - no special processing needed
};

#endif
