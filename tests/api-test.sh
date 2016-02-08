#!/usr/local/bin/bash
# (Yes, yes, this is a bash script)
# Both resty/jsawk use bash'isms

# Possible environment variables
# APITESTUSER - username
# APITESTPASS - password
# APITESTNAMESPACE - namespace / sysadm
# APITESTCLASS - API class
# APITESTPAYLOAD - JSON payload

# Default values
DEFUSER="root"
DEFNAMESPACE="sysadm"
DEFCLASS="lifepreserver"

# Set variable to call jsawk utility
JSAWK="./utils/jsawk -j js24"

if [ `id -u` != "0" ] ; then
  pkgPre="sudo"
else
  pkgPre=""
fi

which npm >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  $pkgPre pkg install -y npm
  if [ $? -ne 0 ] ; then
    echo "Requires npm!"
    exit 1
  fi
fi

pkg info p5-JSON >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  $pkgPre pkg install -y p5-JSON
  if [ $? -ne 0 ] ; then
    echo "Requires p5-JSON!"
    exit 1
  fi
fi

if [ ! -d "${HOME}/.npm/ws" ] ; then
  npm install ws
  if [ $? -ne 0 ] ; then
    echo "Failed installing ws node module"
    exit 1
  fi
fi

if [ ! -d "${HOME}/.npm/wss" ] ; then
  npm install wss
  if [ $? -ne 0 ] ; then
    echo "Failed installing wss node module"
    exit 1
  fi
fi

if [ -z "$APITESTUSER" ] ; then
  echo "Enter your username:"
  echo -e "($DEFUSER)>\c"
  read APITESTUSER
  if [ -z "$APITESTUSER" ] ; then
    APITESTUSER="$DEFUSER"
  fi
  echo ""
fi

if [ -z "$APITESTPASS" ] ; then
  echo "Enter your password:"
  echo -e ">\c"
  read -s APITESTPASS
  echo ""
fi

if [ -z "$APITESTNAMESPACE" ] ; then
  echo "Enter the namespace:"
  echo -e "($DEFNAMESPACE)>\c"
  read APITESTNAMESPACE
  if [ -z "$APITESTNAMESPACE" ] ; then
    APITESTNAMESPACE="$DEFNAMESPACE"
  fi
  echo ""
fi

if [ -z "$APITESTCLASS" ] ; then
  echo "Enter the class name:"
  echo -e "($DEFCLASS)>\c"
  read APITESTCLASS
  if [ -z "$APITESTCLASS" ] ; then
    APITESTCLASS="$DEFCLASS"
  fi
  echo ""
fi

if [ -z "$APITESTPAYLOAD" ] ; then
  echo "Enter the payload json:"
  echo -e "{ \"action\":\"listcron\" }>\c"
  read APITESTPAYLOAD
  if [ -z "$APITESTPAYLOAD" ] ; then
    APITESTPAYLOAD="{ \"action\":\"listcron\" }"
  fi
  echo ""
fi

# Source our resty functions
#. ./utils/resty -W "https://127.0.0.1:12151" -H "Accept: application/json" -H "Content-Type: application/json" -u ${fuser}:${fpass}

# Save output to a file in addition to stdout
#ofile="/tmp/api-response"
#echo "" > /tmp/api-response

# Check the reply of this REST query
#echo "" | tee -a $ofile
#echo "REST Request:" | tee -a $ofile
#echo "-------------------------------" | tee -a $ofile
#echo "PUT /${namesp}/${name}" | tee -a $ofile
#echo "${payload}" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"' | tee -a $ofile

#echo "" | tee -a $ofile
#echo "REST Response:" | tee -a $ofile
#echo "-------------------------------" | tee -a $ofile
#PUT /${namesp}/${name} "${payload}" -v -k 2>/tmp/.rstErr | tee -a $ofile
#if [ $? -ne 0 ] ; then
#  echo "Failed.. Error output:"
#  cat /tmp/.rstErr
#fi
#rm $ofile
#rm /tmp/.rstErr

# Now check the response via WebSockets
export NODE_TLS_REJECT_UNAUTHORIZED=0

rm $ofile >/dev/null 2>/dev/null
ofile="/tmp/api-response"
echo "" > $ofile

echo "REST Request:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "PUT /${APITESTNAMESPACE}/${APITESTCLASS}" | tee -a $ofile
echo "${APITESTPAYLOAD}" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"' | tee -a $ofile

echo "" | tee -a $ofile
echo "WebSocket Request:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "{ \"namespace\":\"${APITESTNAMESPACE}\", \"name\":\"${APITESTCLASS}\", \"id\":\"fooid\", \"args\":${APITESTPAYLOAD} }" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"' | tee -a $ofile

echo "" | tee -a $ofile
echo "Response:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "{ \"namespace\":\"${APITESTNAMESPACE}\", \"name\":\"${APITESTCLASS}\", \"id\":\"fooid\", \"args\":${APITESTPAYLOAD} }" | node sendwebsocket.js "$APITESTUSER" "$APITESTPASS" | tee -a $ofile
