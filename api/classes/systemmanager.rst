.. _systemmanager:

systemmanager
*************

The systemmanager class is used to retrieve information about the system. Every systemmanager class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | systemmanager |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "memorystats", "cpupercentage", "cputemps", "procinfo", "batteryinfo", "externalmounts",   |
|                                 |               | "systemmanager"                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. index:: memorystats, systemmanager

.. _Memory Statistics:

Memory Statistics
=================

The "memorystats" action returns memory statistics, including the amount of active, cached, free, inactive, and total physical (wired) memory.

**REST Request**

.. code-block:: json

 PUT /sysadm/systemmanager
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
   "name" : "systemmanager"
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

.. index:: cpupercentage, systemmanager

.. _CPU Usage:

CPU Usage
=========

The "cpupercentage" action returns the usage percentage of each CPU.

**REST Request**

.. code-block:: json 

 PUT /sysadm/systemmanager
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
   "name" : "systemmanager",
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
 
.. index:: cputemps, systemmanager

.. _CPU Temperature:

CPU Temperature
===============

The "cputemps" action returns the temperature of each CPU.

**REST Request**

.. code-block:: json  

 PUT /sysadm/systemmanager
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
   "name" : "systemmanager",
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
 
.. index:: procinfo, systemmanager

.. _Process Information:

Process Information
===================

The "procinfo" action lists information about each running process. Since a system will have many running processes, the responses in this section only show one process as an example
of the type of information listed by this action.

**REST Request**

.. code-block:: json 

 PUT /sysadm/systemmanager
 {
   "action" : "procinfo"
 }

**REST Response**

.. code-block:: json 

 {
    "args": {
        "procinfo": {
                  "228": {
        "command": "adjkerntz",
        "cpu": "3",
        "nice": "0",
        "pri": "52",
        "res": "1968K",
        "size": "8276K",
        "state": "pause",
        "thr": "1",
        "time": "0:00",
        "username": "root",
        "wcpu": "0.00%"
          }
        }
    }
 }

**WebSocket Request**

.. code-block:: json 

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "systemmanager",
   "args" : {
      "action" : "procinfo"
   }
 }

**WebSocket Response**

.. code-block:: json 

 {
  "args": {
    "procinfo": {
      "228": {
        "command": "adjkerntz",
        "cpu": "3",
        "nice": "0",
        "pri": "52",
        "res": "1968K",
        "size": "8276K",
        "state": "pause",
        "thr": "1",
        "time": "0:00",
        "username": "root",
        "wcpu": "0.00%"
      }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: batteryinfo, systemmanager

.. _Battery Information:

Battery Information
===================

The "batteryinfo" action will indicate whether or not a battery exists. If it does, it will also report its current charge percentage level (1-99). its
status (offline, charging, on backup, or unknown), and estimated time left (in seconds).

**REST Request**

.. code-block:: json

 PUT /sysadm/systemmanager
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
   "name" : "systemmanager",
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

.. index:: externalmounts, systemmanager

.. _List External Mounts:

List External Mounts
====================

The "externalmounts" action returns a list of mounted external devices. Supported device types are UNKNOWN, USB, HDRIVE (external hard drive), DVD, and SDCARD.
For each mounted device, the response will include the device name, filesystem, mount path, and device type.

**REST Request**

.. code-block:: json

 PUT /sysadm/systemmanager
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
   "name" : "systemmanager",
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

.. index:: systemmanager

.. _System Information:

System Information
==================

The "systemmanager" action lists system information, including the architecture, number of CPUs, type of CPU, hostname, kernel name and version, system version and patch level, total amount
of RAM, and the system's uptime.

**REST Request**

.. code-block:: json

 PUT /sysadm/systemmanager
 {
   "action" : "systemmanager"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "systemmanager": {
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
      "action" : "systemmanager"
   },
   "id" : "fooid",
   "name" : "systemmanager",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "systemmanager": {
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