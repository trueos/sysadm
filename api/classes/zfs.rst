.. _zfs:

zfs
***

The zfs class is used to manage and retrieve information about ZFS pools.

Every zfs class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | zfs           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "list_pools"                                                                               |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: list_pools, zfs

.. _List Pools:

List Pools
==========

The "list_pools" action lists pool information. For each ZFS pool, the response includes the pool name, the amount of space that has been physically allocated, whether or not an alternate
root has been defined, capacity (percent used), the deduplication ratio, amount of uninitialized space (expandsz, which usually applies to LUNs), percentage of fragmentation, amount of free
space, pool health, and total size. This action is the equivalent of running :command:`zpool list` from the command line.

**REST Request**

.. code-block:: json

 PUT /sysadm/zfs
 {
   "action" : "list_pools"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "list_pools"
   },
   "name" : "zfs",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
   "list_pools" : {
    "tank": {
      "alloc": "71.8G",
      "altroot": "-",
      "cap": "32%",
      "dedup": "1.00x",
      "expandsz": "-",
      "frag": "18%",
      "free": "148G",
      "health": "ONLINE",
      "size": "220G"
    }
   }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
