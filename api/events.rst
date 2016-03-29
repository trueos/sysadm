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

The Dispatcher subsystem is used by sysadm to process external commands and return specific information from the utility.
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

Dispatcher has two different kinds of websocket messages, general process notifications and specialized subsystem process notifications.
The general process notifications will appear as follows:

**Websocket Event Message (Process Starting)**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "process_id" : "<some id string>", 
    "state" : "running"
    }
 }

..
  .. code-block:: json

    {
      "namespace" : "events",
      "name" : "dispatcher",
      "id" : "none",
      "args" : {
      "time_started" : <ISO 8601 time date string>, 
      "cmd_list" : [ "<command 1>", "<command 2>"],
      "process_id" : "<some id string>", 
      "state" : "running"
      }
    }

**Websocket Event Message (Process Complete)**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "time_finished" : <ISO 8601 time date string>,
    "cmd_list" : [ "<command 1>", "<command 2>"],
    "process_id" : "<some id string>",
    "state" : "finished",
    "return_codes"
    }
 }

Particular classes within sysadm may return information through the Dispatcher events system. These types of messages are slightly different in output format, which is noted here:

**Websocket Event Message**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "none",
  "args" : {
    "event_system" : <namespace>/<name>,
    "state" : "running" OR "finished",
    <Other fields depending on subsystem>
    "process_details" : {
      "time_started" : <ISO 8601 time date string>, 
      "cmd_list" : [ "<command 1>", "<command 2>"],
      "process_id" : "<some id string>", 
      "state" : "running"
      }
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
 