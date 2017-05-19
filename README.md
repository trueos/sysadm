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

**General Project Information: Table of Contents**

- [General TrueOS Information](#gentrosinfo)
	- [TrueOS Project Documentation](#docs)
		- [TrueOS Handbook](#trueosdoc)
		- [Lumina Handbook](#luminadoc)
		- [SysAdm Handbooks](#sysadmdoc)
	- [Filing Issues or Feature Requests](#fileissues)
	- [Community Channels](#community)
		- [Discourse](#discourse)
		- [Gitter](#gitter)
		- [IRC](#irc)
		- [Subreddit](#subreddit)
	- [Social Media](#socialmedia)

<!-- END GENERAL INFO TOC -->

# SysAdm

Official repo for TrueOS' sysadm middleware WebSocket & REST server

This middleware acts as the core for controlling a TrueOS or FreeBSD <br />
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

# General TrueOS Information <a name="gentrosinfo"></a>

This section describes where you can find more information about TrueOS and its related projects, file new issues on GitHub, and converse with other users or contributors to the project.

## TrueOS Project Documentation <a name="docs"></a>

A number of [Sphinx](http://www.sphinx-doc.org/en/stable/) generated reStructuredText handbooks are available to introduce you to the TrueOS, Lumina, and SysAdm projects. These handbooks are open source, and users are always encouraged to open GitHub issues or fix any errors they find in the documentation.

### TrueOS Handbook <a name="trueosdoc"></a>

The [TrueOS User Guide](https://www.trueos.org/handbook/trueos.html) is a comprehensive guide to install TrueOS, along with post-installation configuration help, recommendations for useful utilities and applications, and a help and support section containing solutions for common issues and links to community and development chat channels for uncommon issues. There is also a chapter describing the experimental TrueOS Pico project and links to the Lumina and SysAdm documentation. All TrueOS documentation is hosted on the [TrueOS website](https://www.trueos.org).

### Lumina Handbook <a name="luminadoc"></a>

The Lumina Desktop Environment has its own [handbook](https://lumina-desktop.org/handbook/), hosted on the [Lumina Website](https://lumina-desktop.org). This handbook contains brief installation instructions. However, due to the highly customizable nature of Lumina, the focus of the handbook lies mainly in documenting all user configurable settings. Each option is typically described in detail, with both text and screenshots. Finally, the suite of unique Qt5 utilities included with Lumina are also documented.

TrueOS users are encouraged to review the Lumina documentation, as the Lumina Desktop Environment is installed by default with TrueOS.

### SysAdm Handbooks <a name="sysadmdoc"></a>

Due to complexity of this project, SysAdm documentation is split into three different guides:

1. **API Reference Guide** (https://api.sysadm.us/getstarted.html)

The Application Programming Interface (API) Reference Guide is a comprehensive library of all API calls and WebSocket requests for SysAdm. In addition to documenting all SysAdm subsystems and classes, the guide provides detailed examples of requests and responses, authentication, and SSL certificate management. This guide is constantly updated, ensuring it provides accurate information at all times.

2. **Client Handbook** (https://sysadm.us/handbook/client/)

The SysAdm Client handbook documents all aspects of the SysAdm client, as well as describing of the PC-BSD system utilities is replaces. Detailed descriptions of utilities such as Appcafe, Life Preserver, and the Boot Environment Manager are contained here, as well as a general guide to using these utilities. TrueOS users are encouraged to reference this guide, as the SysAdm client is included with TrueOS.

3. **Server Handbook** (https://sysadm.us/handbook/server/introduction.html)

The Server handbook is a basic installation guide, walking new users through the process of initializing SysAdm with a bridge and server connection.

## Filing Issues or Feature Requests <a name="fileissues"></a>

Due to the number of repositories under the TrueOS "umbrella", the TrueOS Project consolidates its issue trackers into a few repositories:

* [trueos-core](https://github.com/trueos/trueos-core) : Used for general TrueOS issues, Pico issues, and feature  requests.
* [lumina](https://github.com/trueos/lumina) : Issues related to using the Lumina Desktop Environment.
* (Coming Soon) [sysadm](https://github.com/trueos/sysadm) : Issues with using the SysAdm client or server.
* [trueos-docs](https://github.com/trueos/trueos-docs) : Issues related to the TrueOS Handbook.
* [lumina-docs](https://github.com/trueos/lumina-docs) : Issues related to the Lumina Handbook.
* [sysadm-docs](https://github.com/trueos/sysadm-docs) : Issues related to the SysAdm API Guide, Client, and Server Handbooks.
* [trueos-website](https://github.com/trueos/trueos-website) : Issues involving any of the TrueOS Project websites: 
  - https://www.lumina-desktop.org
  - https://www.trueos.org
  - https://www.sysadm.us

The TrueOS handbook has detailed instructions to help you report a bug (https://www.trueos.org/handbook/helpsupport.html#report-a-bug). It is recommended to refer to these instructions when creating new GitHub issues. Better bug reports usually result in faster fixes!

To request a feature, open a new issue in one of the related GitHub issue repositories and begin the title with *Feature Request:*.

## Community Channels <a name="community"></a>

The TrueOS community has a wide variety of chat channels and forum options available for users to interact with not only each other, but contributors to the project and the core development team too.

### Discourse <a name="discourse"></a>

TrueOS  has a [Discourse channel](https://discourse.trueos.org/) managed concurrently with the TrueOS Subreddit. New users need to sign up with Discourse in order to create posts, but it is possible to view posts without an account.

### Gitter <a name="gitter"></a>

The TrueOS Project uses Gitter to provide real-time chat and collaboration with TrueOS users and developers. Gitter does not require an application to use, but does require a login using either an existing GitHub or Twitter account.

To access the TrueOS Gitter community, point a web browser to https://gitter.im/trueos.

Gitter also maintains a full archive of the chat history. This means lengthy conversations about hardware issues or workarounds are always available for reference. To access the Gitter archive, navigate to the desired TrueOS room’s archive. For example, here is the address of the TrueOS Lobby archive: https://gitter.im/trueos/Lobby/archives.

### IRC <a name="irc"></a>

Like many open source projects, TrueOS has an Internet Relay Chat (IRC) channel so users can chat and get help in real time. To get connected, use this information in your IRC client:

* Server name: irc.freenode.net
* Channel name: #trueos (note the # is required)

### Subreddit <a name="subreddit"></a>

The TrueOS Project also has a [Subreddit](https://www.reddit.com/r/TrueOS/) for users who prefer to use Reddit to ask questions and to search for or post how-tos. A Reddit account is not required in order to read the Subreddit, but it is necessary to create a login account to submit or comment on posts.

## Social Media <a name="socialmedia"></a>

The TrueOS Project also maintains a number of social media accounts you can watch:

* Facebook: https://www.facebook.com/groups/4210443834/
* Linkedin: http://www.linkedin.com/groups?gid=1942544
* TrueOS Blog: https://www.trueos.org/blog/
* Twitter: https://twitter.com/TrueOS_Project/


