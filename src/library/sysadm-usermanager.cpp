//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-usermanager.h"
using namespace sysadm;

UserManager::UserManager(QString chroot)
{
    loadUsers();
    loadGroups();
    loadShells();
}

void UserManager::NewUser(QString fullName, QString userName, QString password, QString home, QString shell, int gid, int uid)
{
    if (home == "/usr/home/")
        home += userName;
    //Add User
    qDebug() << "Adding user " << userName;
    // Create the new home-directory
    if ( chroot.isEmpty() )
    {
        system("/usr/local/share/pcbsd/scripts/mkzfsdir.sh " + home.toLatin1() );
        system("pw groupadd " + userName.toLatin1() );
    } else {
        system("mkdir -p " + chroot.toLatin1() + "/" + home.toLatin1() + " 2>/dev/null" );
        system("chroot " + chroot.toLatin1() + " ln -s /usr/home /home 2>/dev/null" );
        system("chroot " + chroot.toLatin1() + " pw groupadd " + userName.toLatin1() );
    }

    QStringList args;
    if ( ! chroot.isEmpty() )
        args << chroot << "pw";
    args << "useradd";
    args << userName;
    args << "-c";
    args << fullName;
    args << "-m";
    args << "-d";
    args << home;
    args << "-s";
    args << shell;
    if (gid != -1)
    {
        args << "-g";
        args << QString::number(gid);
    } else {
        args << "-g";
        args << userName;
    }
    args << "-G";
    args << "operator";
    if( uid != -1)
    {
        args << "-u";
        args << QString::number( uid );
    }
    if ( ! chroot.isEmpty() )
        QProcess::execute("chroot", args);
    else
        QProcess::execute("pw", args);

    QTemporaryFile nfile("/tmp/.XXXXXXXX");
    if ( nfile.open() )
    {
        QTextStream stream( &nfile );
        stream << password;
        nfile.close();
    }
    if ( ! chroot.isEmpty() )
        system("cat " + nfile.fileName().toLatin1() + " | chroot " + chroot.toLatin1() + " pw usermod " + userName.toLatin1() + " -h 0 ");
    else
        system("cat " + nfile.fileName().toLatin1() + " | pw usermod " + userName.toLatin1() + " -h 0 ");
    nfile.remove();

    if ( chroot.isEmpty() ) {
        qDebug() << "Enabling Flash Plugin for " << userName;
        QString flashCmd = "su " + userName + " -c \"flashpluginctl on\"";
        system(flashCmd.toLatin1());
    }

    // Set permissions
    if ( chroot.isEmpty() )
        system("chown -R " + userName.toLatin1() +":" + userName.toLatin1() + " "  + home.toLatin1() );
    else
        system("chroot " + chroot.toLatin1() + " chown -R " + userName.toLatin1() +":" + userName.toLatin1() + " "  + home.toLatin1() );

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
    qDebug() << "Changing password: " << user.UserName;

    // Set the new PW
    QTemporaryFile rfile("/tmp/.XXXXXXXX");
    if ( rfile.open() ) {
    QTextStream stream( &rfile );
    stream << newPassword;
    rfile.close();
    }
    if ( ! chroot.isEmpty() )
        system("cat " + rfile.fileName().toLatin1() + " | chroot " + chroot.toLatin1() + " pw usermod " + user.UserName.toLatin1() + " -h 0 ");
    else
        system("cat " + rfile.fileName().toLatin1() + " | pw usermod " + user.UserName.toLatin1() + " -h 0 ");

    rfile.remove();

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
