//===========================================
//  PC-BSD source code
//  Copyright (c) 2015, PC-BSD Software/iXsystems
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "sysadm-general.h"
#include "sysadm-iocage.h"
#include "sysadm-global.h"

using namespace sysadm;

//PLEASE: Keep the functions in the same order as listed in pcbsd-general.h

// Destroy a jail on the box
QJsonObject Iocage::destroyJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QStringList output;

  output = General::RunCommand("iocage destroy -f " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

      vals.insert(key, value);
    }
  }

  retObject.insert("success", vals);

  return retObject;
}

// Create a jail on the box
QJsonObject Iocage::createJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();

  // Get the key values
  QString switches = jsin.value("switches").toString();
  QString props = jsin.value("props").toString();
  QStringList output;

  if ( keys.contains("switches" ) ) {
    output = General::RunCommand("iocage create " + switches + " " + props).split("\n");
  } else {
    output = General::RunCommand("iocage create " + props).split("\n");
  }

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

      if ( keys.contains("switches" ) ) {
        vals.insert("uuid", key);
      } else {
        vals.insert(key, value);
      }
    }
  }

  retObject.insert("switches", switches);
  retObject.insert("props", props);
  retObject.insert("success", vals);

  return retObject;
}

// Clone a jail on the box
QJsonObject Iocage::cloneJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QString props = jsin.value("props").toString();

  QStringList output = General::RunCommand("iocage clone " + jail + " " + props).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      return retObject;
    } else {
      QString key = output.at(i).simplified().section(":", 0, 0);
      QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
    }
  }

  retObject.insert("jail", jail);
  retObject.insert("props", props);
  retObject.insert("success", vals);

  return retObject;
}

// Clean everything iocage related on a box
QJsonObject Iocage::cleanAll() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage clean -fa ").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    // This means either a mount is stuck or a jail cannot be stopped,
    // give them the error.
    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      break;
    } else {
      // iocage does a spinner for these that is distracting to see returned,
      // returning what a user wants to actually see.
      retObject.insert("success", "All iocage datasets have been cleaned.");
    }
  }

  return retObject;
}

// Clean all templates on a box
QJsonObject Iocage::cleanTemplates() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage clean -ft ").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    // This means either a mount is stuck or a jail cannot be stopped,
    // give them the error.
    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      break;
    } else {
      // iocage does a spinner for these that is distracting to see returned,
      // returning what a user wants to actually see.
      retObject.insert("success", "All templates have been cleaned.");
    }
  }

  return retObject;
}

// Clean all RELEASEs on a box
QJsonObject Iocage::cleanReleases() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage clean -fr ").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    // This means either a mount is stuck or a jail cannot be stopped,
    // give them the error.
    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      break;
    } else {
      // iocage does a spinner for these that is distracting to see returned,
      // returning what a user wants to actually see.
      retObject.insert("success", "All RELEASEs have been cleaned.");
    }
  }

  return retObject;
}

// Clean all jails on a box
QJsonObject Iocage::cleanJails() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage clean -fj ").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    // This means either a mount is stuck or a jail cannot be stopped,
    // give them the error.
    if ( output.at(i).indexOf("ERROR:") != -1 ) {
      retObject.insert("error", output.at(i));
      break;
    } else {
      // iocage does a spinner for these that is distracting to see returned,
      // returning what a user wants to actually see.
      retObject.insert("success", "All jails have been cleaned.");
    }
  }

  return retObject;
}

// Resource cap a jail on the box
QJsonObject Iocage::capJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();
  QStringList output = General::RunCommand("iocage cap " + jail).split("\n");
  QJsonObject vals;

  for ( int i = 0; i < output.size(); i++)
  {
    if ( ! output.at(i).isEmpty())
      break;

    // When a cap is successful, iocage doesn't return anything, so we have to
    // fudge the output a bit.
    retObject.insert("success", "jail " + jail + " capped.");
  }

  return retObject;
}

// Deactivate a zpool for iocage on the box
QJsonObject Iocage::deactivatePool(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("pool") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString pool = jsin.value("pool").toString();
  QStringList output = General::RunCommand("iocage deactivate " + pool).split("\n");
  QJsonObject vals;

  for ( int i = 0; i < output.size(); i++)
  {
    if ( ! output.at(i).isEmpty())
      break;

    // When a pool deactivation is successful, iocage doesn't return anything,
    // so we have to fudge the output a bit.
    retObject.insert("success", "pool " + pool + " deactivated.");
  }

  return retObject;
}

// Activate a zpool for iocage on the box
QJsonObject Iocage::activatePool(QJsonObject jsin) {
  QJsonObject retObject;
  QStringList keys = jsin.keys();

  // Get the key values
  QString pool = jsin.value("pool").toString();
  QStringList output = General::RunCommand("iocage activate " + pool).split("\n");
  QJsonObject vals;

  for ( int i = 0; i < output.size(); i++)
  {
    // Otherwise we get null output before the actual active pool
    if ( i > 0 )
      break;

    // When a pool activation is successful, iocage doesn't return anything,
    // so we have to fudge the output a bit.
    if ( output.at(i).isEmpty())
    {
      retObject.insert("success", "pool " + pool + " activated.");
      break;
    } else {
        QString key = output.at(i).simplified().section(":", 0, 0);
        QString value = output.at(i).simplified().section(":", 1, 1);

        vals.insert(key, value);

        // This means no pool exists, give them the error.
        if ( output.at(i).indexOf("ERROR:") != -1 ) {
          retObject.insert("error", output.at(i));
          break;

        retObject.insert("currently active", vals);
      }
    }
  }

  return retObject;
}

// Stop a jail on the box
QJsonObject Iocage::stopJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();

  QStringList output = General::RunCommand("iocage stop " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert(jail, vals);
  return retObject;
}

// Start a jail on the box
QJsonObject Iocage::startJail(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();

  QStringList output = General::RunCommand("iocage start " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert(jail, vals);
  return retObject;
}

// Return all the default iocage settings
QJsonObject Iocage::getDefaultSettings() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage defaults").split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section("=", 0, 0);
    QString value = output.at(i).simplified().section("=", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert("defaults", vals);
  return retObject;
}

// Return all of the jail settings
QJsonObject Iocage::getJailSettings(QJsonObject jsin) {
  QJsonObject retObject;

  QStringList keys = jsin.keys();
  if (! keys.contains("jail") ) {
    retObject.insert("error", "Missing required keys");
    return retObject;
  }

  // Get the key values
  QString jail = jsin.value("jail").toString();

  QStringList output = General::RunCommand("iocage get all " + jail).split("\n");

  QJsonObject vals;
  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QString key = output.at(i).simplified().section(":", 0, 0);
    QString value = output.at(i).simplified().section(":", 1, 1);

    vals.insert(key, value);
  }

  retObject.insert(jail, vals);
  return retObject;
}

// List the jails on the box
QJsonObject Iocage::listJails() {
  QJsonObject retObject;

  QStringList output = General::RunCommand("iocage list").split("\n");

  for ( int i = 0; i < output.size(); i++)
  {
    if ( output.at(i).indexOf("JID") != -1 )
      continue;

    if ( output.at(i).isEmpty() )
      break;

    QJsonObject jail;
    QString line = output.at(i).simplified();
    QString uuid = line.section(" ", 1, 1);

    jail.insert("jid", line.section(" ", 0, 0));
    jail.insert("boot", line.section(" ", 2, 2));
    jail.insert("state", line.section(" ", 3, 3));
    jail.insert("tag", line.section(" ", 4, 4));
    jail.insert("type", line.section(" ", 5, 5));
    jail.insert("ip4", line.section(" ", 6, 6));

    retObject.insert(uuid, jail);
  }

  return retObject;
}
