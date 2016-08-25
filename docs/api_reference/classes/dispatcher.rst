.. _dispatcher:

dispatcher
**********

The dispatcher class is used to spin up external processes on demand,
such as a user running a custom system setup script.

Every dispatcher class request contains several parameters:

+---------------+------------+--------------------------------------+
| **Parameter** | **Value**  | **Description**                      |
|               |            |                                      |
+===============+============+======================================+
| id            |            | Any unique value for the request,    |
|               |            | including a hash, checksum, or uuid. |
+---------------+------------+--------------------------------------+
| name          | dispatcher |                                      |
|               |            |                                      |
+---------------+------------+--------------------------------------+
| namespace     | rpc        |                                      |
|               |            |                                      |
+---------------+------------+--------------------------------------+
| action        |            | Actions include "list" and "kill".   |
|               |            |                                      |
+---------------+------------+--------------------------------------+

The rest of this section provides examples of the available *actions*
for each type of request, along with their responses.

.. index:: list, dispatcher

.. _List Processes:

List Processes
==============

The "list" action lists all the currently running or pending processes
within the dispatcher queues. Possible queues are "no_queue",
"pkg_queue", and "iocage_queue".

**REST Request**

::

 PUT /rpc/dispatcher
 {
   "action" : "list"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "list"
   },
   "namespace" : "rpc",
   "name" : "dispatcher",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "jobs": {
      "pkg_queue": {
        "sysadm_pkg_install-{9c079421-ace9-4b6e-8870-d023b48f4c49}": {
          "commands": [
            "pkg install -y --repository \"pcbsd-major\"misc/pcbsd-meta-mate"
          ],
          "queue_position": "0",
          "state": "running"
        }
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "rpc"
 }

.. index:: kill, dispatcher

.. _Kill Processes:

Kill Processes
==============

The "kill" action allows a user with full access to cancel pending or
running jobs within the dispatcher system.

**REST Request**

::

 PUT /rpc/dispatcher
 {
   "action" : "kill",
   "job_id" : "sysadm_pkg_install-{9c079421-ace9-4b6e-8870-d023b48f4c49}"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "kill",
      "job_id" : "sysadm_pkg_install-{9c079421-ace9-4b6e-8870-d023b48f4c49}"
   },
   "namespace" : "rpc",
   "name" : "dispatcher",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "killed": {
      "jobs": ["sysadm_pkg_install-{9c079421-ace9-4b6e-8870-d023b48f4c49}"]
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "rpc"
 }