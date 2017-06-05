//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

#include "sysadm-global.h"

using namespace sysadm;

#define PREFIX QString("/usr/local")
QString TRUEOS_ETCCONF(PREFIX + "/etc/trueos.conf");  // The default trueos.conf file

//=================
// RunCommand() variations
//=================
//Note: environment changes should be listed as such: <variable>=<value>

//Return CLI output (and success/failure)
QString General::RunCommand(bool &success, QString command, QStringList arguments, QString workdir, QStringList env){
  QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels); //need output
  //First setup the process environment as necessary
  QProcessEnvironment PE = QProcessEnvironment::systemEnvironment();
    if(!env.isEmpty()){
      for(int i=0; i<env.length(); i++){
    if(!env[i].contains("=")){ continue; }
        PE.insert(env[i].section("=",0,0), env[i].section("=",1,100));
      }
    }
    proc.setProcessEnvironment(PE);
  //if a working directory is specified, check it and use it
  if(!workdir.isEmpty()){
    proc.setWorkingDirectory(workdir);
  }
  //Now run the command (with any optional arguments)
  if(arguments.isEmpty()){ proc.start(command); }
  else{ proc.start(command, arguments); }
  //Wait for the process to finish (but don't block the event loop)
  while( !proc.waitForFinished(500) ){ QCoreApplication::processEvents(); }
  success = (proc.exitCode()==0); //return success/failure
  return QString(proc.readAllStandardOutput());
}

QString General::RunCommand(QString command, QStringList arguments, QString workdir, QStringList env){
  //Overloaded simplification for text-only output
  bool junk = false;
  return General::RunCommand(junk,command,arguments,workdir,env);
}

bool General::RunQuickCommand(QString command, QStringList arguments,QString workdir, QStringList env){
  //Overloaded simplification for success-only output
  bool success = false;
  General::RunCommand(success, command, arguments, workdir, env);
  return success;
}

QStringList General::gitCMD(QString dir, QString cmd, QStringList args){
  //Run a quick command in the proper dir and return the output
  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);
  if( !dir.isEmpty() && QFile::exists(dir) ){ proc.setWorkingDirectory(dir); }
  if(args.isEmpty()){ proc.start(cmd); }
  else{ proc.start(cmd, args); }
  while(!proc.waitForFinished(300)){ QCoreApplication::processEvents(); }
  QStringList out;
  while(proc.canReadLine()){
    out << QString( proc.readLine() );
  }
  return out;
}

//=================
// TEXT FILE INTERACTION
//=================
QStringList General::readTextFile(QString filepath){
  QStringList out;
  QFile file(filepath);
  if(file.open(QIODevice::Text | QIODevice::ReadOnly)){
    QTextStream in(&file);
    while(!in.atEnd()){
      out << in.readLine();
    }
    file.close();
  }
  return out;
}

bool General::writeTextFile(QString filepath, QStringList contents, bool overwrite){
  //Simplification for user directory
  if(filepath.startsWith("~")){ filepath = filepath.replace(0,1,QDir::homePath()); }
  QFile file(filepath);
  //Check existance/validity
  if(file.exists() && !overwrite){ return false; }
  if(!file.exists()){
    //See if we need to create the parent directory as well
    QDir dir = QFileInfo(filepath).dir(); //get the parent directory
    if(!dir.exists()){
      if(!dir.mkpath(dir.absolutePath())){ return false; } //could not create parent directory
    }
  }
  //Make sure that something is there to be written
  // - to delete a file, use QFile::remove() instead of this function
  if(contents.isEmpty()){ contents << "\n"; }
  else if(contents.last().isEmpty()){ contents << "\n"; }
  //Now save the contents to the file
  bool ok = false;
  if( file.open(QIODevice::WriteOnly | QIODevice::Truncate) ){
    QTextStream out(&file);
    out << contents.join("\n");
    file.close();
    ok = true;
  }
  return ok;
}

//== JsonArrayToStringList() ==
QStringList General::JsonArrayToStringList(QJsonArray array){
  //Note: This assumes that the array is only values, not additional objects
  QStringList out;
  for(int i=0; i<array.count(); i++){
    out << array.at(i).toString();
  }
  return out;  
}

