.. _Getting Started:

Getting Started
***************

.. Some intro text here...

Welcome to the SysAdm™ API (application programming interface) documentation handbook!
SysAdm™ is a complex utility with many integral subsystems and classes, and this handbook will attempt to 
guide a new user through the initialization, core features, and intricacies of the utility.

.. Add some links to docs on websockets and json

This handbook will be heavily utilizing the Websocket specification and JSON (JavaScript Object Notation) format.
For detailed descriptions of these items,
please refer to their respective websites at http://www.websocket.org and http://json.org/. 

.. _Authentication:

Authentication
==============

Once a websocket connection is made to the server, the client needs to use the authentication class to authenticate itself to obtain access to the sysadm service. Every authentication
class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | auth          |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | rpc           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| args                            |               | values vary by type of authentication request                                                                        |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+


Several methods are available for authentication. Here is an example of a login using a username and password.

.. note:: when connecting to the localhost, the password field may be left empty for non-root user access.

**WebSocket Request**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth",
  "id" : "sampleID",
  "args" : { 
          "username" : "myuser", 
          "password" : "mypassword" 
          }
  }

Here is an example of using token authentication, where the token is invalidated after 5 minutes of inactivity:
  
**WebSocket Request**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_token",
  "id" : "sampleID",
  "args" : { 
          "token" : "MySavedAuthToken"
          }
  }

Here is an example of using a pre-registered SSL certificate to request authentication. Note that this is a two-step process with only a 30 seconds window of validity, so this is best
left to automated systems rather than direct user requests.

**WebSocket Request (Stage 1 - Initial Request)**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_ssl",
  "id" : "sampleID",
  "args" : ""
  } 

**WebSocket Reply (Stage 1)**

.. code-block:: json

  {
    "args": {
        "test_string" : "<some random plaintext string of letters/numbers>"
    },
    "id": "sampleID",
    "name": "response",
    "namespace": "rpc"
  }

On receipt of the "test_string", the user-side client must encrypt that string with the desired SSL certificate/key combination, then return that encrypted string back to the server
(Stage 2) within 30 seconds of the initial stage 1 reply. The encrypted string should also be base64-encoded before insertion into the stage 2 JSON request to ensure accurate transport
back to the server.

**WebSocket Request (Stage 2 - Return Encoded String)**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_ssl",
  "id" : "sampleID",
  "args" : {
        "encrypted_string" : "<base64-encoded string>"
    }
  }
  
A successful authentication will provide a reply similar to this:

**WebSocket Reply**

.. code-block:: json

  {
    "args": [
        "SampleAuthenticationToken",
        300
    ],
    "id": "sampleID",
    "name": "response",
    "namespace": "rpc"
  }

.. note:: the first element of the "args" array is the authentication token for use later as necessary, while the second element is the number of seconds for which that token is valid.
   The token is reset after every successful communication with the websocket. In this example, it is set to 5 minutes of inactivity before the token is invalidated. The websocket server
   is currently set to close any connection to a client after 10 minutes of inactivity.

An invalid authentication, or a system request after the user session has timed out due to inactivity, looks like this:

**WebSocket Reply**

.. code-block:: json

  {
    "args": {
        "code": 401,
        "message": "Unauthorized"
    },
    "id": "sampleID",
    "name": "error",
    "namespace": "rpc"
  }

To clear a pre-saved authentication token, such as signing out, use this request:
  
**WebSocket Request**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_clear",
  "id" : "sampleID",
  "args" : "junk argument"
  }
  
.. _SSL Certificate Management:

SSL Certificate Management
==========================

Several actions are available for managing the SSL certificates used for authentication.

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | settings      |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "list_ssl_certs", "register_ssl_cert", and "revoke_ssl_cert"                               |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: list_ssl_certs, settings

.. _List SSL Certificates:

List SSL Certificates
---------------------

The "list_ssl_certificates" action lists the known and registered certificates. For each certificate, the response includes the username, public key, and the text of the certificate.

.. index:: register_ssl_cert, settings

.. _Register a SSL Certificate:

Register a SSL Certificate
--------------------------

The "register_ssl_certificate" action registers the specified certificate on the server. Once registered, that user is allowed to authenticate without a password as long as that same
certificate is loaded in any future connections. When using this action, The "pub_key" needs to match the public key of one of the certificates currently loaded into the server/client
connection.

.. index:: revoke_ssl_cert, settings

.. _Revoke a SSL Certificate:

Revoke a SSL Certificate
------------------------

The "revoke_ssl_certificate" action revokes a currently registered certificate so that it can no longer be used for authentication. The "pub_key" must be specified and must match one of the
keys given by the "list_ssl_certs" action, but does not need to match any currently loaded certificates. The "user" is optional and allows a connection with full administrative privileges to
revoke a certificate belonging to another user.

.. note:: if the current user has full administrative access, "list_ssl_certs" will return the registered certificates for all users on the system. Otherwise, it will only return the
   certificates for the current user. Similarly, "revoke_ssl_cert" may be used to remove certificates registered to other users only if the current user/connection has full administrative
   access; otherwise, it may only be used to manage the current user's certificates.
   
.. index:: dispatcher, events   

.. _Dispatcher Subsystem:

Dispatcher Subsystem
====================

The dispatcher subsystem is designed for running external utilities or scripts in an asynchronous fashion. Any connected client can subscribe to per-connection event notifications about
dispatcher processes through the events system, but only users in the *wheel* group have the authority to directly submit new jobs for the dispatcher. 

.. note:: other subsystems may also use the dispatcher for long-running processes in the background,  and these subsystems may allow non-wheel group users to perform these tasks as
   necessary. Also, the events namespace does not really translate over to REST which was not designed for asyncronous events. For this reason, only Websocket examples are used in this
   section.

The format of "dispatcher" event requests is as follows:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | subscribe     | use the desired action                                                                                               |
|                                 | unsubscribe   |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | events        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| args                            | dispatcher    |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

For example, to subscribe to dispatcher events:

.. code-block:: json

  {
  "namespace" : "events",
  "name" : "subscribe",
  "id" : "sampleID",
  "args" : ["dispatcher"]
  }

Once subscribed, the requested events will be received as they are produced. To unsubscribe from event notifications, repeat the request, using "unsubscribe" for the "name". For example,
to unsubscribe from dispatcher events:
  
.. code-block:: json

  {
  "namespace" : "events",
  "name" : "unsubscribe",
  "id" : "sampleID",
  "args" : ["dispatcher"]
  }

This response indicates that a dispatcher event occurred:

.. code-block:: json

  {
  "namespace" : "events",
  "name" : "event",
  "id" : "",
  "args" : {
    "name" : "dispatcher",
    "args" : "<message>"
    }
  }

A "dispatcher" query contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | dispatcher    |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | events        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | "run" is used to submit process commands                                                                             |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

Dispatcher events have the following syntax:

**Websocket Request**

.. code-block:: json   

 {
  "namespace" : "events",
  "name" : "dispatcher",
  "id" : "",
  "args" : {
        "cmd_list" : ["/bin/echo something"],
        "log" : "[Running Command: /bin/echo something ]something\n",
        "proc_id" : "procID",
        "success" : "true",
        "time_finished" : "2016-02-02T13:45:13",
        "time_started" : "2016-02-02T13:45:13"
  }
 }

Any user within the *wheel* group can use the "run" action to submit a new job to the dispatcher:

**REST Request**

.. code-block:: json   

 PUT /rpc/dispatcher
 {
   "action" : "run",
   "procID2" : [
      "echo chainCmd1",
      "echo chainCmd2"
   ],
   "procID1" : "echo sample1"
 }

**REST Response**

.. code-block:: json   

 {
    "args": {
        "started": [
            "procID1",
            "procID2"
        ]
    }
 }

**WebSocket Request**

.. code-block:: json   

 {
   "name" : "dispatcher",
   "namespace" : "rpc",
   "id" : "fooid",
   "args" : {
      "procID1" : "echo sample1",
      "procID2" : [
         "echo chainCmd1",
         "echo chainCmd2"
      ],
      "action" : "run"
   }
 }

**WebSocket Response**

.. code-block:: json   

 {
  "args": {
    "started": [
      "procID1",
      "procID2"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "rpc"
 } 
 
When submitting a job to the dispatcher, keep the following points in mind:

* Process commands are not the same as shell commands. A dispatcher process command uses the syntax "<binary/utility> <list of arguments>", similar to a simple shell  command. However,
  complex shell operations with pipes or test statements will not function properly within a dispatcher process.

* There are two types of jobs: a single string entry for simple commands, and an array of strings for a chain of commands. A chain of commands is treated as a single process, and the
  commands are run sequentially until either a command fails (returns non-0 or the process crashes), or until there are no more commands to run.

* A chain of commands is useful for multi-step operations but is not considered a replacement for a good shell script on the server.
 
.. index:: query, identify, rpc
   
.. _Server Subsystems:

Server Subsystems
=================

An RPC query can be issued to probe all the known subsystems and return which ones are currently available and what level of read and write access the user has.
An RPC query contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | query         |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | rpc           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| args                            |               | can be any data                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /rpc/query
 {
   "junk" : "junk"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "rpc/dispatcher": "read/write",
        "rpc/syscache": "read",
        "sysadm/lifepreserver": "read/write",
        "sysadm/network": "read/write"
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "query",
   "namespace" : "rpc",
   "args" : {
      "junk" : "junk"
   }
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "rpc/dispatcher": "read/write",
    "rpc/syscache": "read",
    "sysadm/lifepreserver": "read/write",
    "sysadm/network": "read/write"
  },
  "id": "fooid",
  "name": "response",
  "namespace": "rpc"
 }

To identify the type of SysAdm system, use an "identify" query. Possible identities are "server", "bridge", and "client".

This type of query contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | identify      |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | rpc           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| args                            |               | can be any data                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

**REST Request**

.. code-block:: json

 PUT /rpc/identify
 {}

**WebSocket Request**

.. code-block:: json

 {
   "args" : {},
   "namespace" : "rpc",
   "id" : "fooid",
   "name" : "identify"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "type": "server"
  },
  "id": "fooid",
  "name": "response",
  "namespace": "rpc"
 }