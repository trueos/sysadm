.. _management:

SysAdm Management
=================

SysAdm comes with a standard configuration file located at :file:`/usr/local/etc/sysadm.conf.dist`.
  
It is possible to edit this file for a custom configuration, but the result will need to be saved as :kbd:`sysadm.conf`.
Here is the current default settings for SysAdm::

  #Sample Configuration file for the sysadm server

  ### Server Port Number ###
  # - Websocket Server (standard)
  PORT=12150
  # - REST Server (started with the "-rest" CLI flag)
  PORT_REST=12151
  
This default configuration also has blacklist options, recreated here::

  ### Blacklist options ###
  # - Number of minutes that an IP remains on the blacklist
  BLACKLIST_BLOCK_MINUTES=60
  # - Number of authorization failures before an IP is placed on the blacklist
  BLACKLIST_AUTH_FAIL_LIMIT=5
  # - Number of minutes of no activity from an IP before  resetting the failure counter
  #   (Note: A successful authorization will always reset the fail counter)
  BLACKLIST_AUTH_FAIL_RESET_MINUTES=10
  
Please note these default options are subject to change as the SysAdm utility is developed.