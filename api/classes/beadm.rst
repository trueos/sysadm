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
| action                          |               | supported actions include "listbes"                                                                                  |
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