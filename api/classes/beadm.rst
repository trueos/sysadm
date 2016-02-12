.. _beadm:

beadm
*****

The beadm class is used to manage boot environments.

Every beadm class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | beadm         |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "listbes", "renamebe", "activatebe", "createbe", "destroybe", and "mountbe"                |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: listbes, beadm

.. _List Boot Environments:

List Boot Environments
======================

The "listbes" action retrieves the list of boot environments. For each boot environment, the response includes its name, its flags (where  "R" is active on reboot, "N" is active now and
"-" is inactive), the date it was created, its mount point, its nickname, and its size.


**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "action" : "listbes"
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "beadm",
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "action" : "listbes"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "listbes": {
      "11.0-CURRENTJAN2016-up-20160128_150853": {
        "active": "NR",
        "date": "2016-01-28",
        "mount": "/",
        "nick": "14:57",
        "space": "10.2G"
      },
      "initial": {
        "active": "-",
        "date": "2016-01-28",
        "mount": "-",
        "nick": "07:00",
        "space": "1.2G"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: renamebe, beadm

.. _Rename a Boot Environment:

Rename a Boot Environment
=========================

The "renamebe" action renames the specified boot environment. When using this action, specify the new name as the "source" and the boot environment as the "target".


**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "source" : "newname",
   "action" : "renamebe",
   "target" : "bootthingy"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "name" : "beadm",
   "id" : "fooid",
   "args" : {
      "source" : "newname",
      "target" : "bootthingy",
      "action" : "renamebe"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "renamebe": {
      "source": "newname",
      "target": "bootthingy"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: activatebe, beadm

.. _Activate Boot Environment:

Activate Boot Environment
=========================

The "activatebe" action activates the specified boot environment (target) so that it will be the default at next boot.


**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "target" : "bootthingy",
   "action" : "activatebe"
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "beadm",
   "args" : {
      "action" : "activatebe",
      "target" : "bootthingy"
   },
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "activatebe": {
      "target": "bootthingy"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: createbe, beadm

.. _Create Boot Environment:

Create Boot Environment
=======================

The "create" action creates a new boot environment. Specify the name of the boot environment as the "newbe". By default, this action clones the active boot environment. 
To specify another, inactive boot environment, also include "clonefrom" to specify which boot environment to clone from.

**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "action" : "createbe",
   "newbe" : "red",
   "clonefrom" : "green"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "newbe" : "red",
      "clonefrom" : "green",
      "action" : "createbe"
   },
   "namespace" : "sysadm",
   "name" : "beadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "createbe": {
      "clonefrom": "green",
      "newbe": "red"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: destroybe, beadm

.. _Destroy a Boot Environment:

Destroy a Boot Environment
==========================

The "destroybe" action destroys the specified "target" boot environment and forcefully unmounts it.


**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "target" : "red",
   "action" : "destroybe"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "destroybe",
      "target" : "red"
   },
   "name" : "beadm",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "destroybe": {
      "target": "red"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 } 
 
.. index:: mountbe, beadm

.. _Mount a Boot Environment:

Mount a Boot Environment
========================

The "mountbe" action mounts the specified boot environment. Use the optional "mountpoint" argument to specify the mount point.

**REST Request**

.. code-block:: json

 PUT /sysadm/beadm
 {
   "mountpoint" : "/tmp/mounteddir/",
   "action" : "mountbe",
   "be" : "green"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "beadm",
   "args" : {
      "mountpoint" : "/tmp/mounteddir/",
      "be" : "green",
      "action" : "mountbe"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "mountbe": {
      "be": "green",
      "mountpoint": "/tmp/mountdir"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }