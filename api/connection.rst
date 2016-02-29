.. _Getting Started:

Getting Started
***************

Some intro text here...


Add some links to docs on websockets and json....

.. _Authentication:

Authentication
==============

Once a websocket connection is made to the server, the client needs to use the authentication class to authenticate itself to obtain access to the sysadm service. Every authentciation class
request contains the following parameters:

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


Several methods are available for authentication. Here is an example of a login using a username and password:
(Note: When connecting to the localhost, the password field may be left empty for non-root user access).

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

Here is an example of using a pre-registered SSL certificate to request authentication:
(Note: This is a two step process with only a 30 seconds window of validity, so this is best left up to automated systems rather than direct user requests).

**WebSocket Request (Stage 1 - Initial Request)**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_ssl",
  "id" : "sampleID",
  "args" : ""
  }
..  
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
..

On receipt of the test_string, the user-side client must encrypt that string with the desired SSL certificate/key combination, then return that encrypted string back to the server (Stage 2) within 30 seconds of the initial stage 1 reply. The encrypted string should also be base64-encoded before insertion into the stage 2 JSON request to ensure accurate transport back to the server.

**WebSocket Request (Stage 2 - Return encoded string)**

.. code-block:: json

  {
  "namespace" : "rpc",
  "name" : "auth_ssl",
  "id" : "sampleID",
  "args" : {
        "encrypted_string" : "<base64-encoded string>"
    }
  }
.. 
  
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

.. _note: the first element of the *"args"* array is the authentication token for use later as necessary, while the second element is the number of seconds for which that token is valid.
   The token is reset after every successful communication with the websocket. In this example, it is set to 5 minutes of inactivity before the token is invalidated. The websocket server is
   currently set to close any connection to a client after 10 minutes of inactivity.

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

.. _Server Subsystems:

Server Subsystems
=================

An RPC query can be issued to probe all the known subsystems and return which ones are currently available and what level of read and write access the user has.
A query contains the following parameters:

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
