.. _firewall:

firewall
********

The firewall class is used for managing the "ipfw" firewall on the system.

.. note:: This class does *not* manage a "pf" firewall. 

Every user request will have several parameters:

+---------------+-----------+---------------------------------------+
| **Parameter** | **Value** | **Description**                       |
|               |           |                                       |
+===============+===========+=======================================+
| id            |           | any unique value for the request;     |
|               |           | examples include a hash, checksum,    |
|               |           | or uuid                               |
+---------------+-----------+---------------------------------------+
| name          | firewall  |                                       |
|               |           |                                       |
+---------------+-----------+---------------------------------------+
| namespace     | sysadm    |                                       |
|               |           |                                       |
+---------------+-----------+---------------------------------------+
| action        |           | "known_ports", "list_open", "status", |
|               |           | "open", "close", "start", "stop",     |
|               |           | "restart", "enable", "disable", and   |
|               |           | "reset-defaults"                      |
+---------------+-----------+---------------------------------------+

.. index:: knownports, firewall

.. _knownports:

Known Ports
===========

:command:`known_ports` will return a list of all known ports and any
names or descriptions for them. This is a static list; it does not
reflect which ports are in use or opened on the system. It is meant to
help match a port to a name or description.

**REST Request**

::

 PUT /sysadm/firewall
 {
    "action" : "known_ports"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "namespace" : "sysadm",
    "args" : {
       "action" : "known_ports"
    },
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "1/tcp": {
       "description": "#TCP Port Service Multiplexer",
       "name": "tcpmux",
       "port": "1/tcp"
     },
     "1/udp": {
       "description": "#TCP Port Service Multiplexer",
       "name": "tcpmux",
       "port": "1/udp"
     },
     "100/tcp": {
       "description": "#[unauthorized use]",
       "name": "newacct",
       "port": "100/tcp"
     }
   }
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: listopen, firewall

.. _listopen:

List Open
=========

:command:`list_open` returns an array of all the open port/type
combinations for the firewall.

**REST Request**

.. code-block:: none
 
 PUT /sysadm/firewall
 {
    "action" : "list_open"
 }

**WebSocket Request**

.. code-block:: json

 {
    "args" : {
       "action" : "list_open"
    },
    "id" : "fooid",
    "name" : "firewall",
    "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "openports": [
       "5353/udp"
     ]
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: status, firewall

.. _status:

Status
======

:command:`status` returns the current state of the firewall.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "status"
 }

**WebSocket Request**

.. code-block:: json

 {
    "name" : "firewall",
    "args" : {
       "action" : "status"
    },
    "id" : "fooid",
    "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "is_enabled": "true",
     "is_running": "true"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: open, firewall

.. _open:

Open
====

:command:`open` will allow traffic through a specified port. The
action requires the argument::

 "ports":[<number>/<type>, <number2>/<type2>]

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "open",
    "ports" : [
       "12151/tcp"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
    "namespace" : "sysadm",
    "name" : "firewall",
    "args" : {
       "ports" : [
          "12151/tcp"
       ],
       "action" : "open"
    },
    "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: close, firewall

.. _close:

Close
=====

:command:`close` will close the designated ports in the firewall. An
additional statement is required: ::

 "ports":["<number>/<type>", "<number2>"/"<type2>"]

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "close",
    "ports" : [
       "12151/tcp"
    ]
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "namespace" : "sysadm",
    "name" : "firewall",
    "args" : {
       "ports" : [
          "12151/tcp"
       ],
       "action" : "close"
    }
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: start, firewall

.. _firewallstart:

Start
=====

:command:`start` will turn on the firewall.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "start"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "start"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: stop, firewall

.. _firewallstop:

Stop
====

:command:`stop` will turn off the firewall.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "stop"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "stop"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: restart, firewall

.. _firewallrestart:

Restart
=======

:command:`restart` will reload the firewall. This will catch any
settings changes and is not generally needed.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "restart"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "restart"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: enable, firewall

.. _firewallenable:

Enable
======

:command:`enable` will automatically start the firewall on bootup.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "enable"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "enable"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: disable, firewall

.. _firewalldisable:

Disable
=======

:command:`disable` Flags the system to not start the firewall on bootup.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "disable"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "disable"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: resetdefaults, firewall

.. _resetdefaults:

Reset Defaults
==============

:command:`reset-defaults` will reset all the firewall settings back
to the defaults and restart the firewall.

.. warning:: This will only work in |trueos|; the API call will return
   an error if used with FreeBSD.

**REST Request**

.. code-block:: none

 PUT /sysadm/firewall
 {
    "action" : "reset-defaults"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "args" : {
       "action" : "reset-defaults"
    },
    "namespace" : "sysadm",
    "name" : "firewall"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "result": "success"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }