.. _intro:

Introduction
============

**Preface**

Written by users of the |sysadm| management utility.

Version |version|

Copyright © 2016 iXSystems®.

The |sysadm| Server Handbook is freely available for sharing and
redistribution under the terms of the
`Creative Commons Attribution License <https://creativecommons.org/licenses/by/4.0/>`_.
This means you have permission to copy, distribute, translate, and adapt
the work as long as you attribute the |lumina| Project as the original
source of the Handbook.

Welcome to |sysadm|! This documentation is intended to educate the user
on initializing and configuring the |sysadm| remote management options.
Initialization and management will be documented in two separate
chapters, :ref:`gettingstarted`, and :ref:`management`.

.. warning:: |sysadm| is still under heavy development, and all
   information contained in the documentation is subject to change.

**What is SysAdm™?**

|sysadm| is a middleware utility designed to streamline system management
with options for both local and remote access.

.. note:: By default, |sysadm| does **not** allow for remote access.
   The user must configure the system to allow this feature.

One unique element to |sysadm| is how the middleware is designed to
modify the system directly. |sysadm| has no middleware database, which
means all changes made with |sysadm| modify the system configuration
files directly, resulting in a system administrator no longer needing to
log into a system via SSH or relearn system management. |sysadm| "speaks"
the same language, allowing for simple and effective system
administration.

For remote access, |sysadm| is being designed to route encrypted traffic
through a "bridge", a static announcement server which facilitates
communication between the user's controlling device and the remote
access system.

In order to address security concerns, the bridge device is always
considered "untrusted" and several layers of encryption are added to all
traffic flowing through the bridge to ensure it can not be used to
record or alter critical information flow.

**Would you like to know more?**

Documentation for the |sysadm| project is split amongst three handbooks:

* **API Reference Guide**: A library of all API calls and WebSocket
  requests for |sysadm|. This reference is constantly updated as new API
  calls are written. It can be found at https://api.sysadm.us/.

* **Client Handbook**: A detailed guide to all client side functions
  of |sysadm|.
  
* **Server Handbook**: A basic guide to initializing |sysadm| with
  a bridge and server connection.