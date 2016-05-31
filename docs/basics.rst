.. _gettingstarted:

Getting Started
===============

Beginning with SysAdm™ is a relatively simple process. 
SysAdm™ files are currently available from the `github repository <https://github.com/pcbsd/sysadm.git>`_


.. _building:

Building SysAdm™
----------------

The following Qt Modules are required before attempting to build SysAdm™: ::

  Qt5 Core (# pkg install qt5-core)
  Qt5 Concurrent (# pkg install qt5-concurrent)
  Qt5 Network (# pkg install qt-network)
  Qt5 Sql (# pkg install qt5-sql)
  Qt5 Websockets (# pkg install qt5-websockets)

Building the prototype version of SysAdm™ assumes you have access to github.com. ::

  % git clone https://github.com/pcbsd/sysadm.git
  % cd sysadm/src
  % /usr/local/lib/qt5/bin/qmake -recursive
  % make && sudo make install

.. _starting:

Starting SysAdm™
----------------

SysAdm™ has two functioning states, WebSockets (preferred) and REST.
Websockets allow the client to stay connected over a long period of time, allowing the usage of notifications and events.
The REST server mode is more limited as it only allows the user to send and receive single messages over a short term connection.
Please note that it is possible to enable SysAdm™ to function in both states simultaneously. ::
  
  (Websocket - recommended)
  % sudo sysrc -f /etc/rc.conf sysadm_enable="YES"
  % sudo service sysadm start

  (Optional for REST)
  % sudo sysrc -f /etc/rc.conf sysadm_rest_enable="YES"
  % sudo service sysadm-rest start

.. _bridge sysinit:

Bridge System Initialization
----------------------------

Initializing the bridge system will be described through two different methods: ground-up initialization and client/system administration. Ground-up will be useful for users who have full control over their network and are building and initializing SysAdm™ from scratch. The client/system administration method assumes a user is attempting to connect to an existing bridge which a system administrator controls.

**Ground-Up Initialization**

.. note:: This will require the latest server/bridge/client from source (on various systems as necessary)

1. (Client) If you have a pre-existing SSL certificate bundle for the client, it will need to be removed (:command:`~/.config/PCBSD/sysadm_ssl*`). Then create a new SSL certificate bundle within the client after a restart, as the client will now create several different certificates.
2. (Client) Once the new SSL certs are created, open up the "Setup SSL -> View Certificates" page/tab in the connection manager, and click the "Save to file" button on both certs. This will export your public SSL key in file form, and place the files either in your Desktop folder or in your home directory (depending on the system/OS).
3. (Server) Run :command:`sudo sysadm-binary bridge_export_key [optional absolute file path]` to export the public SSL key that the server will use to authenticate with the bridge (it helps to dump it to a file like "/tmp/sysadm-server-to-bridge.key").
4. (Bridge) Run (as the user which will be running the bridge) :command:`sysadm-bridge -import_ssl_file <nickname> <filepath>` on the server -> bridge and client -> bridge key files that were exported earlier. If needed, these files can easily be transferred between systems.
5. (Bridge) If the bridge is not already running, start it now. Step 4 can be performed while the bridge is active without any adverse effects.
6. (Server) Run :command:`sudo sysadm-binary -import_ssl_key <username> <filepath> [<email>]` to import the client -> server SSL key file. This grants an individual with that specific SSL authorization the same permissions as <user>.
7. (Server) Run :command:`sudo sysadm-binary bridge_add <nickname> <URL>` to point the server at a bridge. The bridge runs on **port 12149** by default, so you will probably need to add a ":12149" to the end (example: 127.0.0.1:12149).
8. (Server) If necessary, start/restart the server. If you view the log (/var/log/sysadm-server-ws.log) you should see some messages about connecting to the bridge.
9. (Client) Open up the connection Manager and create a connection to the bridge with the same URL as the one you just used for the server (Ex: 127.0.0.1:12149). The username/password are not needed, but the GUI will still ask for them anyway so you can just give it dummy info right now so you can test/connect.

The bridge will show up in the menu tree with a different icon, and will have a sub-menu of connections within it. If you click on the bridged system, it will open the standard UI but connected to that system through the bridge.

**Client/System Administration**

This method reflects a different scenario in which a user requests bridge access from a system administrator, who then adds the user's key to an existing bridge:
Two files are output client to bridge and client to server

**User:**

1. In the SysAdm client, create a SSL certificate bundle. Skip this step if the bundle has already been created.
2. Open the "Setup SSL -> View Certificates" page/tab in the connection manager, and click the "Save to file" button on both certificates. This will export your public SSL keys in file form, and place the files either in your Desktop folder or in your home directory (depending on the system/OS).
3. Contact the bridge's system administrator, requesting access to the bridge and providing the public SSL keys saved in step 1.

**System Administrator:**

1. Once a user request for bridge access is received along with the user generated public SSL keys, the administrator needs to add the keys to the server and bridge:

  a. Export the user submitted keys to their respective locations: client --> bridge and server --> bridge.
  b. On the bridge, run :command:`sysadm-bridge -import_ssl_file <nickname> <filepath>` on the server -> bridge and client -> bridge key files that were exported earlier. If needed, these files can easily be transferred between systems.
  c. Back on the server, run :command:`sudo sysadm-binary -import_ssl_key <username> <filepath> [<email>]` to import the client -> server SSL key file. This grants an individual with that specific SSL authorization the same permissions as <user>.
  
2. Provide the bridge URL to the user.

**User:**

1. In the SysAdm client, open up the connection Manager and create a connection to the bridge with the same URL as the one provided by the system administrator (Ex: 127.0.0.1:12149). 
2. Provide a username/password if necessary.

.. _adddoc:

Additional Documentation
------------------------

API documentation can be found at https://api.pcbsd.org .

