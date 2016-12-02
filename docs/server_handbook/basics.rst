.. _gettingstarted:

Getting Started
===============

Beginning with SysAdm™ is a relatively simple process.
SysAdm™ files are currently available from the
`github repository <https://github.com/trueos/sysadm>`_

.. _building:

Building |sysadm|
-----------------

Several Qt Modules are required before attempting to build
|sysadm|:

.. code-block:: none

  Qt5 Core (# pkg install qt5-core)
  Qt5 Concurrent (# pkg install qt5-concurrent)
  Qt5 Websockets (# pkg install qt5-websockets)

Building the prototype version of |sysadm| assumes you have access to
github.com.

.. code-block:: none

  % git clone https://github.com/trueos/sysadm.git
  % cd sysadm/src
  % /usr/local/lib/qt5/bin/qmake -recursive
  % make && sudo make install

.. _starting:

Starting |sysadm|
-----------------

|sysadm| can be started one of two ways: the traditional rc(8)
mechanism or using the new jobd(8) mechanism

To run under rc(8)

.. code-block:: none

 (For WebSockets - Required for SysAdm™ Client)
 % sudo sysrc -f /etc/rc.conf sysadm_enable="YES"
 % sudo service sysadm start

 (Optional for REST)
 % sudo sysrc -f /etc/rc.conf sysadm_rest_enable="YES"
 % sudo service sysadm-rest start

To run under jobd(8)

.. code-block:: none

 (For WebSockets - Required for SysAdm™ Client)
 % sudo jobctl org.pcbsd.sysadm enable

 (Optional for REST)
 % sudo jobctl org.pcbsd.sysadm-rest enable

.. danger:: Several ports on the system firewall will need to be opened
   for |sysadm| to have remote access functionality:
   
   * Port 12149 for WebSocket interaction.
   * Port 12150 for the REST interface.
   * Port 12151 for the |sysadm| bridge server.
   
   The user can also designate their own ports for |sysadm|.

.. _bridge init:

Bridge Initialization
---------------------

Configuring and connecting to a bridge can be a complicated process.
Thankfully, there are several steps that are done the first time a
server and bridge are configured with |sysadm| but do not need to be
repeated later. Once these steps are complete, it becomes a much simpler
process for a new user to configure their client to communicate with the
now configured server and bridge.

.. tip:: A list of current commands is available by typing :command:`-h`
   after the utility name (Example: :command:`sysadm-bridge -h`).

.. _serverbridge init:

Server and Bridge Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To initialize the server and bridge, begin with the server. Run
:command:`sudo sysadm-binary bridge_export_key [optional absolute file path]`.
This will export the public SSL key the server uses to authenticate with
the bridge.

.. note:: For both server and client, give SSL key files an easy to
   remember name and location to simplify the process of finding those
   files for import to the bridge.

Now, we must transition to the bridge to import the server key. Login to
the bridge as the administrator (or root), then type
:command:`sysadm-bridge import_ssl_file <filename> <filepath>`,
replacing <filename> and <filepath> with the server key filename and
location. Once the server key file is successfully imported, start the
bridge (if not already running).

.. tip:: The bridge can import SSL files regardless of its active state
   with no negative effects.

Back on the server, run :command:`sudo sysadm-binary bridge_add <nickname> <URL>`
to point the server at the bridge. A bridge runs on **port 12149** by
default, so the URL will likely need **:12149** added on the end of the
address (Example URL: 127.0.0.1:12149). If necessary, (re)start the
server. The log (:file:`/var/log/sysadm-server-ws.log`) will display
messages about connecting to the bridge. If properly configured, the
server and bridge will now be communicating with each other. At this
point, clients can be added to the mix which will communicate with the
server through the bridge.

.. _add client:

Adding a Client to the Server/Bridge Connection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. danger:: If you have an old SSL bundle from a pre-alpha version of
   |sysadm| created before June 2016, it will need to be removed prior to
   proceeding with the client initialization process.

In the client UI, create or import an SSL key bundle as prompted by the
UI. Once the new SSL keys are created, open
:menuselection:`Setup SSL --> View Certificates` in the connection 
manager and click :guilabel:`Export Public Key` for both the server and
bridge keys. This will export both SSL keys in file form, depositing
them in either the :file:`Desktop` folder or home directory (depending
on operating system). If necessary, send these key files as an email
attachment to the system administrator as part of a request for
server/bridge access.

Moving to the bridge, as the administrator (or root), run
:command:`sysadm-bridge import_ssl_file <nickname> <filepath>` for the
requesting client's bridge key file. Now the client and bridge should be
able to communicate, but the client/server connection still needs to be
established.

On the server, run :command:`sudo sysadm-binary import_ssl_key <username> <filepath> [<email>]`
to import the client -> server SSL key file. This grants an individual
with the specific SSL authorization the same permissions as <user>.

Back in the user client, open the connection manager and choose
:guilabel:`Bridge Relay` as the connection option. Input the established
bridge's URL and click :guilabel:`Connect`. The bridge will now show up
in the menu tree with a different icon, and will have a sub-menu of
connections within it. Click on the bridged system to will open the
standard UI, but note the connection is still being relayed through the
bridge.