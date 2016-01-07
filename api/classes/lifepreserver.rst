.. _lifepreserver:

lifepreserver
*************

The lifepreserver class is used to manage and retrieve information about scheduled snapshots and replication.

Every lifepreserver class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | lifepreserver |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include listcron                                                                                   |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses.

.. _List Schedules:

List Schedules
==============

The "listcron" action retrieves the information for each scheduled snapshot. For each schedule, it lists the name of the ZFS pool, the number of snapshots to keep, and the time that the
snapshot is taken.

**REST Request**

.. code-block:: json

 PUT /sysadm/lifepreserver
 {
   "action" : "listcron"
 }


**REST Response**

.. code-block:: json

 {
    "args": {
        "listcron": {
            "tank1": {
                "keep": "3",
                "schedule": "daily@18"
            }
        }
    }
 }
 
**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "listcron"
   },
   "name" : "lifepreserver",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listcron": {
      "tank1": {
        "keep": "3",
        "schedule": "daily@18"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }