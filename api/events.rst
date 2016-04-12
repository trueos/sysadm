.. _Events:

Events
======

The "events" namespace can be used to setup and receive asynchronous updates about system status and other types of system notifications.

.. note:: 
   the events namespace does not really translate over to REST which was not designed for asynchronous events. For this reason, only Websocket examples are used in this section.

Every events request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            |               | supported values are "subscribe" or unsubscribe"                                                                     |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | events        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| args                            |               | values vary by type of class                                                                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+


Subsystems can also be tracked using the events namespace. Currently, there are three trackable subsystems: Dispatcher, Life Preserver, and System State.
The following is a template to subscribe to various subsystem event notifications:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher", "life-preserver", "system-state"]
 }
 
Once subscribed, events will be received as they are produced. To unsubscribe from events, repeat the request, using "unsubscribe" for the "name". 

Here is an example reply from the Life Preserver subsystem:
 
**Websocket Reply**

.. code-block:: json

 {
    "namespace" : "events",
    "name" : "life-preserver",
    "id" : "<none>"
    "args" : {
       "message" : <message>,
       "priority" : "<number> -
  <category>",
       "class" : "[snapshot/replication]"
     }
 }

Dispatcher
----------

The Dispatcher subsystem is used by SysAdm™ to process external commands and return specific information from the utility.
This is managed on the server as a separate process, and will not interrupt primary server tasks.
To subscribe to the Dispatcher subsystem for event updates, use the following:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher"]
 }

The Dispatcher event log will display three different states: "pending", "running", and "finished".
Depending upon the current state, the log can change in some minor but noteworthy ways.
The following sample logs will reflect the differences between these states:

**Dispatcher Response: "Pending" state**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "state" : "pending",
   "cmd_list" : ["<command 1>", "<command 2>"],
   "process_id" : "<random>"
   }
 }

**Dispatcher Response: "Running" state**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "state" : "running",
   "cmd_list" : ["<command 1>", "<command 2>"],
   "process_id" : "<random>",
   "time_started" : "<ISO 8601 time date string>",
   "current_cmd" : "<command 2>",
   "<command1>" : "<log after running command1>",
   "<command2>" : "<log for command2>",
   "return_codes/<command1>" : "<integer return code>"
   }
 }

**Dispatcher Response: "Finished" state**

.. code-block:: json

 {
 "namespace" : "events",
 "name" : "dispatcher",
 "id" : "none",
 "args" : {
   "state" : "finished",
   "time_finished" : "<ISO 8601 time date string>",
   "cmd_list" : ["<command 1>", "<command 2>"],
   "return_codes/<command 1>" : "<code 1>",
   "return_codes/<command 2>" : "<code 2>",
   "process_id" : "<random>"
   }
 }


Individual classes such as iohyve will move these dispatcher elements into a "process_details" section.
Tailored sample responses will be provided in these classes' individual documentation pages.
A generalized sample is as follows:

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

For specific details on these special types of events please refer to the Classes section of this User Guide.

Life Preserver
--------------

To subscribe to the Life Preserver subsystem for event updates, use the following:

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
    "priority" : "<number - warning level>",
    "class" : "snapshot" OR "replication"
  }
 }
 
System State
------------

To subscribe to the System State subsystem for event updates, use the following:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["system-state"]
 }

**Websocket Event Message**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "system_state",
  "id" : "none",
  "args" : {
    "hostname" : "<name>",
    "hostnamechanged" : "true",		(only if host name changed)
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
        "priority" : "<priority>" (if error)
      }
    }
  }
 }
 