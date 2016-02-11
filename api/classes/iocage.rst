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
| action                          |               | supported actions include "getdefaultsettings", "listjails", "getjailsettings", "startjail", "stopjail",             |
|                                 |               | "capjail", "clonejail", "cleanjails", "cleanreleases", "cleantemplates", "cleanall", "activatepool", and             |
|                                 |               | "deactivatepool"                                                                                                     |
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
                "ip4": "-",
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
        "ip4": "-",
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
 
.. index:: startjail, iocage

.. _Start a Jail:

Start a Jail
============

The "startjail" action starts the specified jail.

.. note:: since a jail can only be started once, you will receive an error if the jail is already running.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "startjail",
   "jail" : "test"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "startjail": {
            "test": {
                "* Starting 0bf985de-ca0f-11e5-8d45-d05099728dbf (test)": "",
                "+ Started (shared IP mode) OK": "",
                "+ Starting services OK": ""
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "args" : {
      "action" : "startjail",
      "jail" : "test"
   },
   "name" : "iocage"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "startjail": {
      "test": {
        "INFO": " 0bf985de-ca0f-11e5-8d45-d05099728dbf (test) is already up"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: stopjail, iocage

.. _Stop a Jail:

Stop a Jail
===========

The "stopjail" action stops the specified jail.

.. note:: since a jail can only be stopped once, you will receive an error if the jail is not running.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "stopjail",
   "jail" : "test"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "stopjail": {
            "test": {
                "* Stopping 0bf985de-ca0f-11e5-8d45-d05099728dbf (test)": "",
                "+ Removing jail process OK": "",
                "+ Running post-stop OK": "",
                "+ Running pre-stop OK": "",
                "+ Stopping services OK": ""
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "jail" : "test",
      "action" : "stopjail"
   },
   "namespace" : "sysadm",
   "id" : "fooid",
   "name" : "iocage"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "stopjail": {
      "test": {
        "INFO": " 0bf985de-ca0f-11e5-8d45-d05099728dbf (test) is already down"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: capjail, iocage

.. _Cap a Jail:

Cap a Jail
===========

The "capjail" action re-applies resource limits to a running jail. Use this action when you make a change to the specified jail's resources and want to apply the changes without restarting
the jail.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "jail" : "test",
   "action" : "capjail"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "jail" : "test",
      "action" : "capjail"
   },
   "namespace" : "sysadm",
   "name" : "iocage",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "capjail": {
      "success": "jail test capped."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: clonejail, iocage

.. _Clone a Jail:

Clone a Jail
============

The "clonejail" action clones the specified "jail". By default, the clone will inherit that jail's properties. Use "props" to specify any properties that should differ. All available
properties are described in `iocage(8) <https://github.com/iocage/iocage/blob/master/iocage.8.txt>`_. 

In this example, the "tag" property is specified so that the new jail has a different name than the jail it was cloned from. 

**REST Request**

.. code-block:: json 

 PUT /sysadm/iocage
 {
   "props" : "tag=newtest",
   "jail" : "test",
   "action" : "clonejail"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "namespace" : "sysadm",
   "name" : "iocage",
   "args" : {
      "action" : "clonejail",
      "jail" : "test",
      "props" : "tag=newtest"
   },
   "id" : "fooid"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "clonejail": {
      "jail": "test",
      "props": "tag=newtest",
      "success": {
        "Successfully created": " 5e1fe97e-cfba-11e5-8209-d05099728dbf (newtest)"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
In this example, no properties are specified so iocage populates its own values and the props returned in the response is empty:

**REST Request**

.. code-block:: json 

 PUT /sysadm/iocage
 {
   "action" : "clonejail",
   "jail" : "test"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "args" : {
      "jail" : "test",
      "action" : "clonejail"
   },
   "name" : "iocage",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "clonejail": {
      "jail": "test",
      "props": "",
      "success": {
        "Successfully created": " 89e78032-cfba-11e5-8209-d05099728dbf (2016-02-09@23"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: cleanjails, iocage

.. _Clean Jails:

Clean Jails
===========

The "cleanjails" action destroys all existing jail datasets, including all data stored in the jails.

**REST Request**

.. code-block:: json 

 PUT /sysadm/iocage
 {
   "action" : "cleanjails"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "cleanjails"
   },
   "id" : "fooid",
   "name" : "iocage"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "cleanjails": {
      "success": "All jails have been cleaned."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: cleanreleases, iocage

.. _Clean Releases:

Clean Releases
==============

The "cleanreleases" action deletes all releases that have been fetched. Since basejails rely on releases, do not run this action if any basejails still exist.

**REST Request**

.. code-block:: json  

 PUT /sysadm/iocage
 {
   "action" : "cleanreleases"
 }

**WebSocket Request**

**REST Request**

.. code-block:: json  

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "args" : {
      "action" : "cleanreleases"
   },
   "name" : "iocage"
 }

**Response**

**REST Request**

.. code-block:: json  

 {
  "args": {
    "cleanreleases": {
      "success": "All RELEASEs have been cleaned."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: cleantemplates, iocage

.. _Clean Templates:

Clean Templates
===============

The "cleantemplates" action destroys all existing jail templates.

**REST Request**

.. code-block:: json  

 PUT /sysadm/iocage
 {
   "action" : "cleantemplates"
 }

**WebSocket Request**

.. code-block:: json  

 {
   "args" : {
      "action" : "cleantemplates"
   },
   "name" : "iocage",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json  

 {
  "args": {
    "cleantemplates": {
      "success": "All templates have been cleaned."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
 .. index:: cleanall, iocage

.. _Clean All:

Clean All
===========

The "cleanall" action destroys everything associated with iocage. 

**REST Request**

.. code-block:: json 

 PUT /sysadm/iocage
 {
   "action" : "cleanall"
 }

**WebSocket Request**

.. code-block:: json 

 {
   "namespace" : "sysadm",
   "args" : {
      "action" : "cleanall"
   },
   "id" : "fooid",
   "name" : "iocage"
 }

**Response**

.. code-block:: json 

 {
  "args": {
    "cleanall": {
      "success": "All iocage datasets have been cleaned."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: activatepool, iocage

.. _Activate a Pool:

Activate a Pool
===============

The "activatepool" action can be used to specify which ZFS pool is used to store jails. If you do not specify the pool, the response will indicate the current setting.

These examples specify the pool to use:

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "activatepool",
   "pool" : "tank"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "activatepool": {
            "success": "pool tank activated."
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "activatepool",
      "pool" : "tank"
   },
   "name" : "iocage",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "activatepool": {
      "success": "pool tank activated."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

These examples show responses when the pool is not specified:

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "activatepool"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "activatepool": {
            "currently active": {
                "pool": " tank"
            }
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "activatepool"
   },
   "namespace" : "sysadm",
   "name" : "iocage",
   "id" : "fooid"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "activatepool": {
      "currently active": {
        "pool": " tank"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
 .. index:: deactivatepool, iocage

.. _Deactivate a Pool:

Deactivate a Pool
=================

Since only one pool can be active, the "deactivatepool" action can be used to deactivate a currently active pool. This should be done before using the
"activatepool" action to activate a different pool. When a pool is deactivated, no data is removed. However, you won't have access to its jails unless you move those datasets to the newly
activated pool or activate the old pool again.

**REST Request**

.. code-block:: json

 PUT /sysadm/iocage
 {
   "action" : "deactivatepool",
   "pool" : "tank"
 }

**REST Response**

.. code-block:: json

 {
    "args": {
        "deactivatepool": {
            "success": "pool tank deactivated."
        }
    }
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "iocage",
   "args" : {
      "pool" : "tank",
      "action" : "deactivatepool"
   },
   "namespace" : "sysadm"
 }

**WebSocket Response**

.. code-block:: json

 {
  "args": {
    "deactivatepool": {
      "success": "pool tank deactivated."
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
