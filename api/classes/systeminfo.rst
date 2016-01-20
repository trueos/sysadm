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
| action                          |               | supported actions include "externalmounts"                                                                           |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

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