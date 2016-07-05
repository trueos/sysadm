.. _fs:

fs
**

The fs class is used to manage the files and directories on the system.

Every fs class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | fs            |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "dirlist"                                                                                  |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* 
for each type of request, along with their responses. 

.. index:: dirlist, fs

.. _List Directory:

List Directory
==============

The "dirlist" action lists the contents of the specified directory.

**REST Request**

.. code-block:: json

 PUT /sysadm/fs
 {
   "dir" : "/root",
   "action" : "dirlist"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "dir" : "/root",
      "action" : "dirlist"
   },
   "id" : "fooid",
   "name" : "fs",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "dirlist": {
      "VirtualBox VMs": {
        "dir": true
      },
      "freenas-auto.iso": {
        "group": "wheel",
        "owner": "root",
        "size": 408049664
      },
      "ixbuild": {
        "dir": true
      },
      "pc-sysinstall.cfg": {
        "group": "wheel",
        "owner": "root",
        "size": 1741
      },
      "pc-sysinstall.log": {
        "group": "wheel",
        "owner": "root",
        "size": 125408
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }