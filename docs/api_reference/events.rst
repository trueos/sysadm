.. _Events:

Events
======

The "events" namespace can be used to setup and receive asynchronous
updates about system status and other types of system notifications.

.. warning:: The events namespace does not really translate over to REST
             which was not designed for asynchronous events. For this
             reason, only Websocket examples are used in this section.

Every events request contains several parameters, seen here in
:numref:`Table %s <eventpar>`.

.. _eventpar:
.. table:: Event Request Parameters

   +---------------+-----------+--------------------------------------+
   | **Parameter** | **Value** | **Description**                      |
   |               |           |                                      |
   +===============+===========+======================================+
   | id            |           | Any unique value for the request,    |
   |               |           | including a hash, checksum, or uuid. |
   +---------------+-----------+--------------------------------------+
   | name          |           | Supported values are "subscribe" or  |
   |               |           | "unsubscribe".                       |
   +---------------+-----------+--------------------------------------+
   | namespace     | events    |                                      |
   |               |           |                                      |
   +---------------+-----------+--------------------------------------+
   | args          |           | Values vary by type of class.        |
   |               |           |                                      |
   +---------------+-----------+--------------------------------------+

Subsystems can also be tracked using the events namespace. Currently,
there are three trackable subsystems: Dispatcher, Life Preserver, and
System State. Use this template to subscribe to various subsystem event
notifications:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher", "life-preserver", "system-state"]
 }

Once subscribed, events will be received as they are produced. To
unsubscribe from events, repeat the request, using
:command:`"unsubscribe"` for the :command:`"name"`.

Here is an example reply from the Life Preserver subsystem:

**Websocket Reply**

.. code-block:: json

 {
    "namespace" : "events",
    "name" : "life_preserver",
    "id" : "<none>"
    "args" : {
       "message" : "<message>",
       "priority" : "<number>/<category>",
       "class" : "[snapshot/replication]"
     }
 }

Dispatcher
----------

The Dispatcher subsystem is used by |sysadm| to process external
commands and return specific information from the utility. This is
managed on the server as a separate process, and will not interrupt
primary server tasks. Subscribe to the Dispatcher subsystem for event
updates with this request:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher"]
 }

The Dispatcher event log will display three different states: *pending*,
*running*, and *finished*. Depending upon the current state, the log can
change in some minor but noteworthy ways. The following sample logs will
reflect the differences between these states:

.. note:: The variable **event_system** will only appear when one of
   the three available systems initiates the request and response.
   Additional elements tied to these systems will also appear.

**Dispatcher Response: Pending**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "state" : "pending",
   "process_id" : "<random>"
   }
 }

**Dispatcher Response: Running**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "event_system" : "<sysadm with /pkg, /update, or /iohyve]>",
   "state" : "running",
   "process_id" : "<random>",
   "process_details" : {
     "state" : "running",
     "process_id" : "<random id>",
     "time_started" : "<ISO 8601 time date string>",
     "cmd_list" : ["<command 1>", "<command 2>"],
     "<command1>" : "<log after running command1>",
     "return_codes/<command1>" : "<integer return code>",
     "current_cmd" : "<command 2>",
     }
   }
 }

**Dispatcher Response: Finished**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "event_system" : "<sysadm with /pkg, /update, or /iohyve]>",
   "state" : "finished",
   "process_id" : "<random>",
   "process_details" : {
     "state" : "finished",
     "process_id" : "<random id>",
     "time_started" : "<ISO 8601 time date string>",
     "time_finished" : "<ISO 8601 time date string>",
     "cmd_list" : ["<command 1>", "<command 2>"],
     "<command1>" : "<log after running command1>",
     "<command2>" : "<log after running command2>",
     "return_codes/<command1>" : "<integer return code>",
     "return_codes/<command2>" : "<integer return code>"
     }
   }
 }

Individual classes such as iohyve will move these dispatcher elements
into a "process_details" section. Tailored sample responses will be
provided in these classes' individual documentation pages. Here is a
generalized sample:

**Dispatcher Class Event Message**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "event_system" : "<namespace>/<name>",
    "state" : "<pending/running/finished>",
    "<field>" : "<Other fields depending on class>",
    "process_details" : "<pending, running, or finished message as above>"
    }
 }

For specific details on these special types of events please refer to
the :ref:`classes` section of this guide.

Life Preserver
--------------

Subscribe to the Life Preserver subsystem for event updates with this:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["life-preserver"]
 }

**Websocket Event Message**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "life_preserver",
  "id" : "none",
  "args" : {
    "message" : "<text string>",
    "priority" : "<number/warning level>",
    "class" : "snapshot/replication"
  }
 }

System State
------------

Subscribe to the System State subsystem for event updates with this:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["system-state"]
 }

**Websocket Event Message**

This message will appear if the host name has changed and a priority
error has occurred:

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "system_state",
  "id" : "none",
  "args" : {
    "hostname" : "<name>",
    "hostnamechanged" : "true",
    "zpools" : {
      "<poolname>" : {
        "size" : "<107G>", 
        "alloc" : "<13.1G>",
        "free" : "<93.9G>",
        "frag" : "<6%>",
        "expandsz" : "<->",
        "dedup" : "<1.00x>",
        "altroot" : "<->",
        "capacity" : "<12%>",
        "health" : "<online>",
        "priority" : "<priority>"
      }
    }
  }
 }

Bridge
------

Bridge events are automatically received by any system connected to a
bridge, with no subscription required. This event will get sent out any
time a new connection/disconnection is made from the bridge which
impacts the current connection. A client will only get the event when a
server connects/disconnects or vice versa.

**Websocket Reply: Connected Bridge**

.. code-block:: json

  {
    "id" : "",
    "namespace" : "events",
    "name" : "bridge",
    "args" : {
      "available_connections" : ["ID1", "ID2", "<etc..>"]
      }
  }

.. tip:: Available_connections are **all** the connections available at
         the time, **not** a difference from a previous state. There may
         be both new ID's in the list and ID's which are no longer
         listed.