.. _users:

users
*****

The users class is used to manage users and groups on the system.
Every user request will have several parameters:

+----------------+------------+----------------------------------------+
| **Parameter**  | **Value**  | **Description**                        |
|                |            |                                        |
+================+============+========================================+
| id             |            | any unique value for the request;      |
|                |            | examples include a hash, checksum,     |
|                |            | or uuid                                |
+----------------+------------+----------------------------------------+
| name           | users      |                                        |
|                |            |                                        |
+----------------+------------+----------------------------------------+
| namespace      | sysadm     |                                        |
|                |            |                                        |
+----------------+------------+----------------------------------------+
| action         |            | "groupadd", "groupdelete", "groupmod", |
|                |            | "groupshow", "personacrypt_listdevs",  |
|                |            | "useradd", "userdelete", "usermod",    |
|                |            | "usershow"                             |
+----------------+------------+----------------------------------------+

.. index:: groupadd, users

.. _groupadd:

Group Add
=========

:command:`groupadd` will create a new group on the system. The "name"
field is required, the "gid" and "users" fields are optional.

**REST Request**

::

 PUT /sysadm/users
 {
    "action" : "groupadd",
    "name" : "testgroup"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "namespace" : "sysadm",
    "args" : {
       "action" : "groupadd",
       "name" : "testgroup"
    },
    "name" : "users"
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

.. index:: groupdelete, users

.. _groupdelete:

Group Delete
============

:command:`groupdelete` deletes the designated group.

**REST Request**

::

 PUT /sysadm/users
 {
    "action" : "groupdelete",
    "name" : "groupToDelete"
 }
 
**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "namespace" : "sysadm",
    "args" : {
       "action" : "groupdelete",
       "name" : "<groupToDelete>"
    },
    "name" : "users"
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

.. index:: groupmod, users

.. _groupmod:

Group Modify
============

:command:`goupmod` modifies a given group on the system. There are two
required fields: "name": "<desired group>", and any **one** of three
choices:

* "users": ["<array of users>"] (will set the list of users for this
  group).
* "add_users": ["<array of users>"] (will add the listed users to the
  current users).
* "remove_users": ["<array of users>"] (will remove the listed users
  from the current users).

**REST Request**

::

 PUT sysadm/users
 
 {
    "action": "groupmod"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id":"sample",
   "namespace":"sysadm",
   "name":"users",
   "args":{
     "action":"groupmod",
     "name":"operator",
     "users":["user1","user2"]
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

.. index:: groupshow, users

.. _groupshow:

Group Show
==========

The action :command:`groupshow` lists all the known groups on the
system and any users associated with them (if all access) or which
ones the current user is in (if limited access).

**REST Request**

::

 PUT /sysadm/users

 {
    "action" : "groupshow"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "name" : "users",
    "namespace" : "sysadm",
    "args" : {
       "action" : "groupshow"
    }
 }

**Response**

.. code-block:: json

 {
   "args": {
     "_dhcp": {
       "gid": "65",
       "name": "_dhcp",
       "users": [
         ""
       ]
     },
     "_ntp": {
       "gid": "123",
       "name": "_ntp",
       "users": [
         ""
       ]
     },
     "_pflogd": {
       "gid": "64",
       "name": "_pflogd",
       "users": [
         ""
       ]
     },
     "_tss": {
       "gid": "601",
       "name": "_tss",
       "users": [
         ""
       ]
     },
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: personacryptlistdevs, users

.. _personacryptlistdevs:

Personacrypt List Devices
=========================

:command:`personacrypt_listdevs` will run personacrypt and return any
removeable devices which may be used as PC devices.

**REST Request**

::

 PUT /sysadm/users

 {
    "action" : "personacrypt_listdevs"
 }

**WebSocket Request**

.. code-block:: json

 {
    "namespace" : "sysadm",
    "name" : "users",
    "id" : "fooid",
    "args" : {
       "action" : "personacrypt_listdevs"
    }
 }

**Response**

.. code-block:: json

 {
   "args": {
     "da0": "<SanDisk Cruzer 1.26> 7.5G"
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }

.. index:: useradd, users

.. _useradd:

User Add
========

The `useradd` action will add a new user account on the system. There
are a number of required and optional fields to add to the initial
request:

+---------------+----------------+----------------------------------------------------+
| **Name**      | **Required/**  |        **Description**                             |
|               | **Optional**   |                                                    |
+===============+================+====================================================+
| change        | Optional       | Sets a time for the password to expire on the new  |
|               |                | account.                                           |
+---------------+----------------+----------------------------------------------------+
| class         | Optional       | Sets the login class for the user being created.   |
|               |                |                                                    |
+---------------+----------------+----------------------------------------------------+
| comment       | Optional       | This field sets the contents of the psswd GECOS    |
|               |                | field, which normally contains up to four          |
|               |                | comma-separated fields containing the user's full  |
|               |                | name, location, and work and home phone numbers.   |
+---------------+----------------+----------------------------------------------------+
| expire        | Optional       | Sets account expiration date, formatted as either  |
|               |                | a UNIX time in decimal or a date in 'dd-mmm-yyyy'  |
|               |                | format.                                            |
+---------------+----------------+----------------------------------------------------+
| group         | Optional       | Sets the account's primary group to the given      |
|               |                | group, either by name or group number.             |
+---------------+----------------+----------------------------------------------------+
| home_dir      | Optional       | Sets the account's home directory.                 |
|               |                |                                                    |
+---------------+----------------+----------------------------------------------------+
| name/user id  | Required       | A unique string of characters which identifies the |
| (uid)         |                | new user.                                          |
+---------------+----------------+----------------------------------------------------+
| other_groups  | Optional       | Sets secondary group memberships for an account.   |
|               |                |                                                    |
+---------------+----------------+----------------------------------------------------+
| password      | Required       | Locks the user account unless a unique string of   |
|               |                | characters is typed into the system first.         |
+---------------+----------------+----------------------------------------------------+
| shell         | Optional       | Configure the user's login to a shell program.     |
|               |                | The full path to the shell program is required.    |
+---------------+----------------+----------------------------------------------------+

**REST Request**

::

 PUT /sysadm/users
 
 {
    "password" : "test",
    "name" : "test2",
    "action" : "useradd"
 }

**WebSocket Request**

.. code-block:: json

 {
    "name" : "users",
    "namespace" : "sysadm",
    "id" : "fooid",
    "args" : {
       "password" : "test",
       "name" : "test2",
       "action" : "useradd"
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

.. index:: userdelete, users

.. _userdelete:

User Delete
===========

The :command:`userdelete` action will remove a user account from the
system. The request requires a "name" field with the desired username
value. The optional "clean_home" field will remove the user's home
directory and all files within it. Its default value is "true".

**REST Request**

::

 PUT /sysadm/users

 {
    "name" : "test",
    "action" : "userdelete"
 }

**WebSocket Request**

.. code-block:: json

 {
    "id" : "fooid",
    "name" : "users",
    "args" : {
       "action" : "userdelete",
       "name" : "test"
    },
    "namespace" : "sysadm"
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

.. index:: usermod, users

.. _usermod:

User Mod
========

The :command:`usermod` action is similar to the :command:`useradd`
action, but performs changes to an existing user only. A user with
limited access may modify their own account settings with this command,
but no other user's settings. The field "newname" can also be added to
the request to alter the existing user name.

**REST Request**

::

 PUT /sysadm/users
 
 {
    "action" : "usermod",
    "comment" : "somecomment",
    "name" : "test2"
 }

**WebSocket Request**

.. code-block:: json

 {
    "name" : "users",
    "namespace" : "sysadm",
    "args" : {
       "name" : "test2",
       "comment" : "somecomment",
       "action" : "usermod"
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

.. index:: usershow, users

.. _usershow:

User Show
=========

:command:`usershow` will display all user accounts registered on the
system, regardless of active/inactive status.

**REST Request**

::

 PUT /sysadm/users
 
 {
 "action" : "usershow"
 }

**WebSocket Request**

.. code-block:: json

 {
    "namespace" : "sysadm",
    "name" : "users",
    "id" : "fooid",
    "args" : {
      "action" : "usershow"
    }
 }

**Response**

.. code-block:: json

 {
   "args": {
     "_dhcp": {
       "canremove": "false",
       "change": "0",
       "class": "",
       "comment": "dhcp programs",
       "expire": "0",
       "gid": "65",
       "home_dir": "/var/empty",
       "name": "_dhcp",
       "shell": "/usr/sbin/nologin",
       "uid": "65"
     },
     "_ntp": {
       "change": "0",
       "class": "",
       "comment": "NTP Daemon",
       "expire": "0",
       "gid": "123",
       "home_dir": "/var/empty",
       "name": "_ntp",
       "shell": "/usr/sbin/nologin",
       "uid": "123"
     }
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }
