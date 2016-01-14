//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-usermanager.h"
#include "sysadm-general.h"
using namespace sysadm;

UserManager::UserManager(QString chroot)
{
    this->chroot = chroot;
    loadUsers();
    loadGroups();
    loadShells();
}

void UserManager::NewUser(QString fullName, QString userName, QString password, QString shell)
{
    User user;
    user.UserName = userName;
    user.FullName = fullName;
    user.HomeFolder = "/usr/home/"+userName;
    user.Shell = shell;

    //Add User
    qDebug() << "Adding user " << userName;
    // Create the zfs dataset associated with the home directory
    if ( chroot.isEmpty() )
    {
        QStringList args;
        args.append(user.HomeFolder);
        General::RunCommand("/usr/local/share/pcbsd/scripts/mkzfsdir.sh",args);
    }

    QStringList args;
    if ( ! chroot.isEmpty() ) //if chroot is not empty the command starts with chroot instead of pw
        args << chroot << "pw"; //and thus we have to add it as an argument
    args << "useradd"; //create a user
    args << userName; //with this userName
    args << "-c"; //sets the comment field
    args << "\""+ fullName+"\""; //with the full name of the user
    args << "-m"; //create the user's home directory
    args << "-s"; //set the user's shell
    args << shell; //to this
    args << "-G"; //additionally add the user to
    args << "operator"; //the operator's group

    if ( ! chroot.isEmpty() ) //if we're operating with a chroot call
        General::RunCommand("chroot", args);
    else //otherwise
        General::RunCommand("pw", args);

    ChangeUserPassword(user,password);

    //enable flash for the user
    if ( chroot.isEmpty() ) { //if we're not in a chroot
        qDebug() << "Enabling Flash Plugin for " << userName;
        args.clear();
        args.append(userName); //run command as this user
        args.append("-c"); //with the command
        args.append("\"flashpluginctl on\""); //turn on flashpluginctl
        General::RunCommand("su",args);
    }

    //reloads the groups and users so that the internal model is consistent
    loadUsers();
    loadGroups();
}

void UserManager::DeleteUser(User user)
{
    //Delete User
    qDebug() << "Deleting user " << user.UserName;

    //remove the dataset associated with the home folder
    QStringList args;
    args.append(user.HomeFolder);
    General::RunCommand("/usr/local/share/pcbsd/scripts/rmzfsdir.sh",args);

    //delete the user and their home directory
    args.clear();
    if ( ! chroot.isEmpty() ) //if we're in a chroot we need to use chroot before pw
        args << chroot << "pw";
    args << "userdel"; //delete a user
    args << user.UserName; //this user
    args << "-r"; //remove the contents of the user's home directory
    if ( ! chroot.isEmpty() )
        General::RunCommand("chroot", args);
    else
        General::RunCommand("pw", args);

    //update the internal model
    users.removeAll(user);
    loadGroups();
}

const QVector<User> UserManager::GetUsers()
{
    return users;
}

const User UserManager::GetUser(int id)
{
    for(User user: users)
    {
        if(user.ID == id)
            return user;
    }
    return User();
}

const User UserManager::GetUser(QString userName)
{
    for(User user: users)
    {
        if(user.UserName == userName)
            return user;
    }
    return User();
}

void UserManager::ChangeUserPassword(User user, QString newPassword)
{

    //Create a temporary file to store the password in
    QTemporaryFile nfile("/tmp/.XXXXXXXX");
    if ( nfile.open() )
    {
        QTextStream stream( &nfile );
        stream << newPassword;
        nfile.close();
    }

    //set the user password
    QStringList args;
    args.append(nfile.fileName()); //the temp file holding the password
    args.append("|"); //which we're going to pipe to the stdin of
    if ( ! chroot.isEmpty() ) //if we're in a chroot
    {
        args.append("chroot"); //a chroot
        args.append(chroot); //located here
    }
    args.append("pw usermod"); //where we're going to modify a user
    args.append(user.UserName);//this user
    args.append("-h"); //set the user's password
    args.append("0"); //using stdin
    General::RunCommand("cat",args);

    //remove the temp file holding the password
    nfile.remove();

}

void UserManager::ChangeUserShell(User user, QString shell)
{
    if(shells.contains(shell))
    {
        QStringList args;
        args.append("-s"); //set the shell to
        args.append(shell); //this shell
        args.append(user.UserName); //for this user
        General::RunCommand("chsh",args);
    }
    else
        qDebug("Shell not found");
}

void UserManager::ChangeUserFullName(User user, QString newName)
{
    QStringList args;
    args.append(user.UserName); //for this user
    args.append("-c"); //change the gecos field to
    args.append(newName); //this name
    General::RunCommand("pw usermod",args);
}

void UserManager::AddUserToGroup(User user, Group group)
{
    QStringList args;
    args.append(group.Name);//modify this group
    args.append("-m");//by adding a member
    args.append(user.UserName); //this user
    General::RunCommand("pw groupmod",args);

    group.Users.append(user.UserName);
}

void UserManager::RemoveUserFromGroup(User user, Group group)
{
    QStringList args;
    args.append(group.Name); //modify this group
    args.append("-d"); //by removing a user
    args.append(user.UserName); //this user
    General::RunCommand("pw groupmod", args);

    group.Users.removeAll(user.UserName);
}

void UserManager::NewGroup(QString name, QStringList members)
{
    QStringList args;
    qDebug() << "Adding group " << name;
    if ( ! chroot.isEmpty() ) //if we're in a chroot need to add chroot before pw
        args << chroot << "pw";
    args << "groupadd"; //create a new group
    args << name; // with this name
    args << "-M"; //with this list of users
    args << members.join(","); //these guys
    if ( ! chroot.isEmpty() ) //if we're in a chroot
        General::RunCommand("chroot", args);
    else
        General::RunCommand("pw", args);

    LoadGroups();
}

void UserManager::DeleteGroup(Group group)
{
    QStringList args;
    qDebug() << "Deleting group " << group.Name;
    if ( ! chroot.isEmpty() ) //if we're in a chroot need to add chroot before pw
        args << chroot << "pw";
    args << "groupdel"; //delete a group
    args << group.Name; //of this name
    if ( ! chroot.isEmpty() ) //if we're in a chroot
        General::RunCommand("chroot", args);
    else
        General::RunCommand("pw", args);

    LoadGroups();
}

const QVector<Group> UserManager::GetGroups()
{
    return groups;
}

const Group UserManager::getGroup(int id)
{
    for(Group group : groups)
    {
        if(group.ID == id)
            return group;
    }
    return Group();
}

const Group UserManager::getGroup(QString name)
{
    for(Group group : groups)
    {
        if(group.Name == name)
            return group;
    }
    return Group();
}

const QStringList UserManager::GetShells()
{
    return shells;
}

void UserManager::loadUsers()
{
    users.clear();
    QFile userFile(chroot + "/etc/passwd");
    if ( userFile.open(QIODevice::ReadOnly) )
    {
        QTextStream stream(&userFile);
        stream.setCodec("UTF-8");
        QString line;

        while ( !stream.atEnd() )
        {
            line = stream.readLine();

            if ((line.indexOf("#") != 0) && (! line.isEmpty())) //Make sure it isn't a comment or blank
            {
                User user;
                user.UserName = line.section(":",0,0);
                user.ID = line.section(":",2,2).toInt();
                user.GroupID = line.section(":",3,3).toInt();
                user.HomeFolder = line.section(":",5,5);
                user.Shell = line.section(":",6,6);
                user.FullName = line.section(":",4,4);

                users.append(user);
            }
        }
    } else {
    //Unable to open file error
    qWarning("Error! Unable to open /etc/passwd");
    }
}

void UserManager::loadGroups()
{
    groups.clear();
    QFile groupFile(chroot + "/etc/group");
    if ( groupFile.open(QIODevice::ReadOnly) )
    {
        QTextStream stream(&groupFile);
        stream.setCodec("UTF-8");

        QString line;

        while ( !stream.atEnd() )
        {
            line = stream.readLine();

            if ((line.indexOf("#") != 0) && (! line.isEmpty())) //Make sure it isn't a comment or blank
            {
                Group group;
                group.Name = line.section(":",0,0);
                group.ID = line.section(":",2,2).toInt();
                QString memberString = line.section(":",3,3);
                group.Users = memberString.split(",");

                groups.append(group);
            }
        }
    } else {
    //Unable to open file error
    qWarning("Error! Unable to open /etc/group");
    }
}

void UserManager::loadShells()
{
    shells.clear();
    QFile shellFile(chroot + "/etc/shells");
    if ( shellFile.open(QIODevice::ReadOnly) ) {
        QTextStream stream(&shellFile);
        stream.setCodec("UTF-8");

        QString line;

        while ( !stream.atEnd() ) {
            line = stream.readLine();

            if ( !line.startsWith("#") && !line.isEmpty() ) { //Make sure it isn't a comment or blank
                shells.append(line);
            }
        }
    } else {
    //Unable to open file error
    qWarning("Error! Unable to open /etc/shells");
    }

    // Add /sbin/nologin as well
    shells.append("/sbin/nologin");
}
