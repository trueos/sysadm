.. _update:

update
******

The update class is used to check for and manage system and software updates.

Every update class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | update        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "checkupdates"                                                                             |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: checkupdates, update

.. _Check for Updates:

Check for Updates
=================

The "checkupdates" action queries the update server to see if any updates are available. If an update is available, the response will indicate if it is a system security update, an
upgrade to a newer version of the operating system, a system patch, or an update to installed software packages.

**REST Request**

.. code-block:: json

 PUT /sysadm/update
 {
   "action" : "checkupdates"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "checkupdates": {
            "status": "noupdates"
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "update",
   "args" : {
      "action" : "checkupdates"
   },
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "checkupdates": {
      "status": "noupdates"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }