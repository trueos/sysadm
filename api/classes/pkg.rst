.. _pkg:

pkg
***

The pkg class is used to manage software packages.

Every pkg class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | pkg           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | sysadm        |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "pkg_info", "pkg_search", "list_categories", "list_repos", "pkg_audit", "pkg_upgrade",     |
|                                 |               | "pkg_check_upgrade", "pkg_update", "pkg_lock", "pkg_unlock                                                           |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: pkg_info, pkg

.. _Package Information:

Package Information
===================

The "pkg_info" action reads the pkg database directly and returns any relevant information. The "repo" is optional as it defaults to "local". The "pkg_origins" is also optional; if it is
not specified, information for all installed packages will be listed.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "pkg_origins" : [
      "x11/lumina"
   ],
   "repo" : "local",
   "action" : "pkg_info"
 }

**WebSocket Request**

.. code-block:: json

 {
   "name" : "pkg",
   "namespace" : "sysadm",
   "args" : {
      "repo" : "local",
      "action" : "pkg_info",
      "pkg_origins" : [
         "x11/lumina"
      ]
   },
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_info": {
      "x11/lumina": {
        "arch": "FreeBSD:11:amd64",
        "automatic": "0",
        "comment": "Lumina Desktop Environment",
        "dep_formula": "",
        "dependencies": [
          "x11-toolkits/qt5-gui",
          "x11/qt5-x11extras",
          "x11-wm/fluxbox",
          "x11/libXdamage",
          "devel/qt5-linguist",
          "x11/numlockx",
          "devel/qt5-buildtools",
          "multimedia/qt5-multimedia",
          "graphics/qt5-svg",
          "x11/xbrightness",
          "x11/xorg",
          "devel/desktop-file-utils",
          "devel/qt5-concurrent",
          "x11/libX11",
          "net/qt5-network",
          "x11-themes/fluxbox-tenr-styles-pack",
          "x11-themes/kde4-icons-oxygen",
          "devel/qt5-core",
          "x11/xscreensaver",
          "multimedia/gstreamer1-plugins-core",
          "graphics/qt5-imageformats"
        ],
        "desc": "Lumina-DE is a lightweight, BSD licensed desktop environment,\ndesigned specifically for use on FreeBSD\n\nWWW: http://lumina-desktop.org",
        "files": [
          "/usr/local/share/licenses/lumina-0.8.8_2,1/catalog.mk",
          "/usr/local/share/licenses/lumina-0.8.8_2,1/LICENSE",
          "/usr/local/share/licenses/lumina-0.8.8_2,1/BSD3CLAUSE",
          "/usr/local/bin/Lumina-DE",
          "/usr/local/bin/lumina-config",
          "/usr/local/bin/lumina-fileinfo",
          "/usr/local/bin/lumina-fm",
          "/usr/local/bin/lumina-info",
          "/usr/local/bin/lumina-open",
          "/usr/local/bin/lumina-screenshot",
          "/usr/local/bin/lumina-search",
          "/usr/local/bin/lumina-xconfig",
          "/usr/local/etc/luminaDesktop.conf.dist",
          "/usr/local/include/LuminaOS.h",
          "/usr/local/include/LuminaSingleApplication.h",
          "/usr/local/include/LuminaThemes.h",
          "/usr/local/include/LuminaUtils.h",
          "/usr/local/include/LuminaX11.h",
          "/usr/local/include/LuminaXDG.h",
          "/usr/local/lib/libLuminaUtils.so",
          "/usr/local/lib/libLuminaUtils.so.1",
          "/usr/local/lib/libLuminaUtils.so.1.0",
          "/usr/local/lib/libLuminaUtils.so.1.0.0",
          "/usr/local/share/Lumina-DE/Login.ogg",
          "/usr/local/share/Lumina-DE/Logout.ogg",
          "/usr/local/share/Lumina-DE/colors/Black.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Blue-Light.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Grey-Dark.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Lumina-Glass.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Lumina-Gold.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Lumina-Green.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Lumina-Purple.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Lumina-Red.qss.colors",
          "/usr/local/share/Lumina-DE/colors/PCBSD10-Default.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Solarized-Dark.qss.colors",
          "/usr/local/share/Lumina-DE/colors/Solarized-Light.qss.colors",
          "/usr/local/share/Lumina-DE/desktop-background.jpg",
          "/usr/local/share/Lumina-DE/fluxbox-init-rc",
          "/usr/local/share/Lumina-DE/fluxbox-keys",
          "/usr/local/share/Lumina-DE/luminaDesktop.conf",
          "/usr/local/share/Lumina-DE/quickplugins/quick-sample.qml",
          "/usr/local/share/Lumina-DE/themes/Lumina-default.qss.template",
          "/usr/local/share/Lumina-DE/themes/None.qss.template",
          "/usr/local/share/applications/lumina-fm.desktop",
          "/usr/local/share/applications/lumina-info.desktop",
          "/usr/local/share/applications/lumina-screenshot.desktop",
          "/usr/local/share/applications/lumina-search.desktop",
          "/usr/local/share/applications/lumina-support.desktop",
          "/usr/local/share/pixmaps/Insight-FileManager.png",
          "/usr/local/share/pixmaps/Lumina-DE.png",
          "/usr/local/share/wallpapers/Lumina-DE/Lumina_Wispy_gold.jpg",
          "/usr/local/share/wallpapers/Lumina-DE/Lumina_Wispy_green.jpg",
          "/usr/local/share/wallpapers/Lumina-DE/Lumina_Wispy_purple.jpg",
          "/usr/local/share/wallpapers/Lumina-DE/Lumina_Wispy_red.jpg",
          "/usr/local/share/xsessions/Lumina-DE.desktop"
        ],
        "flatsize": "12324767",
        "icon": "\\\"http://www.pcbsd.org/appcafe/icons/x11_lumina.png\\\"",
        "id": "2541",
        "licenselogic": "1",
        "licenses": [
          "BSD3CLAUSE"
        ],
        "locked": "0",
        "maintainer": "kmoore@FreeBSD.org",
        "manifestdigest": "2$0$4ypg5zrco9upyuioczmo3uwbtdd5yart7xuit6fx3gjrn1k979qb",
        "message": "[{\"message\":\"The Lumina Desktop Environment has been installed!\\n\\nAn entry for for launching Lumina from a graphical login manager has already been added to the system, but if you with to start Lumina manually, you will need to do one of the following:\\n1) Put the line \\\"exec Lumina-DE\\\" at the end of your user's \\\"~/.xinitrc\\\" file before running startx\\n2) Wrap the Lumina binary call with an X initialization call: \\nExample: \\\"xinit ${PREFIX}/bin/Lumina-DE -- :0\\\"\\n\\nAlso note that the system-wide default settings for Lumina are contained in ${PREFIX}/etc/luminaDesktop.conf[.dist]. While it is possible to customize the desktop to the user's liking after logging in, you may want to adjust the default settings as necessary if there are multiple user accounts on this system.\"}]",
        "mtree_id": "",
        "name": "lumina",
        "options": {
          "MULTIMEDIA": "on",
          "PCBSD": "on"
        },
        "origin": "x11/lumina",
        "pkg_format_version": "",
        "prefix": "/usr/local",
        "repo_type": "binary",
        "repository": "pcbsd-major",
        "screen1": "\\\"http://www.pcbsd.org/appcafe/screenshots/x11/lumina/screen1.png\\\"",
        "shlibs_provided": [
          "libLuminaUtils.so.1"
        ],
        "shlibs_required": [
          "libxcb.so.1",
          "libxcb-composite.so.0",
          "libxcb-damage.so.0",
          "libXdamage.so.1",
          "libxcb-util.so.1",
          "libGL.so.1",
          "libQt5Core.so.5",
          "libxcb-image.so.0",
          "libxcb-icccm.so.4",
          "libxcb-ewmh.so.2",
          "libQt5Gui.so.5",
          "libQt5Network.so.5",
          "libQt5Widgets.so.5",
          "libQt5Concurrent.so.5",
          "libQt5Multimedia.so.5",
          "libQt5MultimediaWidgets.so.5",
          "libQt5Svg.so.5",
          "libQt5X11Extras.so.5"
        ],
        "time": "1458334158",
        "version": "0.8.8_2,1",
        "www": "http://lumina-desktop.org"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
 .. index:: pkg_search, pkg

.. _Search Packages:

Search Packages
===============

The "pkg_search" action searches the package database for pkgs which match the given "search_term" (required). These parameters are optional:

* **"repo"**: may be used to specifiy searching the specified repository. If not specified, the local package database is searched.

* **"category"**: may be used to restrict searches to the specified package category.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "repo" : "pcbsd-major",
   "category" : "www",
   "action" : "pkg_search",
   "search_term" : "fire"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "pkg",
   "args" : {
      "action" : "pkg_search",
      "search_term" : "fire",
      "category" : "www",
      "repo" : "pcbsd-major"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_search": {
      "www/firefox": {
        "arch": "FreeBSD:11:amd64",
        "cksum": "cc72c379afbd66d152cf06b7d2a14ada413f338071ecb9b084899c94d39f951e",
        "comment": "Web browser based on the browser portion of Mozilla",
        "cpe": "cpe:2.3:a:mozilla:firefox:45.0:::::freebsd11:x64:1",
        "dep_formula": "",
        "desc": "Mozilla Firefox is a free and open source web browser descended from the\nMozilla Application Suite. It is small, fast and easy to use, and offers\nmany advanced features:\n\n o Popup Blocking\n o Tabbed Browsing\n o Live Bookmarks (ie. RSS)\n o Extensions\n o Themes\n o FastFind\n o Improved Security\n\nWWW: http://www.mozilla.com/firefox",
        "flatsize": "96435169",
        "icon": "\\\\\\\"http://www.pcbsd.org/appcafe/icons/www_firefox.png\\\\\\\"",
        "id": "12147",
        "licenselogic": "1",
        "maintainer": "gecko@FreeBSD.org",
        "manifestdigest": "2$0$hcbb9x7urbs9nw1e44chw9bwxn339983b6q9mixxdn5ghdwuh9ny",
        "name": "firefox",
        "no_provide_shlib": "yes",
        "olddigest": "",
        "origin": "www/firefox",
        "osversion": "",
        "path": "All/firefox-45.0_1,1.txz",
        "pkg_format_version": "",
        "pkgsize": "39935776",
        "prefix": "/usr/local",
        "screen1": "\\\\\\\"http://www.pcbsd.org/appcafe/screenshots/www/firefox/screen1.png\\\\\\\"",
        "screen2": "\\\\\\\"http://www.pcbsd.org/appcafe/screenshots/www/firefox/screen2.png\\\\\\\"",
        "version": "45.0_1,1",
        "www": "http://www.mozilla.com/firefox"
      },
      "www/firefox-esr": {
        "arch": "FreeBSD:11:amd64",
        "cksum": "811545c4da089b52db54ddee04af2ea8c439eb12e708f478b09141cdcca7aec5",
        "comment": "Web browser based on the browser portion of Mozilla",
        "cpe": "cpe:2.3:a:mozilla:firefox_esr:38.7.0:::::freebsd11:x64",
        "dep_formula": "",
        "desc": "Mozilla Firefox is a free and open source web browser descended from the\nMozilla Application Suite. It is small, fast and easy to use, and offers\nmany advanced features:\n\n o Popup Blocking\n o Tabbed Browsing\n o Live Bookmarks (ie. RSS)\n o Extensions\n o Themes\n o FastFind\n o Improved Security\n\nWWW: http://www.mozilla.com/firefox",
        "flatsize": "86940998",
        "icon": "\\\\\\\"http://www.pcbsd.org/appcafe/icons/www_firefox-esr.png\\\\\\\"",
        "id": "656",
        "licenselogic": "1",
        "maintainer": "gecko@FreeBSD.org",
        "manifestdigest": "2$0$km1kyyxoae47gyhp9gx7wz7pcnsn6jnc8yxgpz63iyynaxi7ia8y",
        "name": "firefox-esr",
        "no_provide_shlib": "yes",
        "olddigest": "",
        "origin": "www/firefox-esr",
        "osversion": "",
        "path": "All/firefox-esr-38.7.0,1.txz",
        "pkg_format_version": "",
        "pkgsize": "36352676",
        "prefix": "/usr/local",
        "version": "38.7.0,1",
        "www": "http://www.mozilla.com/firefox"
      },
      "www/firefox-esr-i18n": {
        "arch": "FreeBSD:11:*",
        "cksum": "c389f2960fa77548435e0b905b3ef6ddb48957b76c2d8346de1f9f97dd7b23ca",
        "comment": "Localized interface for Firefox",
        "dep_formula": "",
        "desc": "Language packs for Firefox\n\nWWW: http://www.mozilla.org/projects/l10n/",
        "flatsize": "102671800",
        "id": "17350",
        "licenselogic": "1",
        "maintainer": "gecko@FreeBSD.org",
        "manifestdigest": "2$0$wzmx16rcynpdej5eckeg6c8w8z6r7oha86cmjfth4pnfu9iojdmb",
        "name": "firefox-esr-i18n",
        "olddigest": "",
        "origin": "www/firefox-esr-i18n",
        "osversion": "",
        "path": "All/firefox-esr-i18n-38.7.0.txz",
        "pkg_format_version": "",
        "pkgsize": "10449532",
        "prefix": "/usr/local",
        "version": "38.7.0",
        "www": "http://www.mozilla.org/projects/l10n/"
      },
      "www/firefox-i18n": {
        "arch": "FreeBSD:11:*",
        "cksum": "11ca74215bb2c9032a316692b02d4b675cc2102b0e6c9c9f79e85cb6a292e689",
        "comment": "Localized interface for Firefox",
        "dep_formula": "",
        "desc": "Language packs for Firefox\n\nWWW: http://www.mozilla.org/projects/l10n/",
        "flatsize": "107852121",
        "id": "11462",
        "licenselogic": "1",
        "maintainer": "gecko@FreeBSD.org",
        "manifestdigest": "2$0$hozjo4sqt3kn4rqak7hfr4zubt3yahigcnhmbwad7xtuqt1qxntb",
        "name": "firefox-i18n",
        "olddigest": "",
        "origin": "www/firefox-i18n",
        "osversion": "",
        "path": "All/firefox-i18n-45.0.txz",
        "pkg_format_version": "",
        "pkgsize": "10295024",
        "prefix": "/usr/local",
        "version": "45.0",
        "www": "http://www.mozilla.org/projects/l10n/"
      },
      "www/firefox-pulse": {
        "arch": "FreeBSD:11:amd64",
        "cksum": "76bcc4096c378a647c4517ab8fac64d3ecbf2c08a1e47ab0eb9061d95d86c195",
        "comment": "Web browser based on the browser portion of Mozilla",
        "cpe": "cpe:2.3:a:mozilla:firefox:45.0:::::freebsd11:x64:1",
        "dep_formula": "",
        "desc": "Mozilla Firefox is a free and open source web browser descended from the\nMozilla Application Suite. It is small, fast and easy to use, and offers\nmany advanced features:\n\n o Popup Blocking\n o Tabbed Browsing\n o Live Bookmarks (ie. RSS)\n o Extensions\n o Themes\n o FastFind\n o Improved Security\n\nWWW: http://www.mozilla.com/firefox",
        "flatsize": "96438909",
        "icon": "\\\\\\\"http://www.pcbsd.org/appcafe/icons/www_firefox-pulse.png\\\\\\\"",
        "id": "5534",
        "licenselogic": "1",
        "maintainer": "gecko@FreeBSD.org",
        "manifestdigest": "2$0$8mb8qqmcqu3ja8uy4x9nqgyeennjemumrb1q6ugyege76i4rdefb",
        "name": "firefox-pulse",
        "no_provide_shlib": "yes",
        "olddigest": "",
        "origin": "www/firefox-pulse",
        "osversion": "",
        "path": "All/firefox-pulse-45.0_1,1.txz",
        "pkg_format_version": "",
        "pkgsize": "39959876",
        "prefix": "/usr/local",
        "screen1": "\\\\\\\"http://www.pcbsd.org/appcafe/screenshots/www/firefox/screen1.png\\\\\\\"",
        "screen2": "\\\\\\\"http://www.pcbsd.org/appcafe/screenshots/www/firefox/screen2.png\\\\\\\"",
        "version": "45.0_1,1",
        "www": "http://www.mozilla.com/firefox"
      }
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

.. index:: list_categories, pkg

.. _List Categories:

List Categories
===============

The "list_categories" action lists all the known, non-empty categories within the specified repository or, if no repository is specified, the local repository.

**REST Request**

.. code-block:: json
 
 PUT /sysadm/pkg
 {
   "repo" : "local",
   "action" : "list_categories"
 }

**WebSocket Request**

.. code-block:: json
 
 {
   "id" : "fooid",
   "args" : {
      "action" : "list_categories",
      "repo" : "local"
   },
   "namespace" : "sysadm",
   "name" : "pkg"
 }

**Response**

.. code-block:: json
 
 {
  "args": {
    "list_categories": [
      "ports-mgmt",
      "x11",
      "gnome",
      "textproc",
      "devel",
      "python",
      "misc",
      "print",
      "graphics",
      "security",
      "x11-fonts",
      "lang",
      "ipv6",
      "perl5",
      "converters",
      "math",
      "x11-toolkits",
      "sysutils",
      "dns",
      "net",
      "accessibility",
      "databases",
      "shells",
      "x11-themes",
      "multimedia",
      "audio",
      "www",
      "ftp",
      "net-im",
      "archivers",
      "comms",
      "java",
      "deskutils",
      "kde",
      "mail",
      "editors",
      "emulators",
      "games",
      "irc",
      "japanese",
      "news",
      "x11-servers",
      "tk",
      "net-mgmt",
      "ruby",
      "x11-drivers",
      "x11-wm",
      "x11-clocks",
      "kld",
      "tcl",
      "enlightenment",
      "linux"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
.. index:: list_repos, pkg

.. _List Repositories:

List Repositories
=================

The "list_repositories" action scan the package repository configuration files and returns the names of the available repositories. All of the repositories returned by this
action are valid as the optional "repo" argument for the other pkg API actions.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "action" : "list_repos"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "namespace" : "sysadm",
   "name" : "pkg",
   "args" : {
      "action" : "list_repos"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "list_repos": [
      "local",
      "pcbsd-major"
    ]
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: pkg_audit, pkg

.. _Audit Packages:

Audit Packages
==============

The "pkg_audit" action performs an audit of all installed packages and reports any packages with known vulnerabilities as well as other packages which are impacted by those vulnerabilities. 

.. note:: the vulnerability information will be returned as a dispatcher event as this action just queues up the results of the :command:`pkg` operation. This is due to a limitation of
   :command:`pkg`, as it only supports one process call at a time. Refer to the :ref:`Dispatcher Subsystem` for instructions on how to subscribe to and query dispatcher events.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "action" : "pkg_audit"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "pkg_audit"
   },
   "name" : "pkg",
   "id" : "fooid",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_audit": {
      "proc_cmd": "pkg audit -qr",
      "proc_id": "sysadm_pkg_audit-{257cc46b-9178-4990-810a-12416ddfad79}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: pkg_upgrade, pkg

.. _Upgrade Packages:

Upgrade Packages
================

The "pkg_upgrade" action upgrades all currently installed packages. The messages from the upgrade will be returned as a dispatcher event. Refer to the :ref:`Dispatcher Subsystem` for
instructions on how to subscribe to and query dispatcher events.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "action" : "pkg_upgrade"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "pkg_upgrade"
   },
   "name" : "pkg",
   "namespace" : "sysadm",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_upgrade": {
      "proc_cmd": "pkg upgrade -y",
      "proc_id": "sysadm_pkg_upgrade-{19ace7c9-0d83-4a0d-9249-0b56cb105762}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: pkg_check_upgrade, pkg

.. _Check Packages:

Check Packages
==============

The "pkg_check_upgrade" action checks to see if there are any package updates available and returns that information as a dispatcher event. Refer to the :ref:`Dispatcher Subsystem` for
instructions on how to subscribe to and query dispatcher events.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "action" : "pkg_check_upgrade"
 }

**WebSocket Request**

.. code-block:: json

 {
   "args" : {
      "action" : "pkg_check_upgrade"
   },
   "namespace" : "sysadm",
   "name" : "pkg",
   "id" : "fooid"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_check_upgrade": {
      "proc_cmd": "pkg upgrade -n",
      "proc_id": "sysadm_pkg_check_upgrade-{c5e9d9a1-7c49-4a70-9d7c-4a84277c83b0}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: pkg_update, pkg

.. _Update Package Database:

Update Package Database
=======================

The "pkg_update" action instructs :command:`pkg` to update its databases. This action is typically not required.  It returns any information as a dispatcher event. Refer to the
:ref:`Dispatcher Subsystem` for instructions on how to subscribe to and query dispatcher events.

If you include "force" = "true", it forces :command:`pkg` to completely resync all of its databases with all known repositories which may take some time.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "force" : "true",
   "action" : "pkg_update"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "name" : "pkg",
   "namespace" : "sysadm",
   "args" : {
      "force" : "true",
      "action" : "pkg_update"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_update": {
      "proc_cmd": "pkg update -f",
      "proc_id": "sysadm_pkg_update-{8d65bbc5-fefc-4f34-8743-167e61a54c4c}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }
 
.. index:: pkg_lock, pkg_unlock, pkg

.. _Lock/Unlock Packages:

Lock/Unlock Packages
====================

The "pkg_lock" action locks the specified "pkg_origins" so that it will be skipped during a package upgrade and remain at its current version. When using "pkg_origins", specify either a 
single package origin string or an array of package origins.

The "pkg_unlock" action unlocks the previously locked "pkg_origins" so that it is no longer skipped during a package upgrade. 

Both actions return any information as a dispatcher event. Refer to the :ref:`Dispatcher Subsystem` for instructions on how to subscribe to and query dispatcher events.

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "pkg_origins" : [
      "misc/pcbsd-base"
   ],
   "action" : "pkg_lock"
 }

**WebSocket Request**

.. code-block:: json

 {
   "namespace" : "sysadm",
   "id" : "fooid",
   "name" : "pkg",
   "args" : {
      "pkg_origins" : [
         "misc/pcbsd-base"
      ],
      "action" : "pkg_lock"
   }
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_lock": {
      "proc_cmd": "pkg lock -y misc/pcbsd-base",
      "proc_id": "sysadm_pkg_lock-{352f7f66-d036-4c16-8978-67950957bf22}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }

**REST Request**

.. code-block:: json

 PUT /sysadm/pkg
 {
   "action" : "pkg_unlock",
   "pkg_origins" : "misc/pcbsd-base"
 }

**WebSocket Request**

.. code-block:: json

 {
   "id" : "fooid",
   "args" : {
      "action" : "pkg_unlock",
      "pkg_origins" : "misc/pcbsd-base"
   },
   "name" : "pkg",
   "namespace" : "sysadm"
 }

**Response**

.. code-block:: json

 {
  "args": {
    "pkg_unlock": {
      "proc_cmd": "pkg unlock -y misc/pcbsd-base",
      "proc_id": "sysadm_pkg_unlock-{d1771b41-c1ca-480a-a3ce-42d4eddbfae8}",
      "status": "pending"
    }
  },
  "id": "fooid",
  "name": "response",
  "namespace": "sysadm"
 }