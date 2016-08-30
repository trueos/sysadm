.. _update:

update
******

The update class is used to check for and manage system and software
updates.

Every update class request contains several parameters:

+----------------+------------+---------------------------------------+
| **Parameter**  | **Value**  | **Description**                       |
|                |            |                                       |
+================+============+=======================================+
| id             |            | Any unique value for the request,     |
|                |            | including a hash, checksum, or uuid.  |
+----------------+------------+---------------------------------------+
| name           | update     |                                       |
|                |            |                                       |
+----------------+------------+---------------------------------------+
| namespace      | sysadm     |                                       |
|                |            |                                       |
+----------------+------------+---------------------------------------+
| action         |            | Actions include "changesettings",     |
|                |            | "checkupdates", "listbranches",       |
|                |            | "listsettings", "startupdate",        |
|                |            | "stopupdate"                          |
+----------------+------------+---------------------------------------+

The rest of this section provides examples of the available *actions*
for each type of request, along with their responses.

.. index:: changesettings, update

.. _Change Settings:

Change Settings
===============

Use :command:`changesettings` to change the various updatemanager
settings (maxbe, package_set, package_url, auto_update).

**REST Request**

::

 PUT /sysadm/update
 {
    "action" : "changesettings",
    "maxbe" : "6"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "name" : "update",
    "namespace" : "sysadm",
    "args" : {
       "maxbe" : "6",
       "action" : "changesettings"
    }
 }

**Response**

.. code-block:: json

 {
   "args": {
     "changesettings": {
       "result": "success"
     }
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: checkupdates, update

.. _Check for Updates:

Check for Updates
=================

The "checkupdates" action queries the update server to see if any
updates are available. If an update is available, the response will
indicate if it is a system security update, an upgrade to a newer
version of the operating system, a system patch, or an update to
installed software packages.

**REST Request**

::

 PUT /sysadm/update
 {
   "action" : "checkupdates"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "last_check" : "<ISO date/time stamp>",
        "checkupdates": {
            "status": "updatesavailable",
            "details": "<updatedetails>"
        },
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "update",
   "args" : {
      "action" : "checkupdates",
      "force":"[true/false]"
   },
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "last_check" : "<ISO date/time stamp>",
    "checkupdates": {
      "status": "updatesavailable",
      "details": "<updatedetails>"
    },
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: listbranches, update

.. _List Branches:

List Branches
=============

The "listbranches" action retrieves the list of available branches
(operating system versions). The currently installed version will be
listed as "active".

**REST Request**

::

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

.. index:: listsettings, update

.. _List Settings:

List Settings
=============

:command:`listsettings` shows all of the current settings.

**REST Request**

::

 PUT /sysadm/update
 {
    "action" : "listsettings"
 }

**WebSocket Request**

.. code-block:: json

 {
    "args" : {
       "action" : "listsettings"
    },
    "id" : "fooid",
    "namespace" : "sysadm",
    "name" : "update"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "listsettings": {
       "maxbe": " 5",
       "package_set": " EDGE"
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

The "startupdate" action starts the specified update. You must specify a
"target" to indicate the type of update to perform. The available
targets are:

* **chbranch:** Will update to the specified "branch" (operating system
  version). You can determine which branches are available by using the
  "listbranches" action.

* **pkgupdate:** Only updates installed software.

* **standalone:** Only apply the update specified as a "tag". Use the
  "checkupdates" action to determine the name (tag) of the update to
  specify.

**REST Request**

::

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
    "event_system" : "sysadm/update",
    "state" : "finished",
    "update_log" : "<update log>",
    "process_details" : {
      "time_finished" : "<ISO 8601 time date string>",
      "cmd_list" : ["<command 1>", "<command 2>"],
      "return_codes/<command 1>" : "<code 1>",
      "return_codes/<command 2>" : "<code 2>",
      "process_id" : "<random>",
      "state" : "finished"
      }
    }
 }

.. index:: stopupdate, update

.. _Stop Updates:

Stop Updates
============

This will look for any currently-running pc-updatemanager processes
and kill/stop them as needed.

**Websocket Request**

.. code-block:: json

 {
  "id":"dummy",
  "namespace":"sysadm",
  "name":"update",
  "args": {
     "action":"stopupdate"
  }
 }

**Websocket Response**

.. code-block:: json

 {
   "args":{
     "stopupdate": {
       "result": "success"
       }
   }
 }