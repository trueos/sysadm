.. _lifepreserver:

lifepreserver
*************

The lifepreserver class is used to manage and retrieve information about scheduled snapshots and replication.

Every lifepreserver class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | lifepreserver |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "listcron", "cronsnap", "cronscrub", "createsnap", "listsnap", "revertsnap", "removesnap", |
|                                 |               | "addreplication", "removereplication", "listreplication", "runreplication", "initreplication", "settings", and       |
|                                 |               | "savesettings"                                                                                                       |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. index:: listcron, Life Preserver

.. _List Schedules:

List Schedules
==============

The "listcron" action retrieves the information for each Life Preserver scheduled task. If snapshots have been configured for a ZFS pool, it lists the number of snapshots to keep and
the time that snapshots are taken. If scrubs have been configured on that ZFS pool, it also lists the time that ZFS scrubs occur.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "listcron"
 }


**REST Response**

.. code-block:: json

 {
    "args": {
        "listcron": {
            "tank1": {
                "keep": "3",
                "schedule": "daily@18"
                "scrub": "daily@22"
            }
        }
    }
 }
 
**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "listcron"
   },
   "name" : "lifepreserver",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listcron": {
      "tank1": {
        "keep": "3",
        "schedule": "daily@18"
        "scrub": "daily@22"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: cronsnap, Life Preserver
 
.. _Create a Snapshot Schedule:

Create a Snapshot Schedule
==========================

The "cronsnap" action is used to create snapshot schedules for Life Preserver. This action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| pool                            | name of ZFS pool to snapshot                                                                                         |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| keep                            | specify the number of snapshots to keep                                                                              |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| frequency                       | specify when to take the snapshots; possible values are "daily@XX" (where XX is the number of the hour),             |
|                                 | "hourly", "30min", "10min", "5min" or "none" (disables snapshots)                                                    |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+


**REST Request**

.. code-block:: json

  PUT /sysadm/lifepreserver
  {
    "pool" : "mytank",
    "keep" : "10",
    "frequency" : "daily@11",
    "action" : "cronsnap"
  }

**REST Response**

.. code-block:: json

 {
    "args": {
        "cronsnap": {
            "frequency": "daily@11",
            "keep": "10",
            "pool": "mytank"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "cronsnap",
      "keep" : "10",
      "frequency" : "daily@11",
      "pool" : "mytank"
   },
   "namespace" : "sysadm",
   "name" : "lifepreserver",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "cronsnap": {
      "frequency": "daily@11",
      "keep": "10",
      "pool": "mytank"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: cronscrub, Life Preserver
 
.. _Create a Scrub Schedule:

Create a Scrub Schedule
==========================

The "cronscrub" action is used to schedule a ZFS scrub. This action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| pool                            | name of ZFS pool to scrub                                                                                            |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| frequency                       | specify when to perform the scrub; possible values are "daily@XX", "weekly@YY@XX", and monthly@ZZ@XX, where "XX" is  |
|                                 | the hour, "YY" is the day of week ("01" for Monday through "07" for Sunday), "ZZ" is the day of month, and "none"    |
|                                 | disables scrubs                                                                                                      |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "cronscrub",
   "pool" : "tank",
   "frequency" : "daily@22"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "cronscrub": {
            "frequency": "daily@22",
            "pool": "tank"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "cronscrub",
      "pool" : "tank",
      "frequency" : "daily@22"
   },
   "namespace" : "sysadm",
   "name" : "lifepreserver",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "cronscrub": {
      "frequency": "daily@22",
      "pool": "tank"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: createsnap, Life Preserver
 
.. _Create a Snapshot:

Create a Snapshot
=================

The "createsnap" action creates a one-time snapshot of the specified dataset.

**REST Request**

.. code-block:: json 

 PUT /sysadm/lifepreserver
 {
   "snap" : "mytestsnap",
   "dataset" : "tank",
   "comment" : "Testing",
   "action" : "createsnap"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "args" : {
      "comment" : "Testing",
      "dataset" : "tank",
      "action" : "createsnap",
      "snap" : "mytestsnap"
   },
   "name" : "lifepreserver",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "createsnap": {
      "comment": "Testing",
      "dataset": "tank",
      "snap": "mytestsnap"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: listsnap, Life Preserver
 
.. _List Snapshots:

List Snapshots
==============

The "listsnap" action retrieves the list of saved snapshots.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "pool" : "tank1",
   "action" : "listsnap"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "listsnap": {
            "tank1@auto-2016-01-04-18-00-00": {
                "comment": "Automated Snapshot"
            },
            "tank1@auto-2016-01-05-18-00-00": {
                "comment": "Automated Snapshot"
            },
            "tank1@auto-2016-01-06-18-00-00": {
                "comment": "Automated Snapshot"
            },
            "tank1@auto-2016-01-07-18-00-00": {
                "comment": "Automated Snapshot"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "lifepreserver",
   "args" : {
      "pool" : "tank1",
      "action" : "listsnap"
   },
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listsnap": {
      "tank1@auto-2016-01-04-18-00-00": {
        "comment": "Automated Snapshot"
      },
      "tank1@auto-2016-01-05-18-00-00": {
        "comment": "Automated Snapshot"
      },
      "tank1@auto-2016-01-06-18-00-00": {
        "comment": "Automated Snapshot"
      },
      "tank1@auto-2016-01-07-18-00-00": {
        "comment": "Automated Snapshot"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: revertsnap, Life Preserver
 
.. _Revert a Snapshot:

Revert a Snapshot
=================

The "revertsnap" action is used to rollback the contents of the specified dataset to the point in time that the specified snapshot was taken.

.. warning:: performing this operation will revert the contents of the dataset back in time, meaning that all changes to the dataset's files that occurred since the snapshot was taken will
   be lost.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "snap" : "auto-2016-01-09-18-00-00",
   "dataset" : "tank1/usr/jails",
   "action" : "revertsnap"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "revertsnap": {
            "dataset": "tank1/usr/jails",
            "snap": "auto-2016-01-09-18-00-00"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "dataset" : "tank1/usr/jails",
      "action" : "revertsnap",
      "snap" : "auto-2016-01-09-18-00-00"
   },
   "namespace" : "sysadm",
   "name" : "lifepreserver",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "revertsnap": {
      "dataset": "tank1/usr/jails",
      "snap": "auto-2016-01-09-18-00-00"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: removesnap, Life Preserver
 
.. _Remove a Snapshot:

Remove a Snapshot
=================

The "removesnap" action is used to remove a ZFS snapshot from the specified dataset or pool.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "dataset" : "tank1/usr/jails",
   "snap" : "auto-2016-01-09-18-00-00",
   "action" : "removesnap"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "removesnap": {
            "dataset": "tank1/usr/jails",
            "snap": "auto-2016-01-09-18-00-00"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "snap" : "auto-2016-01-09-18-00-00",
      "action" : "removesnap",
      "dataset" : "tank1/usr/jails"
   },
   "name" : "lifepreserver",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
     "removesnap": {
        "dataset": "tank1/usr/jails",
        "snap": "auto-2016-01-09-18-00-00"
     }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: addreplication, Life Preserver
 
.. _Add Replication:

Add Replication
===============

The "addreplication" action is used to create a replication task in Life Preserver. This action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| host                            | remote hostname or IP address                                                                                        |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| port                            | SSH port number on remote system                                                                                     |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| user                            | user must exist on remote system                                                                                     |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| password                        | the password for *user* on remote system                                                                             |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| dataset                         | name of local dataset to replicate                                                                                   |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| remotedataset                   | path to dataset on remote system                                                                                     |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| frequency                       | when to replicate; supported times are "XX" (hour), "sync" (as snapshot is created, not recommended for frequent     |
|                                 | snapshots), "hour" (hourly), "30min" (every 30 minutes), "10min" (every 10 minutes), or "manual" (only when          |
|                                 | requested by user)                                                                                                   |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "addreplication",
   "password" : "mypass",
   "dataset" : "tank1",
   "remotedataset" : "tank/backups",
   "user" : "backupuser",
   "frequency" : "22",
   "port" : "22",
   "host" : "192.168.0.10"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "addreplication": {
            "frequency": "22",
            "host": "192.168.0.10",
            "ldataset": "tank1",
            "port": "22",
            "rdataset": "tank/backups",
            "user": "backupuser"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "name" : "lifepreserver",
   "args" : {
      "action" : "addreplication",
      "user" : "backupuser",
      "dataset" : "tank1",
      "frequency" : "22",
      "port" : "22",
      "password" : "mypass",
      "host" : "192.168.0.10",
      "remotedataset" : "tank/backups"
   },
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "addreplication": {
      "frequency": "22",
      "host": "192.168.0.10",
      "ldataset": "tank1",
      "port": "22",
      "rdataset": "tank/backups",
      "user": "backupuser"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: removereplication, Life Preserver
 
.. _Remove Replication:

Remove Replication
==================

The "removereplication" action is used to delete an existing replication task. Note that this action only deletes the task--it does not remove any already replicated data from the
remote system.

This action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| host                            | remote hostname or IP address                                                                                        |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| dataset                         | name of local dataset to remove from replication                                                                     |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "dataset" : "tank",
   "host" : "192.168.0.10",
   "action" : "removereplication"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "removereplication": {
            "dataset": "tank",
            "host": "192.168.0.10"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "removereplication",
      "dataset" : "tank",
      "host" : "192.168.0.10"
   },
   "name" : "lifepreserver",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "removereplication": {
      "dataset": "tank",
      "host": "192.168.0.10"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: listreplication, Life Preserver
 
.. _List Replications: 

List Replications
=================

The "listreplication" action is used to retrieve the settings of configured replication tasks. For each task, the response includes the name of the local ZFS pool or dataset to replicate,
the IP address and listening port number of the remote system to replicate to, when the replication occurs (see the "frequency" description in :ref:`Add Replication`), the name of the
dataset on the remote system to store the replicated data ("rdset"), and the name of the replication user account.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "listreplication"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "listreplication": {
            "tank1->192.168.0.9": {
                "dataset": "tank1",
                "frequency": "22",
                "host": "192.168.0.9",
                "port": "22",
                "rdset": "tank/backups",
                "user": "backups"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "listreplication"
   },
   "id" : "fooid",
   "name" : "lifepreserver"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listreplication": {
      "tank1->192.168.0.9": {
        "dataset": "tank1",
        "frequency": "22",
        "host": "192.168.0.9",
        "port": "22",
        "rdset": "tank/backups",
        "user": "backups"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: runreplication, Life Preserver
 
.. _Start Replication:

Start Replication
=================

The "runreplication" action can be used to manually replicate the specified dataset to the specified remote server.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "host" : "10.0.10.100",
   "dataset" : "mypool",
   "action" : "runreplication"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "runreplication": {
            "dataset": "mypool",
            "host": "10.0.10.100"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "lifepreserver",
   "args" : {
      "host" : "10.0.10.100",
      "dataset" : "mypool",
      "action" : "runreplication"
   },
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "runreplication": {
      "dataset": "mypool",
      "host": "10.0.10.100"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: initreplication, Life Preserver
 
.. _Initialize Replication:

Initialize Replication
======================

The "initreplication" action can be used to clear the replication data on the remote server. This is useful if a replication becomes stuck. After running this action, issue a
"runreplication" action to start a new replication.

The "initreplication" action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| host                            | remote hostname or IP address                                                                                        |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| dataset                         | name of local dataset or pool being replicated                                                                       |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "dataset" : "tank1",
   "host" : "192.168.0.9",
   "action" : "initreplication"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "initreplication": {
            "dataset": "tank1",
            "host": "192.168.0.9"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "host" : "192.168.0.9",
      "dataset" : "tank1",
      "action" : "initreplication"
   },
   "namespace" : "sysadm",
   "name" : "lifepreserver"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "initreplication": {
      "dataset": "tank1",
      "host": "192.168.0.9"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: settings, Life Preserver
 
.. _View Settings:

View Settings
=============

The "settings" action returns the system-wide settings of the Life Preserver utility. The returned settings include the disk percentage used at which Life Preserver will issue a warning, the
level at which an email will be sent, the email address to send notifications to, and whether or not snapshots are taken recursively (include all child datasets).

Run :command:`lpreserver help set` for more information about each available setting.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "settings"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "settings": {
            "diskwarn": "85%",
            "email": "WARN",
            "emailaddress": "me@mycompany.com",
            "recursive": "ON"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "settings"
   },
   "namespace" : "sysadm",
   "name" : "lifepreserver"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "settings": {
      "diskwarn": "85%",
      "email": "WARN",
      "emailaddress": "me@mycompany.com",
      "recursive": "ON"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: savesettings, Life Preserver
 
.. _Save Settings:

Save Settings
=============

The "savesettings" action can be used to modify the system-wide settings of the Life Preserver utility. This action supports the following parameters:

+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Description**                                                                                                      |
|                                 |                                                                                                                      |
+=================================+======================================================================================================================+
| duwarn                          | disk percentage (from 0-99) at which to warn of low disk space                                                       |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| email                           | email address to send notifications to                                                                               |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| emailopts                       | conditions which trigger an email notification; possible values are "ALL" (every snapshot, warning and error),       |
|                                 | "WARN" (warnings and errors--this is the default condition), or "ERROR" (errors only)                                |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+
| recursive                       | whether or not to include all child datasets in the snapshot; possible values are "true" or "false"                  |
|                                 |                                                                                                                      |
+---------------------------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "emailopts" : "ALL",
   "duwarn" : "70",
   "recursive" : "true",
   "action" : "savesettings",
   "email" : "kris@example.org"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "savesettings": {
            "duwarn": "70",
            "email": "kris@example.org",
            "emailopts": "ALL",
            "recursive": "true"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "emailopts" : "ALL",
      "action" : "savesettings",
      "duwarn" : "70",
      "recursive" : "true",
      "email" : "kris@example.org"
   },
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "lifepreserver"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "savesettings": {
      "duwarn": "70",
      "email": "kris@example.org",
      "emailopts": "ALL",
      "recursive": "true"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
