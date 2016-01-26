.. _Events:

Events
******

The "events" namespace can be used to setup and receive asyncronous updates about system status and other types of system notifications.

.. _note: the events namespace does not really translate over to REST which was not designed for asyncronous events. For this reason, only Websocket examples are used in this section.

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

Here is an example of subscribing to Life Preserver events:

**Websocket Request**

.. code-block:: json

 {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher", "life-preserver"]
 }
 
Once subscribed, events will be received as they are produced. To unsubscribe from events, repeat the request, using "unsubscribe" for the "name". 

Here is an example reply:
 
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

