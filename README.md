# SysAdm

Official repo for PC-BSD's sysadm middleware WebSocket & REST server

This middleware acts as the core for controlling a PC-BSD or FreeBSD
system either locally or remotely via WebSockets or REST.

### Required Qt Modules

```
Qt5 Core (pkg install qt5-core)
Qt5 Concurrent (pkg install qt5-concurrent)
Qt5 Websockets (pkg install qt5-websockets)
```

### Building SysAdm

```
% git clone https://github.com/pcbsd/sysadm.git
% cd sysadm/src
% /usr/local/lib/qt5/bin/qmake -recursive
% make && sudo make install
```

### Starting SysAdm

```
(For WebSockets - Required for SysAdm Client)
% sudo sysrc -f /etc/rc.conf sysadm_websocket_enable="YES"
% sudo service sysadm-websocket start

(Optional for REST)
% sudo sysrc -f /etc/rc.conf sysadm_restserver_enable="YES"
% sudo service sysadm-restserver start
```

### API Documentation

https://api.pcbsd.org

### Contributing new API calls

Adding new API calls to the middleware is very straight-forward, simply
add a new function which accepts JSON in, and returns JSON, then connect
it to the backend.

Example:
https://github.com/pcbsd/sysadm/commit/4d3b590f460d301b9376a063f9407dabfd7c9c66

### Adding new Classes for API calls

Adding a new API class requires tweaking a few more files than a new API call only.

Example:
https://github.com/pcbsd/sysadm/commit/1ba65b33880e2298ade3e5cc4f2718aa6112e44f

### Testing new API calls / classes

Before committing or sending a pull request, you'll need to run our 
"api-test.sh" script and confirm it works properly. To do so, first add
your new call and restart the websocket server. Next do the following:

```
% cd sysadm/tests
% ./api-test.sh
```

The api-test script will prompt you to enter username, password, and some
information about which class / API call to run. When that is done, and you
have verified the functionality of your new call you should add the output
of the test script (either from copy-n-paste, or from the file /tmp/api-response)
to your commit. (This will allow us to document the new call / class)

