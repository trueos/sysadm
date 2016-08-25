.. _network:

network
*******

The network class is used to manage and retrieve information from
Ethernet and wireless network devices.

Every network class request contains the following parameters:

+---------------+-----------+-------------------------------------------+
| **Parameter** | **Value** | **Description**                           |
|               |           |                                           |
+===============+===========+===========================================+
| id            |           | Any unique value for the request,         |
|               |           | including a hash, checksum, or uuid.      |
+---------------+-----------+-------------------------------------------+
| name          | network   |                                           |
|               |           |                                           |
+---------------+-----------+-------------------------------------------+
| namespace     | sysadm    |                                           |
|               |           |                                           |
+---------------+-----------+-------------------------------------------+
| action        |           | Supported actions include "list-devices". |
|               |           |                                           |
+---------------+-----------+-------------------------------------------+

The rest of this section provides examples of the available *actions*
for each type of request, along with their responses.

.. index:: list-devices, network

.. _List Devices:

List Devices
============

The "list-devices" action lists information about currently recognized
network devices. For each network device, the response includes the
device's MAC address, description, IPv4 address, IPv6 address, whether
or not the device is active, whether or not the device is configured
using DHCP, whether or not the device is wireless, its subnet mask, and
its current status.

**REST Request**

::

 PUT /sysadm/network
 {
    "action" : "list-devices"
 }

**WebSocket Request**

.. code-block:: json

 {
    "namespace" : "sysadm",
    "args" : {
       "action" : "list-devices"
    },
    "id" : "fooid",
    "name" : "network"
 }

**Response**

.. code-block:: json

 {
   "args": {
     "lo0": {
       "MAC": "00:00:00:00:00:00",
       "description": "",
       "ipv4": "127.0.0.1",
       "ipv6": "::1",
       "is_active": "true",
       "is_dhcp": "false",
       "is_wireless": "false",
       "netmask": "255.0.0.0",
       "status": "no carrier"
     },
     "re0": {
       "MAC": "fc:aa:14:77:a0:8d",
       "description": "RealTek 8168/8111 B/C/CP/D/DP/E/F/G PCIe Gigabit Ethernet",
       "ipv4": "192.168.1.130",
       "ipv6": "fe80::feaa:14ff:fe77:a08d%re0",
       "is_active": "true",
       "is_dhcp": "true",
       "is_wireless": "false",
       "netmask": "255.255.255.0",
       "status": "active"
     }
   },
   "id": "fooid",
   "name": "response",
   "namespace": "sysadm"
 }