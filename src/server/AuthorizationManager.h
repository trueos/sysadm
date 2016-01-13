// ===============================
//  PC-BSD REST/JSON API Server
// Available under the 3-clause BSD License
// Written by: Ken Moore <ken@pcbsd.org> July 2015
// =================================
#ifndef _PCBSD_REST_AUTHORIZATION_MANAGER_H
#define _PCBSD_REST_AUTHORIZATION_MANAGER_H

#include <QHash>
#include <QString>
#include <QStringList>
#include <QDateTime>

class AuthorizationManager{
public:
	AuthorizationManager();
	~AuthorizationManager();

	// == Token Interaction functions ==
	void clearAuth(QString token); //clear an authorization token
	bool checkAuth(QString token); //see if the given token is valid
	bool hasFullAccess(QString token); //see if the token is associated with a full-access account

	int checkAuthTimeoutSecs(QString token); //Return the number of seconds that a token is valid for

	// == Token Generation functions
	QString LoginUP(bool localhost, QString user, QString pass); //Login w/ username & password
	QString LoginService(bool localhost, QString service); //Login a particular automated service

private:
	QHash<QString, QDateTime> HASH;
	QString generateNewToken(bool isOperator);
	QStringList getUserGroups(QString user);

	//token->hashID filter simplification
	QString hashID(QString token){
	  QStringList tmp = QStringList(HASH.keys()).filter(token+"::::");
	  if(tmp.isEmpty()){ return ""; }
	  else{ return tmp.first(); }
	}
	
	//PAM login/check files
	bool pam_checkPW(QString user, QString pass);
	void pam_logFailure(int ret);
};

#endif
