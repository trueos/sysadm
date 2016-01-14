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
    User()
    {
        FullName = "";
        UserName = "";
        ID = -1;
        HomeFolder = "";
        Shell = "";
        GroupID = -1;
        Encrypted = false;
    }

    QString FullName;
    QString UserName;
    int ID;
    QString HomeFolder;
    QString Shell;
    int GroupID;
    bool Encrypted;
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
    Group()
    {
        ID = -1;
        Name = "";
        Members = QStringList();
    }

    int ID;
    QString Name;
    //While the object model would be more "correct" if
    //Users were to be a Vector of User pointers, it's
    //expensive to wire up and we don't really gain anything
    //from doing so
    QStringList Members;
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
     * @param shell the user's shell, defaults to /bin/tcsh
     * @param uid the user id of the user
     * @param gid the group id of the user
     */
    void NewUser(QString fullName, QString userName, QString password, QString shell = "/bin/tcsh", int uid = -1, int gid = -1);
    /**
     * @brief DeleteUser Deletes a user
     * @param user the user to delete
     */
    void DeleteUser(User user);

    /**
     * @brief GetUsers getter for the users vector
     * @return a QVector<Users> that is a copy of the current state
     * do not modify it, instead call functions on this class to change
     * things and then get another copy of the vector
     */
    const QVector<User> GetUsers();
    /**
     * @brief GetUser get a particular user by their UID
     * @param id the UID of the user to get
     * @return the user with the UID specified, if not found
     * returns a blank User
     */
    const User GetUser(int id);

    /**
     * @brief GetUser get a particular user by their UID
     * @param userName the username of the user to get
     * @return the user with the user name specified, if not found
     * returns a blank User
     */
    const User GetUser(QString userName);

    /**
     * @brief ChangeUserPassword changes the specified user's password
     * @param user the user to change the password of
     * @param newPassword the new password
     */
    void ChangeUserPassword(User user, QString newPassword);
    /**
     * @brief ChangeUserShell change a specified user's shell
     * @param user the user to change the shell for
     * @param shell the shell to change to, note that if the shell
     * is not in the shells list then it does nothing
     */
    void ChangeUserShell(User user, QString shell);
    /**
     * @brief ChangeUserFullName change the gecos field of a user to a new name
     * @param user the user to change the name of
     * @param newName the name to change to
     */
    void ChangeUserFullName(User user, QString newName);
    //#endsection

    //#section group actions
    /**
     * @brief AddUserToGroup add the specified user to the specified group
     * @param user the user to add to the group
     * @param group the group to add the user to
     */
    void AddUserToGroup(User user, Group group);
    /**
     * @brief RemoveUserFromGroup removes the specified user from the specified group
     * @param user the user to remove from the group
     * @param group the group to remove the user from
     */
    void RemoveUserFromGroup(User user, Group group);

    /**
     * @brief NewGroup creates a new group
     * @param name the name of the new group
     * @param Users a list of users to add to the group
     */
    void NewGroup(QString name, QStringList Users = QStringList());
    /**
     * @brief DeleteGroup delete a specified group
     * @param group the group to delete
     */
    void DeleteGroup(Group group);

    /**
     * @brief GetGroups get the internal list of groups
     * @return a QVector<Group> that is a copy of the current state
     * do not modify it, instead call functions on this class to change
     * things and then get another copy of the vector
     */
    const QVector<Group> GetGroups();
    /**
     * @brief getGroup get a specified group by their gid
     * @param id the gid of the group to get
     * @return the group with the specified gid
     */
    const Group getGroup(int id);
    /**
     * @brief getGroup get a specified group by their name
     * @param name the name of the group to get
     * @return the group with the specified name
     */
    const Group getGroup(QString name);
    //#endsection

    /**
     * @brief GetShells the list of shells that are currently installed on the system
     * @return a QStringList of shells on the system
     */
    const QStringList GetShells();

private:
    QVector<User> users;
    QVector<Group> groups;
    QStringList shells;
    QString chroot;

    //loads the users from /etc/passwd
    void loadUsers();
    //load the groups from /etc/group
    void loadGroups();
    //load the shells from /etc/shells
    void loadShells();
};
}
#endif // USERMANAGER_H
