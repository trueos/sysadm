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
| action                          |               | supported actions include "listvms"                                                                                  |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: listvms, iohyve

.. _List VMs:

List VMs
================

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