.. _zfs:

zfs
***

The zfs class is used to manage and retrieve information about ZFS pools.

Every zfs class request contains the following parameters:

+---------------------------------+---------------+------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                              |
|                                 |               |                                                                              |
+=================================+===============+==============================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid |
|                                 |               |                                                                              |
+---------------------------------+---------------+------------------------------------------------------------------------------+
| name                            | zfs           |                                                                              |
|                                 |               |                                                                              |
+---------------------------------+---------------+------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                              |
|                                 |               |                                                                              |
+---------------------------------+---------------+------------------------------------------------------------------------------+
| action                          |               | supported actions include "list_pools", "datasets"                           |
|                                 |               |                                                                              |
+---------------------------------+---------------+------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* 
for each type of request, along with their responses. 

.. index:: list_pools, zfs

.. _List Pools:

List Pools
==========

The "list_pools" action lists pool information. For each ZFS pool, the 
response includes the pool name, the amount of space that has been 
physically allocated, whether or not an alternate root has been defined,
capacity (percent used), the deduplication ratio, amount of 
uninitialized space (expandsz, which usually applies to LUNs), 
percentage of fragmentation, amount of free space, pool health, and 
total size. This action is the equivalent of running 
:command:`zpool list` from the command line.

**REST Request**

::

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

.. index:: datasets, zfs

.. _List Datasets:

List Datasets
=============

The "datasets" action lists the ZFS datasets on the specified pool.

**REST Request**

::

 PUT /sysadm/zfs
 {
   "action" : "datasets",
   "zpool" : "tank"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "action" : "datasets",
      "zpool" : "tank"
   },
   "name" : "zfs"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "datasets": {
      "tank": {
        "avail": "320G",
        "mountpoint": "none",
        "refer": "96K",
        "used": "125G"
      },
      "tank/ROOT": {
        "avail": "320G",
        "mountpoint": "none",
        "refer": "96K",
        "used": "63.7G"
      },
      "tank/ROOT/11.0-CURRENTAPRIL2016-up-20160418_124146": {
        "avail": "320G",
        "mountpoint": "/",
        "refer": "34.7G",
        "used": "63.7G"
      },
      "tank/ROOT/11.0-CURRENTFEB2016-up-20160303_094216": {
        "avail": "320G",
        "mountpoint": "/",
        "refer": "29.7G",
        "used": "272K"
      },
      "tank/ROOT/11.0-CURRENTMAR2016-up-20160304_102405": {
        "avail": "320G",
        "mountpoint": "/",
        "refer": "30.5G",
        "used": "248K"
      },
      "tank/ROOT/11.0-CURRENTMAR2016-up-20160315_092952": {
        "avail": "320G",
        "mountpoint": "/",
        "refer": "31.2G",
        "used": "256K"
      },
      "tank/ROOT/11.0-CURRENTMAR2016-up-20160318_090405": {
        "avail": "320G",
        "mountpoint": "/",
        "refer": "34.7G",
        "used": "280K"
      },
      "tank/ROOT/initial": {
        "avail": "320G",
        "mountpoint": "/mnt",
        "refer": "5.60G",
        "used": "232K"
      },
      "tank/iocage": {
        "avail": "320G",
        "mountpoint": "/iocage",
        "refer": "152K",
        "used": "1.13G"
      },
      "tank/iocage/.defaults": {
        "avail": "320G",
        "mountpoint": "/iocage/.defaults",
        "refer": "96K",
        "used": "992K"
      },
      "tank/iocage/download": {
        "avail": "320G",
        "mountpoint": "/iocage/download",
        "refer": "96K",
        "used": "203M"
      },
      "tank/iocage/download/10.2-RELEASE": {
        "avail": "320G",
        "mountpoint": "/iocage/download/10.2-RELEASE",
        "refer": "202M",
        "used": "202M"
      },
      "tank/iocage/jails": {
        "avail": "320G",
        "mountpoint": "/iocage/jails",
        "refer": "104K",
        "used": "1000K"
      },
      "tank/iocage/releases": {
        "avail": "320G",
        "mountpoint": "/iocage/releases",
        "refer": "96K",
        "used": "953M"
      },
      "tank/iocage/releases/10.2-RELEASE": {
        "avail": "320G",
        "mountpoint": "/iocage/releases/10.2-RELEASE",
        "refer": "96K",
        "used": "952M"
      },
      "tank/iocage/releases/10.2-RELEASE/root": {
        "avail": "320G",
        "mountpoint": "/iocage/releases/10.2-RELEASE/root",
        "refer": "825M",
        "used": "951M"
      },
      "tank/iocage/templates": {
        "avail": "320G",
        "mountpoint": "/iocage/templates",
        "refer": "96K",
        "used": "992K"
      },
      "tank/iohyve": {
        "avail": "320G",
        "mountpoint": "/iohyve",
        "refer": "96K",
        "used": "22.8G"
      },
      "tank/iohyve/Firmware": {
        "avail": "320G",
        "mountpoint": "/iohyve/Firmware",
        "refer": "96K",
        "used": "992K"
      },
      "tank/iohyve/ISO": {
        "avail": "320G",
        "mountpoint": "/iohyve/ISO",
        "refer": "96K",
        "used": "453M"
      },
      "tank/iohyve/ISO/FreeBSD-10.1-RELEASE-amd64-bootonly.iso": {
        "avail": "320G",
        "mountpoint": "/iohyve/ISO/FreeBSD-10.1-RELEASE-amd64-bootonly.iso",
        "refer": "219M",
        "used": "220M"
      },
      "tank/iohyve/ISO/FreeBSD-10.2-RELEASE-amd64-bootonly.iso": {
        "avail": "320G",
        "mountpoint": "/iohyve/ISO/FreeBSD-10.2-RELEASE-amd64-bootonly.iso",
        "refer": "231M",
        "used": "232M"
      },
      "tank/iohyve/bsdguest": {
        "avail": "320G",
        "mountpoint": "/iohyve/bsdguest",
        "refer": "96K",
        "used": "22.4G"
      },
      "tank/iohyve/bsdguest/disk0": {
        "avail": "341G",
        "mountpoint": "-",
        "refer": "1.75G",
        "used": "22.4G"
      },
      "tank/tmp": {
        "avail": "320G",
        "mountpoint": "/tmp",
        "refer": "2.95M",
        "used": "18.3M"
      },
      "tank/usr": {
        "avail": "320G",
        "mountpoint": "none",
        "refer": "96K",
        "used": "37.5G"
      },
      "tank/usr/home": {
        "avail": "320G",
        "mountpoint": "/usr/home",
        "refer": "96K",
        "used": "27.8G"
      },
      "tank/usr/home/kris": {
        "avail": "320G",
        "mountpoint": "/usr/home/kris",
        "refer": "21.9G",
        "used": "27.8G"
      },
      "tank/usr/jails": {
        "avail": "320G",
        "mountpoint": "/usr/jails",
        "refer": "96K",
        "used": "992K"
      },
      "tank/usr/obj": {
        "avail": "320G",
        "mountpoint": "/usr/obj",
        "refer": "4.68G",
        "used": "4.75G"
      },
      "tank/usr/ports": {
        "avail": "320G",
        "mountpoint": "/usr/ports",
        "refer": "2.20G",
        "used": "2.96G"
      },
      "tank/usr/src": {
        "avail": "320G",
        "mountpoint": "/usr/src",
        "refer": "1.82G",
        "used": "2.01G"
      },
      "tank/var": {
        "avail": "320G",
        "mountpoint": "none",
        "refer": "96K",
        "used": "13.5M"
      },
      "tank/var/audit": {
        "avail": "320G",
        "mountpoint": "/var/audit",
        "refer": "96K",
        "used": "992K"
      },
      "tank/var/log": {
        "avail": "320G",
        "mountpoint": "/var/log",
        "refer": "1.43M",
        "used": "5.21M"
      },
      "tank/var/mail": {
        "avail": "320G",
        "mountpoint": "/var/mail",
        "refer": "120K",
        "used": "1.21M"
      },
      "tank/var/tmp": {
        "avail": "320G",
        "mountpoint": "/var/tmp",
        "refer": "3.20M",
        "used": "5.99M"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }