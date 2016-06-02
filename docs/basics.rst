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

.. _bridge init:

Bridge Initialization
---------------------

Configuring and connecting to a bridge can be a complicated process. Thankfully, there are several steps that are done the first time a server and bridge are configured with SysAdm but do not need to be repeated later. Once these steps are complete, it becomes a much simpler process to for a new user to add their client to the now configured server and bridge.

.. _serverbridge init:

Server and Bridge Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To initialize the server and bridge, begin with the server. Run :command:`sudo sysadm-binary bridge_export_key [optional absolute file path]`. This will export the public SSL key the server uses to authenticate with the bridge.

.. note:: For both server and client, giving SSL key files an easy to remember name and location will simplify the process of finding those files for import to the bridge.

Now, we must transition to the bridge to import this token. Login to the bridge as the administrator (or root), then type :command:`sysadm-bridge -import_ssl_file <filename> <filepath>` to import the server key file. If not already active, start the bridge now.

.. note:: The bridge can import SSL files whether it is active or not with no negative effects.

Moving back to the server, run :command:`sudo sysadm-binary bridge_add <nickname> <URL>` to point the server at the bridge. A bridge runs on **port 12149** by default, so the URL will likely need **:12149** added to the end (Example URL: 127.0.0.1:12149). If necessary, (re)start the server. The log (:file:`/var/log/sysadm-server-ws.log`) should display messages about connecting to the bridge.
The server and bridge should now be configured and communicating with each other. Now we can begin to add clients which will communicate with the server through the bridge.

.. _add client:

Adding a Client to the Server/Bridge Connection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: If you have an old SSL bundle from a pre-alpha version of SysAdm created before June 2016, it will need to be removed prior to proceeding with the client initialization process.

In the client UI, create or manage the SSL bundle as prompted by the UI. Once the new SSL keys are created, open :menuselection:`Setup SSL --> View Certificates` in the connection manager and click "Save to File" for both the server and bridge keys. This will export both SSL keys in file form, depositing them in either the "Desktop" folder or home directory (depending on operating system). If necessary, send these key files as an email attachment to your system administrator as part of a request for server/bridge access.

Moving to the bridge, as the administrator (or root), run :command:`sysadm-bridge -import_ssl_file <nickname> <filepath>` for the requesting client's bridge key file. Now the client and bridge should be able to communicate, but the client/server connection still needs to be established. 

On the server, run :command:`sudo sysadm-binary -import_ssl_key <username> <filepath> [<email>]` to import the client -> server SSL key file. This grants an individual with that specific SSL authorization the same permissions as <user>.

Back in the user client, open the connection manager and choose "Bridge Relay" as the connection option. Input the established bridge's URL and click "Connect".The bridge will now show up in the menu tree with a different icon, and will have a sub-menu of connections within it. If you click on the bridged system, it will open the standard UI but the connection is still being relayed through the bridge.

.. _adddoc:

Additional Documentation
------------------------

API documentation can be found at https://api.pcbsd.org .

