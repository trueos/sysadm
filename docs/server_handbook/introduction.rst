
.. _intro:

Introduction
============

**Preface** 

Written by users of the SysAdm™ management utility.

Version |version|

Copyright © 2016 iXSystems®.

Welcome to SysAdm™! This documentation is intended to educate the user 
on initializing and managing the SysAdm™ middleware. Initialization and 
management will be documented in two separate chapters, 
:ref:`gettingstarted`, and :ref:`management`. API documentation is being
handled at https://api.pcbsd.org.

**What is SysAdm™?**

SysAdm™ is a middleware utility designed to provide a user access to
firewalled servers and systems from any location with an open access
point to the internet. To accomplish this goal, a bridge device has been
created, which relays all traffic between the firewalled system and the
user's controlling device. In order to address security concerns, the
bridge device is always considered "untrusted" and several layers of
encryption are added to all traffic flowing through the bridge to ensure
the bridge can not be used to record or alter critical information flow.
Once a secure connection has been established, a user can fully control
a firewalled system or group of systems through the SysAdm™ utility.
Installing packages, creating work tasks, and managing build servers and
automation will all be possible once the utility is fully developed.
Controlling a secure system from any Internet connected device with
minimal risk of a security failure is the ultimate goal of SysAdm™.