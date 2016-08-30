.. _services:

services
********

The services class manages all the background daemons on the system.

Every services class request contains several parameters:

+----------------+------------+---------------------------------------+
| **Parameter**  | **Value**  | **Description**                       |
|                |            |                                       |
+================+============+=======================================+
| id             |            | Any unique value for the request,     |
|                |            | including a hash, checksum, or uuid.  |
+----------------+------------+---------------------------------------+
| name           | services   |                                       |
|                |            |                                       |
+----------------+------------+---------------------------------------+
| namespace      | sysadm     |                                       |
|                |            |                                       |
+----------------+------------+---------------------------------------+
| action         |            | Actions include "disable", "enable",  |
|                |            | "list_services", "restart", "start",  |
|                |            | and "stop"                            |
+----------------+------------+---------------------------------------+

The rest of this section provides examples of the available *actions*
for each type of request, along with their responses.

.. index:: disable, services

.. _disable:

Disable
=======

The :command:`disable` command will tag the service to no longer
startup upon (re)booting the system.

**REST Request**

::

 PUT /sysadm/services
 {
    "action": "disable",
    "services": [
       "cupsd"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
   "args": {
      "action": "disable",
      "services": [
        "cupsd"
      ]
   },
   "name" : "services",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "services_disabled": [
      "cupsd"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }


.. index:: enable, services

.. _enable:

Enable
======

The :command:`enable` command will mark the service for startup upon
(re)booting the system.

**REST Request**

::

 PUT /sysadm/services
 {
    "action": "enable",
    "services": [
       "cupsd"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
   "args": {
      "action": "enable",
      "services": [
        "cupsd"
      ]
   },
   "name" : "services",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "services_enabled": [
      "cupsd"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }


.. index:: list_services, services

.. _list services:

List Services
=============

The list_services action returns a list of all services available on
the system.

.. note:: The response message has been shortened for the example.
   Many more services will be typically listed.

**REST Request**

::

 PUT /sysadm/services
 {
    "action" : "list_services"
 }

**WebSocket Request**

.. code-block:: json

 {
    "args" : {
       "action" : "list_services"
    },
    "id" : "fooid",
    "namespace" : "sysadm",
    "name" : "services"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "services": {
       "accounting": {
         "description": "",
         "is_enabled": "false",
         "name": "accounting",
         "path": "/etc/rc.d/accounting",
         "tag": "accounting_enable",
         "is_running": "false"
       },
     }
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: restart, services

.. _restart:

Restart
=======

:command:`restart` will stop, then immediately start the specified
service(s).

**REST Request**

::

 PUT /sysadm/services
 {
    "action": "restart",
    "services": [
       "cupsd"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
   "args": {
      "action": "restart",
      "services": [
        "cupsd"
      ]
   },
   "name" : "services",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "services_restarted": [
      "cupsd"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: start, services

.. _start:

Start
=====

The :command:`start` command will start a specified service(s).

**REST Request**

::

 PUT /sysadm/services
 {
    "action": "start",
    "services": [
       "cupsd"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
   "args": {
      "action": "start",
      "services": [
        "cupsd"
      ]
   },
   "name" : "services",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "services_started": [
      "cupsd"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }


.. index:: stop, services

.. _stop:

Stop
====

The :command:`stop` command will halt a specified service(s).

**REST Request**

::

 PUT /sysadm/services
 {
    "action": "stop",
    "services": [
       "cupsd"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
   "args": {
      "action": "stop",
      "services": [
        "cupsd"
      ]
   },
   "name" : "services",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "services_stopped": [
      "cupsd"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }