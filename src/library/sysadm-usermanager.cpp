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

void UserManager::NewUser(QString fullName, QString userName, QString password, QString home, QString shell, int gid, int uid)
{
    User user;
    user.UserName = userName;
    user.FullName = fullName;
    user.HomeFolder = home;
    user.Shell = shell;
    user.ID = uid;

    if (home == "/usr/home/")
        home += userName;
    //Add User
    qDebug() << "Adding user " << userName;
    // Create the new home-directory
    if ( chroot.isEmpty() )
    {
        //create the home directory and a zfs dataset for it
        QStringList args;
        args.append(home);
        General::RunCommand("/usr/local/share/pcbsd/scripts/mkzfsdir.sh",args);

        //create the group associated with the user
        args.clear();
        args.append(userName);
        General::RunCommand("pw groupadd",args);
    } else {
        //make the home directory
        QStringList args;
        args.append("-p"); // create intermediate directories as needed
        args.append(chroot); //chroot location
        args.append(home); //directory to create, in this case home
        args.append("2>/dev/null"); //send stderr to /dev/null
        General::RunCommand("mkdir",args);

        //link /usr/home to /home
        args.clear();
        args.append(chroot); //location the chroot is in
        args.append("ln"); //link
        args.append("-s"); //symbolic link
        args.append("/usr/home"); //folder to link from
        args.append("/home"); //The link to create
        args.append("2>/dev/null"); //send stderr to /dev/null
        General::RunCommand("chroot",args);

        //create the group associated with the user
        args.clear();
        args.append(chroot); //location the chroot is in
        args.append("pw groupadd"); //create a group
        args.append(userName); //name of the group to create
        General::RunCommand("chroot",args);
    }

    QStringList args;
    if ( ! chroot.isEmpty() ) //if chroot is not empty the command starts with chroot instead of pw
        args << chroot << "pw"; //and thus we have to add it as an argument
    args << "useradd"; //create a user
    args << userName; //with this userName
    args << "-c"; //sets the comment field
    args << fullName; //with the full name of the user
    args << "-m"; //create the user's home directory
    args << "-d"; //set the path of the user's home directory
    args << home; //to here
    args << "-s"; //set the user's shell
    args << shell; //to this
    if (gid != -1) //if a group id was specified
    {
        args << "-g"; //set the user's group to
        args << QString::number(gid); // this
    } else { //if a group id wasn't set
        args << "-g"; //set the group
        args << userName; //to a group with the same name as the user
    }
    args << "-G"; //additionally add the user to
    args << "operator"; //the operator's group
    if( uid != -1) //if the user set a UID
    {
        args << "-u"; //set the UID
        args << QString::number( uid ); //to this
    }
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

    // Set permissions
    if ( chroot.isEmpty() ) //if we're not in a chroot
    {
        //give the user ownership of their home directory
        args.append("-R"); //Recursive, change the ownership of both folders and files
        args.append(userName+":"+userName); //to the user
        args.append(home); //their home directory
        General::RunCommand("chown",args);
    } else { //if we are in a chroot
        //give the user ownership of their home directory
        args.append(chroot); //the chroot location
        args.append("chown"); //change ownership of
        args.append("-R"); //Recursive, change the ownership of both folders and files
        args.append(userName + ":" + userName); //to the user
        args.append(home); //their home directory
        General::RunCommand("chroot",args);
    }

    //reloads the groups and users so that the internal model is consistent
    loadUsers();
    loadGroups();
}

void UserManager::DeleteUser(User user)
{
    //Delete User
    qDebug() << "Deleting user " << user.UserName;

    /*if(userIt->getEnc())
    {
        // Unmount PEFS
        system("umount " + userIt->getHome().toLatin1() );
    }*/
    QStringList args;
    if ( ! chroot.isEmpty() )
        args << chroot << "pw";
    args << "userdel";
    args << user.UserName;
    args << "-r";
    system("/usr/local/share/pcbsd/scripts/rmzfsdir.sh " + user.HomeFolder.toLatin1() );

    if ( ! chroot.isEmpty() )
        QProcess::execute("chroot", args);
    else
        QProcess::execute("pw", args);

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

    if ( ! chroot.isEmpty() ) //if we're in a chroot
    {
        //set the user password
        QStringList args;
        args.append(nfile.fileName()); //the temp file holding the password
        args.append("|"); //which we're going to pipe to the stdin of
        args.append("chroot"); //a chroot
        args.append(chroot); //located here
        args.append("pw usermod"); //where we're going to modify a user
        args.append(user.UserName);//this user
        args.append("-h"); //set the user's password
        args.append("0"); //using stdin
        General::RunCommand("cat",args);
    }
    else
    {
        QStringList args;
        args.append(nfile.fileName()); //the temp file holding the password
        args.append("|"); //which we're going to pipe to the stdin of
        args.append("pw usermod"); //and we're going to modify the user account of
        args.append(user.UserName); //this user
        args.append("-h"); //and we're going to set their password
        args.append("0"); //using stdin
        General::RunCommand("cat",args);
    }
    //remove the temp file holding the password
    nfile.remove();

}

void UserManager::ChangeUserShell(User user, QString shell)
{
    if(shells.contains(shell))
        system("chsh -s " + shell.toLocal8Bit() + " " + user.UserName.toLocal8Bit());
    else
        qDebug("Shell not found");
}

void UserManager::ChangeUserFullName(User user, QString newName)
{
    system("pw usermod " + user.UserName.toLatin1() + " -c " + newName.toLatin1());
}

void UserManager::AddUserToGroup(User user, Group group)
{
    system("pw groupmod "+ group.Name.toLatin1() + " -m " + user.UserName.toLatin1());
}

void UserManager::RemoveUserFromGroup(User user, Group group)
{
    system("pw groupmod"+ group.Name.toLatin1() +"-d" + user.UserName.toLatin1());
}

void UserManager::NewGroup(QString name, QStringList members)
{
    QStringList args;
    qDebug() << "Adding group " << name;
    if ( ! chroot.isEmpty() )
        args << chroot << "pw";
    args << "groupadd";
    args << name;
    args << "-M";
    args << members.join(",");
    if ( ! chroot.isEmpty() )
        QProcess::execute("chroot", args);
    else
        QProcess::execute("pw", args);

}

void UserManager::DeleteGroup(Group group)
{
    QStringList args;
    qDebug() << "Deleting group " << group.Name;
    if ( ! chroot.isEmpty() )
        args << chroot << "pw";
    args << "groupdel";
    args << group.Name;
    if ( ! chroot.isEmpty() )
        QProcess::execute("chroot", args);
    else
        QProcess::execute("pw", args);
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
