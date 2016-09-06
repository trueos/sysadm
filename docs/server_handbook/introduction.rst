.. _intro:

Introduction
============

**Preface**

Written by users of the SysAdm™ management utility.

Version |version|

Copyright © 2016 iXSystems®.

Welcome to SysAdm™! This documentation is intended to educate the user
on initializing and configuring the SysAdm™ remote management options.
Initialization and management will be documented in two separate
chapters, :ref:`gettingstarted`, and :ref:`management`.

.. warning:: SysAdm™ is still under heavy development, and all
   information contained in the documentation is subject to change.

**What is SysAdm™?**

SysAdm™ is a middleware utility designed to streamline system management
with options for both local and remote access.

.. note:: By default, SysAdm™ does **not** allow for remote access.
   The user must configure the system to allow this feature.

One unique element to SysAdm™ is how the middleware is designed to
modify the system directly. SysAdm™ has no middleware database, which
means all changes made with SysAdm™ modify the system configuration
files directly, resulting in a system administrator no longer needing to
log into a system via SSH or relearn system management. SysAdm™ "speaks"
the same language, allowing for simple and effective system
administration.

For remote access, SysAdm™ is being designed to route encrypted traffic
through a "bridge", a static announcement server which facilitates
communication between the user's controlling device and the remote
access system.

In order to address security concerns, the bridge device is always
considered "untrusted" and several layers of encryption are added to all
traffic flowing through the bridge to ensure it can not be used to
record or alter critical information flow.

**Would you like to know more?**

Documentation for the SysAdm™ project is split amongst three handbooks:

* **API Reference Guide**: A library of all API calls and WebSocket
  requests for SysAdm™. This reference is constantly updated as new API
  calls are written. It can be found at https://api.sysadm.us/.

* **Client Handbook**: A detailed guide to all client side functions
  of SysAdm™.
  
* **Server Handbook**: A basic guide to initializing SysAdm™ with
  a bridge and server connection.