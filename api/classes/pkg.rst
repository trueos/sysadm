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
| action                          |               | supported actions include "pkg_info"                                                                                 |
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