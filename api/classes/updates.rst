.. _update:

update
******

The update class is used to check for and manage system and software updates.

Every update class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | update        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "checkupdates", "listbranches", "startupdate"                                              |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: checkupdates, update

.. _Check for Updates:

Check for Updates
=================

The "checkupdates" action queries the update server to see if any updates are available. If an update is available, the response will indicate if it is a system security update, an
upgrade to a newer version of the operating system, a system patch, or an update to installed software packages.

**REST Request**

.. code-block:: json

 PUT /sysadm/update
 {
   "action" : "checkupdates"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "checkupdates": {
            "status": "noupdates"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "update",
   "args" : {
      "action" : "checkupdates"
   },
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "checkupdates": {
      "status": "noupdates"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: listbranches, update

.. _List Branches:

List Branches
=============

The "listbranches" action retrieves the list of available branches (operating system versions). The currently installed version will be listed as "active".

**REST Request**

.. code-block:: json

 PUT /sysadm/update
 {
   "action" : "listbranches"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "listbranches": {
            "10.2-RELEASE": "active",
            "11.0-CURRENTJAN2016": "available"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "listbranches"
   },
   "namespace" : "sysadm",
   "name" : "update",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listbranches": {
      "10.2-RELEASE": "active",
      "11.0-CURRENTJAN2016": "available"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: startupdate, update

.. _Start Updates:

Start Updates
=============

The "startupdate" action starts the specified update. You must specify a "target" to indicate the type of update to perform. The available targets are:

* **chbranch:** will update to the specified "branch" (operating system version). You can determine which branches are available by using the "listbranches" action.

* **pkgupdate:** only update installed software.

* **fbsdupdate:** only apply FreeBSD system updates.

* **fbsdupdatepkgs:** update installed software and apply FreeBSD system updates.

* **standalone:** only apply the update specified as a "tag". Use the "checkupdates" action to determine the name (tag) of the update to specify.

**REST Request**

.. code-block:: json

 PUT /sysadm/update
 {
   "action" : "startupdate",
   "target" : "pkgupdate"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
      "startupdate": {
        "queueid": "611c89ae-c43c-11e5-9602-54ee75595566",
        "command": "pc-updatemanager pkgupdate",
        "comment": "Task Queued"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "startupdate",
      "target" : "pkgupdate"
   },
   "name" : "update",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "startupdate": {
      "queueid": "611c89ae-c43c-11e5-9602-54ee75595566",
      "command": "pc-updatemanager pkgupdate",
      "comment": "Task Queued"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

**Dispatcher Events System Reply**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "event_system" : "sysadm"/"update",
    "state" : "running" OR "finished",
    "update_log"
    "process_details" : {
      "time_started" : <ISO 8601 time date string>,
      "cmd_list" : [ "<command 1>", "<command 2>"],
      "process_id" : "<some id string>",
      "state" : "running"
      }
    }
 }