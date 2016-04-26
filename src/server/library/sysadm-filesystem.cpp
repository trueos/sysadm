//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-filesystem.h"
#include "sysadm-general.h"

using namespace sysadm;

QJsonObject FS::list_dir(QJsonObject jsin) {
   QJsonObject retObject;
   QString fsdir;

   QStringList keys = jsin.keys();
   if(! keys.contains("dir")){
     retObject.insert("error", "Requires dir key");
     return retObject;
   }

   fsdir = jsin.value("dir").toString();

   if ( fsdir.isEmpty() ) {
     retObject.insert("error", "Empty dir name");
     return retObject;
   }

   // TODO
   //
   // Add some sort of check here to ensure the connected user has
   // perms to read / list this directory

   QString tmp;
   QDir dir(fsdir);
   QFileInfoList list = dir.entryInfoList();
   for (long i = 0; i < list.size(); ++i) {
     QFileInfo fileInfo = list.at(i);
     tmp = fileInfo.fileName();
     if ( tmp == "." || tmp == ".." )
       continue;

     QJsonObject dsetvals;
     if ( fileInfo.isDir() ) {
       dsetvals.insert("dir", true );
     } else if (fileInfo.isSymLink() ) {
       dsetvals.insert("symlink", true );
       dsetvals.insert("target", fileInfo.symLinkTarget() );
     } else {
       dsetvals.insert("size", fileInfo.size() );
       dsetvals.insert("owner", fileInfo.owner() );
       dsetvals.insert("group", fileInfo.group() );
     }
     retObject.insert(tmp, dsetvals);
   }
   return retObject;
}
