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
| action                          |               | supported actions include "listvms", "fetchiso", "listisos", "renameiso", "rmiso", "setup",  "issetup", "create",    |
|                                 |               | "install", "start", "stop", "delete", "adddisk", "listdisks", "resizedisk", "deletedisk", "version", "getprops",     |
|                                 |               | and "setprop"                                                                                                        |
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

**Dispatcher Events System Reply**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "event_system" : "sysadm"/"iohyve",
    "state" : "running" OR "finished",
    "filename",
    "percent_done",
    "download_rate",
    "process_details" : {
      "time_started" : <ISO 8601 time date string>,
      "cmd_list" : [ "<command 1>", "<command 2>"],
      "process_id" : "<some id string>",
      "state" : "running"
      }
   }
 }

.. index:: listisos, iohyve

.. _List ISOs:

List ISOs
=========

The "listisos" action lists all the known ISO files which iohyve can use.

**REST Request**

.. code-block:: json
 
 PUT /sysadm/iohyve
 {
   "action" : "listisos"
 }

**WebSocket Request**

.. code-block:: json
 
 {
   "name" : "iohyve",
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "action" : "listisos"
   }
 }

**Response**

.. code-block:: json
 
 {
  "args": {
    "listisos": [
      "TRUEOS10.2-RELEASE-08-19-2015-x64-netinstall.iso"
    ]
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
 
.. index:: create, iohyve

.. _Create Guest:

Create Guest
============

The "create" action creates a new iohyve guest of the specified "name" and "size".

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "create",
   "name" : "bsdguest",
   "size" : "10G"
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "iohyve",
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "name" : "bsdguest",
      "action" : "create",
      "size" : "10G"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "create": {
      "name": "bsdguest",
      "size": "10G"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: install, iohyve

.. _Install Guest:

Install Guest
=============

The "install" action starts the iohyve installation of the specified guest from the specified ISO. This action only boots the VM with the ISO; to do the actual installation,
run :command:`iohyve console <name>` from the system.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "name" : "bsdguest",
   "iso" : "FreeBSD-10.2-RELEASE-amd64-disc1.iso",
   "action" : "install"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "name" : "iohyve",
   "id" : "fooid",
   "args" : {
      "action" : "install",
      "iso" : "FreeBSD-10.2-RELEASE-amd64-disc1.iso",
      "name" : "bsdguest"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "install": {
      "iso": "FreeBSD-10.2-RELEASE-amd64-disc1.iso",
      "name": "bsdguest"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: start, iohyve

.. _Start VM:

Start VM
========

The "start" action starts the specified VM.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "start",
   "name" : "bsdguest"
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "iohyve",
   "id" : "fooid",
   "args" : {
      "action" : "start",
      "name" : "bsdguest"
   },
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "start": {
      "name": "bsdguest"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: stop, iohyve

.. _Stop VM:

Stop VM
=======

The "stop" action stops the specified VM.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "stop",
   "name" : "bsdguest"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "stop",
      "name" : "bsdguest"
   },
   "name" : "iohyve",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "stop": {
      "name": "bsdguest"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: delete, iohyve

.. _Delete a VM:

Delete a VM
===========

The "delete" action deletes the specified iohyve guest.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "delete",
   "name" : "bsdguest"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "action" : "delete",
      "name" : "bsdguest"
   },
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "delete": {
      "name": "bsdguest"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: adddisk, iohyve

.. _Add a Disk:

Add a Disk
==========

The "adddisk" action adds and creates a disk for a VM.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "name" : "bsdguest",
   "action" : "adddisk",
   "size" : "10G"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "size" : "10G",
      "name" : "bsdguest",
      "action" : "adddisk"
   },
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "adddisk": {
      "bsdguest": {
        "size": "10G"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: listdisks, iohyve

.. _List Disks:

List Disks
==========

The "listdisks" action lists the disks connected to the specified VM.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "name" : "bsdguest",
   "action" : "listdisks"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "listdisks",
      "name" : "bsdguest"
   },
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "listdisks": {
      "disk0": "10G"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: resizedisk, iohyve

.. _Resize a Disk:

Resize a Disk
=============

The "resizedisk" action **increases** the specified disk in the specified VM. The new specified size must be larger than the current size.

**REST Request**

.. code-block:: json 

 PUT /sysadm/iohyve
 {
   "disk" : "disk0",
   "name" : "bsdguest",
   "action" : "resizedisk",
   "size" : "20G"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "name" : "iohyve",
   "id" : "fooid",
   "args" : {
      "size" : "20G",
      "action" : "resizedisk",
      "disk" : "disk0",
      "name" : "bsdguest"
   },
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "resizedisk": {
      "disk": "disk0",
      "name": "bsdguest",
      "size": "20G"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: deletedisk, iohyve

.. _Delete a Disk:

Delete a Disk
=============

The "deletedisk" action removes the specified disk from the specified VM.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "disk" : "disk1",
   "name" : "bsdguest",
   "action" : "deletedisk"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "name" : "iohyve",
   "args" : {
      "name" : "bsdguest",
      "action" : "deletedisk",
      "disk" : "disk1"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "deletedisk": {
      "disk": "disk1",
      "name": "bsdguest"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
 .. index:: version, iohyve

.. _List Version:

List Version
============

The "version" action displays the iohyve version.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "version"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "version"
   },
   "id" : "fooid",
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "version": {
      "version": "iohyve v0.7.3 2016/01/08 Bear in a Datacenter Edition"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: getprops, iohyve

.. _List Guest Properties:

List Guest Properties
=====================

The "getprops" action lists the properties for the specified guest.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "action" : "getprops",
   "name" : "bsdguest"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "args" : {
      "name" : "bsdguest",
      "action" : "getprops"
   },
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "getprops": {
      "bsdguest": {
        "autogrub": "\\n",
        "bargs": "-A_-H_-P",
        "boot": "0",
        "con": "nmdm0",
        "cpu": "1",
        "description": "Tue",
        "install": "no",
        "loader": "bhyveload",
        "name": "bsdguest",
        "os": "default",
        "persist": "1",
        "ram": "256M",
        "size": "10G",
        "tap": "tap0"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: setprop, iohyve

.. _Set Guest Properties:

Set Guest Properties
=====================

The "setprop" action can be used to modify the properties for the specified guest. For each property, specify its name and desired value. Use "getprops" to see the current properties
and values for the guest.

**REST Request**

.. code-block:: json

 PUT /sysadm/iohyve
 {
   "ram" : "512M",
   "name" : "bsdguest",
   "action" : "setprop"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "ram" : "512M",
      "name" : "bsdguest",
      "action" : "setprop"
   },
   "name" : "iohyve"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "setprop": {
      "bsdguest": {
        "ram": "512M"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }