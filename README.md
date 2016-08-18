<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [SysAdm](#sysadm)
    - [Required Qt Modules](#required-qt-modules)
    - [Building SysAdm](#building-sysadm)
    - [Starting SysAdm](#starting-sysadm)
    - [API Documentation](#api-documentation)
    - [Contributing new API calls](#contributing-new-api-calls)
    - [Adding new Classes for API calls](#adding-new-classes-for-api-calls)
    - [Testing new API calls / classes](#testing-new-api-calls--classes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# SysAdm

Official repo for PC-BSD's sysadm middleware WebSocket & REST server

This middleware acts as the core for controlling a PC-BSD or FreeBSD <br />
system either locally or remotely via WebSockets or REST. It is also the <br />
server component to [TrueOS' SysAdm GUI client](https://github.com/trueos/sysadm-ui-qt).

### Required Qt Modules

```
Qt5 Core (pkg install qt5-core)
Qt5 Concurrent (pkg install qt5-concurrent)
Qt5 Websockets (pkg install qt5-websockets)
```

### Building SysAdm

```
% git clone https://github.com/trueos/sysadm.git
% cd sysadm/src
% /usr/local/lib/qt5/bin/qmake -recursive
% make && sudo make install
```

### Starting SysAdm

SysAdm can be started one of two ways:
1. The traditional rc(8) mechanism
2. The new jobd(8) mechanism

To run under rc(8):
```
(For WebSockets - Required for SysAdm Client)
% sudo sysrc -f /etc/rc.conf sysadm_enable="YES"
% sudo service sysadm start

(Optional for REST)
% sudo sysrc -f /etc/rc.conf sysadm_rest_enable="YES"
% sudo service sysadm-rest start
```

To run under jobd(8):
```
(For WebSockets - Required for SysAdm Client)
% sudo jobctl org.pcbsd.sysadm enable

(Optional for REST)
% sudo jobctl org.pcbsd.sysadm-rest enable
```

### API Documentation

https://api.pcbsd.org

### Contributing new API calls

SysAdm is written using the Qt toolkit, which has excellent [reference documentation](http://doc.qt.io/qt-5/classes.html).

All Qt Core classes (I.E. non-gui) can be used in SysAdm server.

Adding new API calls to the middleware is very straight-forward, simply <br />
add a new function which accepts JSON in, and returns JSON, then connect <br />
it to the backend.

Example:
https://github.com/trueos/sysadm/commit/4d3b590f460d301b9376a063f9407dabfd7c9c66

### Adding new Classes for API calls

Adding a new API class requires tweaking a few more files than a new API call only.

Example:
https://github.com/trueos/sysadm/commit/1ba65b33880e2298ade3e5cc4f2718aa6112e44f

### Testing new API calls / classes

Before committing or sending a pull request, you'll need to run our  <br />
"api-test.sh" script and confirm it works properly. To do so, first add <br />
your new call and restart the websocket server. Next do the following:

```
% cd sysadm/tests
% ./api-test.sh
```

The api-test script will prompt you to enter username, password, and some <br />
information about which class / API call to run. When that is done, and you <br />
have verified the functionality of your new call you should add the output <br />
of the test script (either from copy-n-paste, or from the file /tmp/api-response) <br />
to your commit. (This will allow us to document the new call / class)

