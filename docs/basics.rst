.. _gettingstarted:

Getting Started
===============

Beginning with SysAdm is a relatively simple process. 
SysAdm files are currently available from the `github repository <https://github.com/pcbsd/sysadm.git>`_


.. _building:

Building SysAdm
---------------

The following Qt Modules are required before attempting to build SysAdm: ::

  Qt5 Core (# pkg install qt5-core)
  Qt5 Concurrent (# pkg install qt5-concurrent)
  Qt5 Network (# pkg install qt-network)
  Qt5 Sql (# pkg install qt5-sql)
  Qt5 Websockets (# pkg install qt5-websockets)

Building the prototype version of SysAdm assumes you have access to github.com. ::

  % git clone https://github.com/pcbsd/sysadm.git
  % cd sysadm/src
  % /usr/local/lib/qt5/bin/qmake -recursive
  % make && sudo make install

.. _starting:

Starting SysAdm
---------------

SysAdm has two functioning states, WebSockets (preferred) and REST.
Websockets allow the client to stay connected over a long period of time, allowing the usage of notifications and events.
The REST server mode is more limited as it only allows the user to send and receive single messages over a short term connection.
Please note that it is possible to enable SysAdm to function in both states simultaneously. ::
  
  (Websocket - recommended)
  % sudo sysrc -f /etc/rc.conf sysadm_enable="YES"
  % sudo service sysadm start

  (Optional for REST)
  % sudo sysrc -f /etc/rc.conf sysadm_rest_enable="YES"
  % sudo service sysadm-rest start


.. _adddoc:

Additional Documentation
------------------------

API documentation can be found at https://api.pcbsd.org .

