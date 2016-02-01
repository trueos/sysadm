.. _iocage:

iocage
******

The iocage class is used to manage jails which provide a light-weight, operating system-level virtualization for running applications or services.

Every iocage class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | iocage        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "getdefaultsettings", "listjails", "getjailsettings"                                       |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: getdefaultsettings, iocage

.. _Default Settings:

Default Settings
================

The "getdefaultsettings" action lists all of the global settings that apply to all jails.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "getdefaultsettings"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "getdefaultsettings": {
            "defaults": {
                "allow_chflags": "0",
                "allow_mount": "0",
                "allow_mount_devfs": "0",
                "allow_mount_nullfs": "0",
                "allow_mount_procfs": "0",
                "allow_mount_tmpfs": "0",
                "allow_mount_zfs": "0",
                "allow_quotas": "0",
                "allow_raw_sockets": "0",
                "allow_set_hostname": "1",
                "allow_socket_af": "0",
                "allow_sysvipc": "0",
                "available": "426G",
                "boot": "off",
                "bpf": "off",
                "children_max": "0",
                "compression": "lz4",
                "compressratio": "1.00x",
                "coredumpsize": "off",
                "count": "1",
                "cpuset": "off",
                "cputime": "off",
                "datasize": "off",
                "dedup": "off",
                "defaultrouter": "none",
                "defaultrouter6": "none",
                "devfs_ruleset": "4",
                "dhcp": "off",
                "enforce_statfs": "2",
                "exec_clean": "1",
                "exec_fib": "0",
                "exec_jail_user": "root",
                "exec_poststart": "/usr/bin/true",
                "exec_poststop": "/usr/bin/true",
                "exec_prestart": "/usr/bin/true",
                "exec_prestop": "/usr/bin/true",
                "exec_start": "/bin/sh /etc/rc",
                "exec_stop": "/bin/sh /etc/rc.shutdown",
                "exec_system_jail_user": "0",
                "exec_system_user": "root",
                "exec_timeout": "60",
                "ftpfiles": "base.txz doc.txz lib32.txz src.txz",
                "ftphost": "ftp.freebsd.org",
                "gitlocation": "https://github.com",
                "hack88": "0",
                "host_domainname": "none",
                "host_hostname": "442a5843-c6bd-11e5-bbe9-fcaa14deb15d",
                "host_hostuuid": "442a5843-c6bd-11e5-bbe9-fcaa14deb15d",
                "hostid": "4145fbb8-c5b6-11e5-9f2f-fcaa14deb15d",
                "interfaces": "vnet0:bridge0,vnet1:bridge1",
                "ip4": "new",
                "ip4_addr": "none",
                "ip4_autoend": "none",
                "ip4_autostart": "none",
                "ip4_autosubnet": "none",
                "ip4_saddrsel": "1",
                "ip6": "new",
                "ip6_addr": "none",
                "ip6_saddrsel": "1",
                "istemplate": "no",
                "jail_zfs": "off",
                "jail_zfs_dataset": "iocage/jails/442a5843-c6bd-11e5-bbe9-fcaa14deb15d/data",
                "jail_zfs_mountpoint": "none",
                "last_started": "none",
                "login_flags": "-f root",
                "maxproc": "off",
                "memorylocked": "off",
                "memoryuse": "8G:log",
                "mount_devfs": "1",
                "mount_fdescfs": "1",
                "mount_linprocfs": "0",
                "mount_procfs": "0",
                "mountpoint": "/iocage/.defaults",
                "msgqqueued": "off",
                "msgqsize": "off",
                "nmsgq": "off",
                "notes": "none",
                "nsemop": "off",
                "nshm": "off",
                "nthr": "off",
                "openfiles": "off",
                "origin": "-",
                "owner": "root",
                "pcpu": "off",
                "pkglist": "none",
                "priority": "99",
                "pseudoterminals": "off",
                "quota": "none",
                "release": "11.0-CURRENTJAN2016",
                "reservation": "none",
                "resolver": "none",
                "rlimits": "off",
                "securelevel": "2",
                "shmsize": "off",
                "stacksize": "off",
                "stop_timeout": "30",
                "swapuse": "off",
                "sync_target": "none",
                "sync_tgt_zpool": "none",
                "tag": "2016-01-29@14:19:49",
                "type": "basejail",
                "used": "96K",
                "vmemoryuse": "off",
                "vnet": "off",
                "vnet0_mac": "none",
                "vnet1_mac": "none",
                "vnet2_mac": "none",
                "vnet3_mac": "none",
                "wallclock": "off"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "getdefaultsettings"
   },
   "id" : "fooid",
   "name" : "iocage",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "getdefaultsettings": {
      "defaults": {
        "allow_chflags": "0",
        "allow_mount": "0",
        "allow_mount_devfs": "0",
        "allow_mount_nullfs": "0",
        "allow_mount_procfs": "0",
        "allow_mount_tmpfs": "0",
        "allow_mount_zfs": "0",
        "allow_quotas": "0",
        "allow_raw_sockets": "0",
        "allow_set_hostname": "1",
        "allow_socket_af": "0",
        "allow_sysvipc": "0",
        "available": "426G",
        "boot": "off",
        "bpf": "off",
        "children_max": "0",
        "compression": "lz4",
        "compressratio": "1.00x",
        "coredumpsize": "off",
        "count": "1",
        "cpuset": "off",
        "cputime": "off",
        "datasize": "off",
        "dedup": "off",
        "defaultrouter": "none",
        "defaultrouter6": "none",
        "devfs_ruleset": "4",
        "dhcp": "off",
        "enforce_statfs": "2",
        "exec_clean": "1",
        "exec_fib": "0",
        "exec_jail_user": "root",
        "exec_poststart": "/usr/bin/true",
        "exec_poststop": "/usr/bin/true",
        "exec_prestart": "/usr/bin/true",
        "exec_prestop": "/usr/bin/true",
        "exec_start": "/bin/sh /etc/rc",
        "exec_stop": "/bin/sh /etc/rc.shutdown",
        "exec_system_jail_user": "0",
        "exec_system_user": "root",
        "exec_timeout": "60",
        "ftpfiles": "base.txz doc.txz lib32.txz src.txz",
        "ftphost": "ftp.freebsd.org",
        "gitlocation": "https://github.com",
        "hack88": "0",
        "host_domainname": "none",
        "host_hostname": "442a5843-c6bd-11e5-bbe9-fcaa14deb15d",
        "host_hostuuid": "442a5843-c6bd-11e5-bbe9-fcaa14deb15d",
        "hostid": "4145fbb8-c5b6-11e5-9f2f-fcaa14deb15d",
        "interfaces": "vnet0:bridge0,vnet1:bridge1",
        "ip4": "new",
        "ip4_addr": "none",
        "ip4_autoend": "none",
        "ip4_autostart": "none",
        "ip4_autosubnet": "none",
        "ip4_saddrsel": "1",
        "ip6": "new",
        "ip6_addr": "none",
        "ip6_saddrsel": "1",
        "istemplate": "no",
        "jail_zfs": "off",
        "jail_zfs_dataset": "iocage/jails/442a5843-c6bd-11e5-bbe9-fcaa14deb15d/data",
        "jail_zfs_mountpoint": "none",
        "last_started": "none",
        "login_flags": "-f root",
        "maxproc": "off",
        "memorylocked": "off",
        "memoryuse": "8G:log",
        "mount_devfs": "1",
        "mount_fdescfs": "1",
        "mount_linprocfs": "0",
        "mount_procfs": "0",
        "mountpoint": "/iocage/.defaults",
        "msgqqueued": "off",
        "msgqsize": "off",
        "nmsgq": "off",
        "notes": "none",
        "nsemop": "off",
        "nshm": "off",
        "nthr": "off",
        "openfiles": "off",
        "origin": "-",
        "owner": "root",
        "pcpu": "off",
        "pkglist": "none",
        "priority": "99",
        "pseudoterminals": "off",
        "quota": "none",
        "release": "11.0-CURRENTJAN2016",
        "reservation": "none",
        "resolver": "none",
        "rlimits": "off",
        "securelevel": "2",
        "shmsize": "off",
        "stacksize": "off",
        "stop_timeout": "30",
        "swapuse": "off",
        "sync_target": "none",
        "sync_tgt_zpool": "none",
        "tag": "2016-01-29@14:19:49",
        "type": "basejail",
        "used": "96K",
        "vmemoryuse": "off",
        "vnet": "off",
        "vnet0_mac": "none",
        "vnet1_mac": "none",
        "vnet2_mac": "none",
        "vnet3_mac": "none",
        "wallclock": "off"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: listjails, iocage

.. _List Jails:

List Jails
==========

The "listjails" action lists information about currently installed jails. For each jail, the response includes the UUID of the jail, whether or not the jail has been configured to start at
system boot, the jail ID (only applies to running jails), whether or not the jail is running, a friendly name for the jail (tag), and the type of jail (basejail or thickjail).

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "listjails"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "listjails": {
            "611c89ae-c43c-11e5-9602-54ee75595566": {
                "boot": "off",
                "jid": "-",
                "state": "down",
                "tag": "testjail",
                "type": "basejail"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "listjails"
   },
   "name" : "iocage",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "listjails": {
      "611c89ae-c43c-11e5-9602-54ee75595566": {
        "boot": "off",
        "jid": "-",
        "state": "down",
        "tag": "testjail",
        "type": "basejail"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: getjailsettings, iocage

.. _Jail Settings:

Jail Settings
=============

The "getjailsettings" action lists all of the settings that apply to the specified jail. This is equivalent to running :command:`iocage get all <jail>`.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "jail" : "testjail",
   "action" : "getjailsettings"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "getjailsettings": {
            "testjail": {
                "allow_chflags": "0",
                "allow_mount": "0",
                "allow_mount_devfs": "0",
                "allow_mount_nullfs": "0",
                "allow_mount_procfs": "0",
                "allow_mount_tmpfs": "0",
                "allow_mount_zfs": "0",
                "allow_quotas": "0",
                "allow_raw_sockets": "0",
                "allow_set_hostname": "1",
                "allow_socket_af": "0",
                "allow_sysvipc": "0",
                "boot": "off",
                "bpf": "off",
                "branch": "-",
                "children_max": "0",
                "coredumpsize": "off",
                "count": "1",
                "cpuset": "off",
                "cputime": "off",
                "datasize": "off",
                "defaultrouter": "none",
                "defaultrouter6": "none",
                "devfs_ruleset": "4",
                "dhcp": "off",
                "enforce_statfs": "2",
                "exec_clean": "1",
                "exec_fib": "0",
                "exec_jail_user": "root",
                "exec_poststart": "/usr/bin/true",
                "exec_poststop": "/usr/bin/true",
                "exec_prestart": "/usr/bin/true",
                "exec_prestop": "/usr/bin/true",
                "exec_start": "/bin/sh /etc/rc",
                "exec_stop": "/bin/sh /etc/rc.shutdown",
                "exec_system_jail_user": "0",
                "exec_system_user": "root",
                "exec_timeout": "60",
                "ftpdir": "-",
                "ftpfiles": "-",
                "ftphost": "-",
                "ftplocaldir": "-",
                "gitlocation": "https",
                "hack88": "0",
                "host_domainname": "none",
                "host_hostname": "4bb3f929-c6bf-11e5-bbe9-fcaa14deb15d",
                "host_hostuuid": "4bb3f929-c6bf-11e5-bbe9-fcaa14deb15d",
                "hostid": "4145fbb8-c5b6-11e5-9f2f-fcaa14deb15d",
                "interfaces": "vnet0",
                "ip4": "new",
                "ip4_addr": "none",
                "ip4_autoend": "none",
                "ip4_autostart": "none",
                "ip4_autosubnet": "none",
                "ip4_saddrsel": "1",
                "ip6": "new",
                "ip6_addr": "none",
                "ip6_saddrsel": "1",
                "istemplate": "no",
                "jail_zfs": "off",
                "jail_zfs_dataset": "iocage/jails/4ba5d76b-c6bf-11e5-bbe9-fcaa14deb15d/data",
                "jail_zfs_mountpoint": "none",
                "last_started": "none",
                "login_flags": "-f root",
                "maxproc": "off",
                "memorylocked": "off",
                "memoryuse": "8G",
                "mount_devfs": "1",
                "mount_fdescfs": "1",
                "mount_linprocfs": "0",
                "mount_procfs": "0",
                "msgqqueued": "off",
                "msgqsize": "off",
                "nmsgq": "off",
                "notes": "none",
                "nsemop": "off",
                "nshm": "off",
                "nthr": "off",
                "openfiles": "off",
                "owner": "root",
                "pcpu": "off",
                "pkglist": "none",
                "priority": "99",
                "pseudoterminals": "off",
                "release": "10.2-RELEASE",
                "resolver": "none",
                "rlimits": "off",
                "securelevel": "2",
                "shmsize": "off",
                "stacksize": "off",
                "start": "-",
                "stop_timeout": "30",
                "swapuse": "off",
                "sync_stat": "-",
                "sync_target": "none",
                "sync_tgt_zpool": "none",
                "tag": "testjail",
                "template": "-",
                "type": "basejail",
                "vmemoryuse": "off",
                "vnet": "off",
                "vnet0_mac": "none",
                "vnet1_mac": "none",
                "vnet2_mac": "none",
                "vnet3_mac": "none",
                "wallclock": "off"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "jail" : "testjail",
      "action" : "getjailsettings"
   },
   "id" : "fooid",
   "name" : "iocage",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "getjailsettings": {
      "testjail": {
        "allow_chflags": "0",
        "allow_mount": "0",
        "allow_mount_devfs": "0",
        "allow_mount_nullfs": "0",
        "allow_mount_procfs": "0",
        "allow_mount_tmpfs": "0",
        "allow_mount_zfs": "0",
        "allow_quotas": "0",
        "allow_raw_sockets": "0",
        "allow_set_hostname": "1",
        "allow_socket_af": "0",
        "allow_sysvipc": "0",
        "boot": "off",
        "bpf": "off",
        "branch": "-",
        "children_max": "0",
        "coredumpsize": "off",
        "count": "1",
        "cpuset": "off",
        "cputime": "off",
        "datasize": "off",
        "defaultrouter": "none",
        "defaultrouter6": "none",
        "devfs_ruleset": "4",
        "dhcp": "off",
        "enforce_statfs": "2",
        "exec_clean": "1",
        "exec_fib": "0",
        "exec_jail_user": "root",
        "exec_poststart": "/usr/bin/true",
        "exec_poststop": "/usr/bin/true",
        "exec_prestart": "/usr/bin/true",
        "exec_prestop": "/usr/bin/true",
        "exec_start": "/bin/sh /etc/rc",
        "exec_stop": "/bin/sh /etc/rc.shutdown",
        "exec_system_jail_user": "0",
        "exec_system_user": "root",
        "exec_timeout": "60",
        "ftpdir": "-",
        "ftpfiles": "-",
        "ftphost": "-",
        "ftplocaldir": "-",
        "gitlocation": "https",
        "hack88": "0",
        "host_domainname": "none",
        "host_hostname": "4bb3f929-c6bf-11e5-bbe9-fcaa14deb15d",
        "host_hostuuid": "4bb3f929-c6bf-11e5-bbe9-fcaa14deb15d",
        "hostid": "4145fbb8-c5b6-11e5-9f2f-fcaa14deb15d",
        "interfaces": "vnet0",
        "ip4": "new",
        "ip4_addr": "none",
        "ip4_autoend": "none",
        "ip4_autostart": "none",
        "ip4_autosubnet": "none",
        "ip4_saddrsel": "1",
        "ip6": "new",
        "ip6_addr": "none",
        "ip6_saddrsel": "1",
        "istemplate": "no",
        "jail_zfs": "off",
        "jail_zfs_dataset": "iocage/jails/4ba5d76b-c6bf-11e5-bbe9-fcaa14deb15d/data",
        "jail_zfs_mountpoint": "none",
        "last_started": "none",
        "login_flags": "-f root",
        "maxproc": "off",
        "memorylocked": "off",
        "memoryuse": "8G",
        "mount_devfs": "1",
        "mount_fdescfs": "1",
        "mount_linprocfs": "0",
        "mount_procfs": "0",
        "msgqqueued": "off",
        "msgqsize": "off",
        "nmsgq": "off",
        "notes": "none",
        "nsemop": "off",
        "nshm": "off",
        "nthr": "off",
        "openfiles": "off",
        "owner": "root",
        "pcpu": "off",
        "pkglist": "none",
        "priority": "99",
        "pseudoterminals": "off",
        "release": "10.2-RELEASE",
        "resolver": "none",
        "rlimits": "off",
        "securelevel": "2",
        "shmsize": "off",
        "stacksize": "off",
        "start": "-",
        "stop_timeout": "30",
        "swapuse": "off",
        "sync_stat": "-",
        "sync_target": "none",
        "sync_tgt_zpool": "none",
        "tag": "testjail",
        "template": "-",
        "type": "basejail",
        "vmemoryuse": "off",
        "vnet": "off",
        "vnet0_mac": "none",
        "vnet1_mac": "none",
        "vnet2_mac": "none",
        "vnet3_mac": "none",
        "wallclock": "off"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 