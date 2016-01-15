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

void UserManager::NewUser(QString fullName, QString userName, QString password, QString home, QString shell, int uid, int gid, bool encrypt)
{
    User user;
    user.UserName = userName;
    user.FullName = fullName;
    user.HomeFolder = (home.isEmpty())?"/usr/home/"+userName : home;
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
    if(!home.isEmpty())
    {
        args << "-d"; //set the home directory to
        args << home; //this
    }
    args << "-s"; //set the user's shell
    args << shell; //to this
    if(gid != -1)
    {
        args << "-g"; //set the group id to
        args << QString::number(gid); //this
    }
    if(uid != -1)
    {
        args << "-u"; //set the user id to
        args << QString::number(uid); //this
    }
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
        args << userName; //run command as this user
        args << "-c"; //with the command
        args << "\"flashpluginctl on\""; //turn on flashpluginctl
        General::RunCommand("su",args);
    }

    //if we're going to PersonaCrypt the home directory
    if(encrypt)
        initPCDevice(user,home,password);

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
    args << user.HomeFolder;
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

    loadUsers();
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
    //Don't Change the password of a user with an encrypted Home directory
    if( !QFile::exists("/var/db/personacrypt/"+user.UserName+".key") ){ return; }

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
        args << "chroot"; //a chroot
        args << chroot; //located here
    }
    args << "pw"; //change users
    args << "usermod"; //where we're going to modify a user
    args << user.UserName;//this user
    args << "-h"; //set the user's password
    args << "0"; //using stdin
    General::RunCommand("cat",args);

    //remove the temp file holding the password
    nfile.remove();

}

void UserManager::ChangeUserShell(User user, QString shell)
{
    if(shells.contains(shell))
    {
        qDebug("Shell found");
        QStringList args;
        args << "usermod"; // modify the user
        args << "-n"; //specify a user name
        args << user.UserName; //for this user
        args << "-s"; //set the shell to
        args << shell; //this shell
        General::RunCommand("pw",args);
    }
    else
        qDebug("Shell not found");

    loadUsers();
}

void UserManager::ChangeUserFullName(User user, QString newName)
{
    QStringList args;
    args << "usermod"; //modify the user
    args << user.UserName; //for this user
    args << "-c"; //change the gecos field to
    args << newName; //this name
    General::RunCommand("pw",args);
    loadUsers();
}

void UserManager::AddUserToGroup(User user, Group group)
{
    QStringList args;
    args << "groupmod"; //modify a group
    args << "-n"; //modify for a group
    args << group.Name;//this group
    args << "-m";//by adding a member
    args << user.UserName; //this user
    General::RunCommand("pw",args);

    loadGroups();
}

void UserManager::RemoveUserFromGroup(User user, Group group)
{
    QStringList args;
    args << "groupmod"; //modify a group
    args << "-n"; //modify for a group
    args << group.Name; //this group
    args << "-d"; //by removing a user
    args << user.UserName ; //this user
    General::RunCommand("pw", args);

    loadGroups();
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

    loadGroups();
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

    loadGroups();
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
    QStringList userStrings;
    QStringList args;
    if(!chroot.isEmpty())
    {
        args << chroot;
        args << "pw";
    }
    args << "usershow";
    args << "-a";
    if(chroot.isEmpty())
        userStrings = General::RunCommand("pw",args).split("\n");
    else
        userStrings = General::RunCommand("chroot",args).split("\n");

    //remove the empty string at the end
    userStrings.removeLast();

    for(QString line : userStrings)
    {
        User user;
        user.UserName = line.section(":",0,0);
        user.ID = line.section(":",2,2).toInt();
        user.GroupID = line.section(":",3,3).toInt();
        user.HomeFolder = line.section(":",8,8);
        user.Shell = line.section(":",9,9);
        user.FullName = line.section(":",7,7);

        users.append(user);
    }
}

void UserManager::loadGroups()
{
    groups.clear();
    QStringList groupStrings;
    QStringList args;
    if(!chroot.isEmpty())
    {
        args << chroot;
        args << "pw";
    }
    args << "groupshow";
    args << "-a";
    if(chroot.isEmpty())
        groupStrings = General::RunCommand("pw",args).split("\n");
    else
        groupStrings = General::RunCommand("chroot",args).split("\n");

    //remove the empty string at the end
    groupStrings.removeLast();

    for(QString line : groupStrings)
    {
        Group group;
        group.Name = line.section(":",0,0);
        group.ID = line.section(":",2,2).toInt();
        QString memberString = line.section(":",3,3);
        group.Members = memberString.split(",");

        groups.append(group);
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


void UserManager::importPCKey(User user, QString filename){
  //Double check that the key does not exist (button should have been hidden earlier if invalid)
  if( QFile::exists("/var/db/personacrypt/"+user.UserName+".key") ){ return; }

  //if the location is empty cancel
  if(filename.isEmpty()){ return; }

  //Now run the import command
  QStringList args;
  args << "import";
  args << "\""+filename + "\"";
  if( 0 == General::RunCommand("personacrypt",args) ){
    //Success
    qDebug("The key file was imported successfully.");
  }else{
    //Failure
    qWarning("The key file could not be imported. Please ensure you are using a valid file.");
  }
}

void UserManager::exportPCKey(User user, QString filename){
  //Double check that the key exists (button should have been hidden earlier if invalid)
  if( !QFile::exists("/var/db/personacrypt/"+user.UserName+".key") ){ return; }

  if(filename.isEmpty()){ return; } //cancelled
  if( !filename.endsWith(".key") ){ filename.append(".key"); }
  //Now get/save the key file
  QStringList args;
  args << "export";
  args << "\"" + user.UserName + "\"";
  QString key = General::RunCommand("personacrypt",args);

  QFile file(filename);
  if( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ){
    //Could not open output file
    qWarning() <<"Output file could not be opened:\n\n" << filename;
    return;
  }
  QTextStream out(&file);
  out << key;
  file.close();
  qDebug() << "The PersonaCrypt key has been saved successfully: \n\n" << filename;
}

void UserManager::disablePCKey(User user){
//Double check that the key exists (button should have been hidden earlier if invalid)
  if( !QFile::exists("/var/db/personacrypt/"+user.UserName+".key") ){ return; }

  if( QFile::remove("/var/db/personacrypt/"+user.UserName+".key") ){
    //Success
    qDebug("The PersonaCrypt user key has been disabled." );
  }else{
    //Failure (should almost never happen, since this utility runs as root and just needs to delete a file)
    qDebug("The PersonaCrypt user key could not be removed. Do you have the proper permissions?" );
  }
}

void UserManager::disableAndCopyPCKey(User user, QString password){
    QStringList args;
    args << "list";
    QStringList cusers = General::RunCommand("personacrypt",args).split("\n");
    bool available = false;
    for(int i=0; i<cusers.length(); i++){
        if(cusers[i].section(" on ",0,0) == user.UserName){ available = true; break; } //disk is connected to the system
    }
    if(!available){
        //Warn the user that they need to plug in their USB stick first
        qWarning("PersonaCrypt Device Not Found, Please ensure that your PersonaCrypt device is connected to the system and try again.");
        return;
    }

    if(password.isEmpty()){ return; } //cancelled
    //Save the password to a temporary file
    QTemporaryFile tmpfile("/tmp/.XXXXXXXXXXXXXXXXXXXX");
    if( !tmpfile.open() ){ return; } //could not create a temporary file (extremely rare)
    QTextStream out(&tmpfile);
    out << password;
    tmpfile.close();

    //Now run the PersonaCrypt command
    args.clear();
    args << "remove";
    args << "\"" + user.UserName + "\"";
    args << "\"" + tmpfile.fileName() + "\"";
    if(0 == General::RunCommand("personacrypt",args) ){
        //Success
        qDebug("Success; The data for this user has been merged onto the system and the system key has been disabled");
    }else{
        //Failure
        qWarning("Failure; The PersonaCrypt user data could not be merged onto the system. Invalid Password?" );
    }
}

void UserManager::initPCDevice(User user, QString home, QString password)
{
    //Double check that the key does not exist (button should have been hidden earlier if invalid)
    if( QFile::exists("/var/db/personacrypt/" + user.UserName + ".key") ){ return; }

    //Prompt for the user to select a device
    QStringList args;
    args << "list";
    args << "-r";
    QStringList devlist = General::RunCommand("personacrypt",args).split("\n");
    for(int i=0; i<devlist.length(); i++){
        //qDebug() << "Devlist:" << devlist[i];
        if(devlist[i].isEmpty() || devlist[i].startsWith("gpart:"))
        {
          devlist.removeAt(i);
          i--;
        }
    }
    if(devlist.isEmpty() || devlist.join("").simplified().isEmpty()){
        qWarning("No Devices Found; Please connect a removable device and try again");
        return;
    }

    args.clear();
    args << "-h";
    args << user.HomeFolder;
    bool ok = false;
    QString space = General::RunCommand("df -h "+home).split("\n").filter(home).join("");
    space.replace("\t"," ");
    space = space.section(" ",2,2,QString::SectionSkipEmpty);

    if(!ok || home.isEmpty()){ return; }

    home = home.section(":",0,0); //only need the raw device
    //Save the password to a temporary file (for input to personacrypt)
    QTemporaryFile tmpfile("/tmp/.XXXXXXXXXXXXXXX");
    if(!tmpfile.open()){
        //Error: could not open a temporary file
        qWarning("Error; Could not create a temporary file for personacrypt");
        return;
    }
    QTextStream out(&tmpfile);
    out << password;
    tmpfile.close();
    //Now start the process of setting up the device
    bool success = false;
    args.clear();
    args << "init";
    args << "\""+user.UserName + "\"";
    args << "\"" + tmpfile.fileName() + "\"";
    args << home;
    QStringList output = General::RunCommand(success,"personacrypt",args).split("\n");
    if(success){
        //Success
        qDebug("Success; The PersonaCrypt device was successfully initialized");
    }else{
        //Failure - make sure the key was not created before the failure
        if(QFile::exists("/var/db/personacrypt/"+user.UserName+".key")){
            QFile::remove("/var/db/personacrypt/"+user.UserName+".key");
        }
        //Now show the error message with the log
        qWarning("Failure; The PersonaCrypt device could not be initialized");
  }
}
