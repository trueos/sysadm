//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================

#ifndef USERMANAGER_H
#define USERMANAGER_H
#include<QtCore>

namespace sysadm{
class User
{
public:
    QString FullName;
    QString UserName;
    int ID;
    QString HomeFolder;
    QString Shell;
    int GroupID;
    friend bool operator<(const User lhs, const User rhs){
        return std::tie(lhs.ID,lhs.UserName) < std::tie(rhs.ID,rhs.UserName);
    }
    friend bool operator>(const User lhs, const User rhs)
    {   return rhs < lhs;}
    friend bool operator==(const User lhs, const User rhs)
    {
        return lhs.ID == rhs.ID && lhs.UserName == rhs.UserName;
    }
    friend bool operator !=(const User lhs, const User rhs)
    {   return !(lhs == rhs);}
};
class Group
{
public:
    int ID;
    QString Name;
    //While the object model would be more "correct" if
    //Users were to be a Vector of User pointers, it's
    //expensive to wire up and we don't really gain anything
    //from doing so
    QStringList Users;
};

class UserManager
{
public:    
    UserManager(QString chroot = "");

    //#section user actions
    /**
     * @brief NewUser Create a new user
     * @param fullName The full name of the user
     * @param userName The username of the user
     * @param password The user's password
     * @param home the user's home directory, defaults to /usr/home/$userName
     * @param shell the user's shell, defaults to /bin/tcsh
     * @param gid the gid of the user, defaults to -1, which means to leave it up to autogen
     * @param uid the uid of the user, defaults to -1, which means to leave it up to autogen
     */
    void NewUser(QString fullName, QString userName, QString password, QString shell = "/bin/tcsh");
    void DeleteUser(User user);

    const QVector<User> GetUsers();
    const User GetUser(int id);
    const User GetUser(QString userName);

    void ChangeUserPassword(User user, QString newPassword);
    void ChangeUserShell(User user, QString shell);
    void ChangeUserFullName(User user, QString newName);
    //#endsection

    //#section group actions
    void AddUserToGroup(User user, Group group);
    void RemoveUserFromGroup(User user, Group group);

    void NewGroup(QString name, QStringList Users = QStringList());
    void DeleteGroup(Group group);

    const QVector<Group> GetGroups();
    const Group getGroup(int id);
    const Group getGroup(QString name);
    //#endsection

    const QStringList GetShells();

private:
    QVector<User> users;
    QVector<Group> groups;
    QStringList shells;
    QString chroot;

    void loadUsers();
    void loadGroups();
    void loadShells();
};
}
#endif // USERMANAGER_H