//== getConfFileValue() ==
QString General::getConfFileValue(QString fileName, QString Key, int occur )
{
    int found = 1;

    QFile file( fileName );
    if ( ! file.open( QIODevice::ReadOnly ) ) {
        return QString();
    }

    QTextStream stream( &file );
    QString line;
    while ( !stream.atEnd() )
    {
        line = stream.readLine(); // line of text excluding '\n'
        line = line.section("#",0,0).trimmed(); //remove any comments
        if(line.isEmpty()){ continue; }
        int index = line.indexOf(Key, 0);
        //qDebug() << "Line:" << line << index;
        // If the KEY is not found at the start of the line, continue processing
        if(index!=0)
            continue;

        if ( found == occur) {
            line.remove(line.indexOf(Key, 0), Key.length());

            // Remove any quotes
            if ( line.indexOf('"') == 0 )
                line = line.remove(0, 1);

            if ( line.indexOf('"') != -1  )
                line.truncate(line.indexOf('"'));

            file.close();

            return line;
        } else {
           found++;
        }
    }

    file.close();
    return QString();
}
bool General::setConfFileValue(QString fileName, QString oldKey, QString newKey, int occur )
{
    // Lets the dev save a value into a specified config file.
    // The occur value tells which occurance of "oldKey" to replace
    // If occur is set to -1, it will remove any duplicates of "oldKey"

    //copy the original file to create a temporary file for editing
    QStringList args;
    QString oFileTmp = fileName + ".tmp";
    args << fileName << oFileTmp;
    General::RunCommand("cp",args);

    //Continue evaluating the temporary file
    QStringList SavedFile;
    int found = 1;

    // Load the old file, find the oldKey, remove it and replace with newKey
    QFile file( oFileTmp );
    if ( ! file.open( QIODevice::ReadOnly ) )
        return false;

    QTextStream stream( &file );
    QString line;
    while ( !stream.atEnd() ) {
        line = stream.readLine(); // line of text excluding '\n'

        // Key is not found at all
        if ( line.indexOf(oldKey, 0) == -1 ) {
            SavedFile << line ;
            continue;
        }

        // Found the key, but it is commented out, so don't worry about this line
        if ( line.trimmed().indexOf("#", 0) == 0 ) {
            SavedFile << line ;
            continue;
        }

        // If the KEY is found, and we are just on wrong occurance, save it and continue to search
        if ( occur != -1 && found != occur ) {
            SavedFile << line ;
            found++;
            continue;
        }

        // If the KEY is found in the line and this matches the occurance that must be processed
        if ( ! newKey.isEmpty() && found == occur )
        {
            SavedFile << newKey ;
            newKey.clear();
            found++;
            continue;
        }

        // If the KEY is found and we just want one occurance of the key
        if ( occur == -1 && ! newKey.isEmpty() ) {
            SavedFile << newKey ;
            newKey.clear();
            found++;
            continue;
        }

    }

    file.close();

    // Didn't find the key? Write it!
    if ( ! newKey.isEmpty() )
        SavedFile << newKey;


    // Save the new file
    QFile fileout( oFileTmp );
    if ( ! fileout.open( QIODevice::WriteOnly ) )
        return false;

    QTextStream streamout( &fileout );
    for (int i = 0; i < SavedFile.size(); ++i)
        streamout << SavedFile.at(i) << "\n";

    fileout.close();

    //Have the temporary file with new changes overwrite the original file
    args.clear();
    args << oFileTmp << fileName;
    General::RunCommand("mv",args);
    return true;
}

QString General::getValFromTrueOSConf(QString key) {
	return getValFromTOConf(TRUEOS_ETCCONF, key);
}

QString General::getValFromTOConf(QString conf, QString key) {
  
  // Load from conf the requested key
  QFile confFile(conf);
  if ( confFile.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &confFile );
        stream.setCodec("UTF-8");
        QString line;
        while ( !stream.atEnd() ) {
            line = stream.readLine().simplified();
	    if ( line.indexOf(key + ": ") == 0 ) {
   		confFile.close();
		return line.replace(key + ": ", "");
	    }

        }
   confFile.close();
   }
}

//===========================
//     SYSCTL ACCESS (might require root)
//===========================
//Retrieve a text-based sysctl
QString General::sysctl(QString var){
   char result[1000];
   size_t len = sizeof(result);
   if(0!=sysctlbyname(var.toLocal8Bit(), result, &len, NULL, 0)){ return ""; }
   result[len] = '\0';
   //qDebug() << "Sysctl:" << var << len << result;
   return QString(QByteArray(result,len));
}

//Retrieve a number-based sysctl
long long General::sysctlAsInt(QString var){
   long long result = 0;
   size_t len = sizeof(result);
   if(0!=sysctlbyname(var.toLocal8Bit(), &result, &len, NULL, 0) ){ return 0; }
   return result;
}

//===========================
//     Misc
//===========================

QString General::bytesToHumanReadable(long long bytes)
{
   float num = bytes;
   QStringList list;
   list << "KB" << "MB" << "GB" << "TB";

   QStringListIterator i(list);
   QString unit("bytes");

   while(num >= 1024.0 && i.hasNext())
   {
     unit = i.next();
     num /= 1024.0;
   }
   return QString().setNum(num,'f',2)+" "+unit;
}

void General::emptyDir(QString dir){
  QDir d(dir);
  if(!d.exists()){ return; } //quick check to make sure directory exists first
  //Remove all the files in this directory
  QStringList tmp = d.entryList(QDir::Files | QDir::NoDotAndDotDot);
  for(int i=0; i<tmp.length(); i++){
    d.remove(tmp[i]);
  }
  //Now remove all the directories in this directory (recursive)
  tmp = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for(int i=0; i<tmp.length(); i++){
    General::emptyDir(d.absoluteFilePath(tmp[i])); //Empty this directory first
    d.rmdir(tmp[i]); //Now try to remove it
  }
}
