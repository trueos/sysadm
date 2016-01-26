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
| action                          |               | supported actions include "memorystats", "cpupercentage", "cputemps", "batteryinfo", "externalmounts", "systeminfo"  |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. index:: memorystats, systeminfo

.. _Memory Statistics:

Memory Statistics
=================

The "memorystats" action returns memory statistics, including the amount of active, cached, free, inactive, and total physical (wired) memory.

**REST Request**

.. code-block:: json

 PUT /sysadm/systeminfo
 {
   "action" : "memorystats"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "memorystats": {
            "active": "818",
            "cache": "69",
            "free": "4855",
            "inactive": "2504",
            "wired": "1598"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "memorystats"
   },
   "namespace" : "sysadm",
   "name" : "systeminfo"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "memorystats": {
      "active": "826",
      "cache": "69",
      "free": "4847",
      "inactive": "2505",
      "wired": "1598"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: cpupercentage, systeminfo

.. _CPU Usage:

CPU Usage
=========

The "cpupercentage" action returns the usage percentage of each CPU.

**REST Request**

.. code-block:: json 

 PUT /sysadm/systeminfo
 {
   "action" : "cpupercentage"
 }

**REST Response**

.. code-block:: json 

 {
    "args": {
        "cpupercentage": {
            "busytotal": "28",
            "cpu1": {
                "busy": "28"
            },
            "cpu2": {
                "busy": "31"
            },
            "cpu3": {
                "busy": "29"
            },
            "cpu4": {
                "busy": "24"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json 

 {
   "args" : {
      "action" : "cpupercentage"
   },
   "name" : "systeminfo",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json 

 {
  "args": {
    "cpupercentage": {
      "busytotal": "28",
      "cpu1": {
        "busy": "28"
      },
      "cpu2": {
        "busy": "31"
      },
      "cpu3": {
        "busy": "29"
      },
      "cpu4": {
        "busy": "24"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: cputemps, systeminfo

.. _CPU Temperature:

CPU Temperature
===============

The "cputemps" action returns the temperature of each CPU.

**REST Request**

.. code-block:: json  

 PUT /sysadm/systeminfo
 {
   "action" : "cputemps"
 }

**REST Response**

.. code-block:: json  

 {
    "args": {
        "cputemps": {
            "cpu0": "27.0C",
            "cpu1": "34.0C",
            "cpu2": "33.0C",
            "cpu3": "31.0C"
        }
    }
 }

**WebSocket Request**

.. code-block:: json  

 {
   "args" : {
      "action" : "cputemps"
   },
   "id" : "fooid",
   "name" : "systeminfo",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json  

 {
  "args": {
    "cputemps": {
      "cpu0": "34.0C",
      "cpu1": "32.0C",
      "cpu2": "34.0C",
      "cpu3": "31.0C"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: batteryinfo, systeminfo

.. _Battery Information:

Battery Information
===================

The "batteryinfo" action will indicate whether or not a battery exists. If it does, it will also report its current charge percentage level (1-99). its
status (offline, charging, on backup, or unknown), and estimated time left (in seconds).

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

.. index:: systeminfo

.. _System Information:

System Information
==================

The "systeminfo" action lists system information, including the architecture, number of CPUs, type of CPU, hostname, kernel name and version, system version and patch level, total amount
of RAM, and the system's uptime.

**REST Request**

.. code-block:: json

 PUT /sysadm/systeminfo
 {
   "action" : "systeminfo"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "systeminfo": {
            "arch": "amd64",
            "cpucores": "4",
            "cputype": "Intel(R) Xeon(R) CPU E3-1220 v3 @ 3.10GHz",
            "hostname": "krisdesktop",
            "kernelident": "GENERIC",
            "kernelversion": "10.2-RELEASE-p11",
            "systemversion": "10.2-RELEASE-p12",
            "totalmem": 10720,
            "uptime": "up 2 days 5:09"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "systeminfo"
   },
   "id" : "fooid",
   "name" : "systeminfo",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "systeminfo": {
      "arch": "amd64",
      "cpucores": "4",
      "cputype": "Intel(R) Xeon(R) CPU E3-1220 v3 @ 3.10GHz",
      "hostname": "krisdesktop",
      "kernelident": "GENERIC",
      "kernelversion": "10.2-RELEASE-p11",
      "systemversion": "10.2-RELEASE-p12",
      "totalmem": 10720,
      "uptime": "up 2 days 5:09"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }