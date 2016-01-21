.. _systeminfo:

systeminfo
**********

The systeminfo class is used to retrieve information about the system. Every systeminfo class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | systeminfo    |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "batteryinfo", "externalmounts"                                                            |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. index:: batteryinfo, systeminfo

.. _Battery Information:

Battery Information
===================

The "batteryinfo" action will indicate whether or not a battery exists. If it does, it will also report its current charge percentage level (1-99) and its
status (offline, charging, on backup, or unknown).

**REST Request**

.. code-block:: json

 PUT /sysadm/systeminfo
 {
   "action" : "batteryinfo"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "batteryinfo": {
            "battery": "false"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "name" : "systeminfo",
   "id" : "fooid",
   "args" : {
      "action" : "batteryinfo"
   }
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "batteryinfo": {
      "battery": "false"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: externalmounts, systeminfo

.. _List External Mounts:

List External Mounts
====================

The "externalmounts" action returns a list of mounted external devices. Supported device types are UNKNOWN, USB, HDRIVE (external hard drive), DVD, and SDCARD.
For each mounted device, the response will include the device name, filesystem, mount path, and device type.

**REST Request**

.. code-block:: json

 PUT /sysadm/systeminfo
 {
   "action" : "externalmounts"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "externalmounts": {
            "/dev/fuse": {
                "filesystem": "fusefs",
                "path": "/usr/home/kris/.gvfs",
                "type": "UNKNOWN"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "systeminfo",
   "args" : {
      "action" : "externalmounts"
   }
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "externalmounts": {
      "/dev/fuse": {
        "filesystem": "fusefs",
        "path": "/usr/home/kris/.gvfs",
        "type": "UNKNOWN"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }