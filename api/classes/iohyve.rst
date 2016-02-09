.. _iohyve:

iohyve
******

The iohyve class is used to manage virtual machines (VMs) running in the bhyve type 2 hypervisor.

Every iohyve class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | iohyve        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "listvms", "fetchiso", "renameiso", "rmiso", "setup",  "issetup"                           |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: listvms, iohyve

.. _List VMs:

List VMs
========

The "listvms" action lists information about currently installed VMs. For each VM, the response includes the VM's name, description, whether or not it is scheduled to start when the host
system boots, whether or not it is currently running, and whether or not the VM is currently loaded into memory.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "listvms"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "listvms": {
            "testguest": {
                "description": "February 1, 2016 at 03:11:57 PM EST",
                "rcboot": "NO",
                "running": "NO",
                "vmm": "YES"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "listvms"
   },
   "name" : "iohyve",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listvms": {
      "testguest": {
        "description": "February 1, 2016 at 03:11:57 PM EST",
        "rcboot": "NO",
        "running": "NO",
        "vmm": "YES"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: fetchiso, iohyve

.. _Fetch ISO:

Fetch ISO
=========

The "fetchiso" action is used to retrieve the installation ISO. It is used with the "url" argument which contains the ISO address beginning with *http://*, 
*ftp://*, or
*file://*.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "url" : "ftp://ftp.freebsd.org/pub/FreeBSD/releases/amd64/amd64/ISO-IMAGES/10.1/FreeBSD-10.1-RELEASE-amd64-disc1.iso",
   "action" : "fetchiso"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "name" : "iohyve",
   "args" : {
      "url" : "ftp://ftp.freebsd.org/pub/FreeBSD/releases/amd64/amd64/ISO-IMAGES/10.1/FreeBSD-10.1-RELEASE-amd64-disc1.iso",
      "action" : "fetchiso"
   },
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "fetchiso": {
      "command": "iohyve fetch ftp://ftp.freebsd.org/pub/FreeBSD/releases/amd64/amd64/ISO-IMAGES/10.1/FreeBSD-10.1-RELEASE-amd64-disc1.iso",
      "comment": "Task Queued",
      "queueid": "{b3a8b980-a564-4ff8-86a2-1971bd4f58d1}"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: renameiso, iohyve

.. _Rename ISO:

Rename ISO
==========

The "renameiso" action is used to to rename an existing ISO file on disk. Specify the existing name with "source" and the new name with "target".

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "source" : "test.iso",
   "target" : "102.iso",
   "action" : "renameiso"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "target" : "102.iso",
      "source" : "test.iso",
      "action" : "renameiso"
   },
   "id" : "fooid",
   "name" : "iohyve",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "renameiso": {
      "source": "test.iso",
      "target": "102.iso"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: rmiso, iohyve

.. _Remove ISO:

Remove ISO
==========

The "rmiso" action is used to to remove an existing ISO file from disk. Specify the ISO's name as the "target".

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "rmiso",
   "target" : "FreeBSD-10.2-RELEASE-amd64-bootonly.iso"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "iohyve",
   "args" : {
      "target" : "FreeBSD-10.2-RELEASE-amd64-bootonly.iso",
      "action" : "rmiso"
   },
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "rmiso": {
      "target": "FreeBSD-10.2-RELEASE-amd64-bootonly.iso"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: setup, iohyve

.. _Setup iohyve:

Setup iohyve
============

The "setup" action performs the initial setup of iohyve. It is mandatory to specify the FreeBSD device name of the "nic" and the ZFS "pool" to use.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "nic" : "re0",
   "pool" : "tank",
   "action" : "setup"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "iohyve",
   "args" : {
      "pool" : "tank",
      "nic" : "re0",
      "action" : "setup"
   },
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "setup": {
      "nic": "re0",
      "pool": "tank"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: issetup, iohyve

.. _Determine iohyve Setup:

Determine iohyve Setup
======================

The "issetup" action queries if iohyve has been setup and returns either "true" or "false".

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "issetup"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "args" : {
      "action" : "issetup"
   },
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "issetup": {
      "setup": "true"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }