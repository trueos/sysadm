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
| name                            | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | lifepreserver |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "listcron", "listsnap", "cronsnap", and "settings"                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. _List Schedules:

List Schedules
==============

The "listcron" action retrieves the information for each Life Preserver scheduled snapshot task. For each schedule, it lists the name of the ZFS pool, the number of snapshots to keep, and
the time that the snapshot is taken.

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
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

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

.. _Create a Schedule:

Create a Schedule
=================

The "cronsnap" action is used to create snapshot schedules for Life Preserver. When creating a schedule, specify the ZFS pool to snapshot, how many snapshots to keep, and at what time the
snapshots should occur.

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