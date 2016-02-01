#!/usr/local/bin/bash
# (Yes, yes, this is a bash script)
# Both resty/jsawk use bash'isms

# Set variable to call jsawk utility
JSAWK="./utils/jsawk -j js24"

which npm >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  pkg install -y npm
  if [ $? -ne 0 ] ; then
    echo "Requires npm!"
    exit 1
  fi
fi

pkg info p5-JSON >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  pkg install -y p5-JSON
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

echo "Enter your username:"
echo -e ">\c"
read fuser
echo ""

echo "Enter your password:"
echo -e ">\c"
read -s fpass
echo ""

echo "Enter the namespace:"
echo -e "(sysadm)>\c"
read namesp
if [ -z "$namesp" ] ; then
  namesp="sysadm"
fi
echo ""

echo "Enter the class name:"
echo -e "(lifepreserver)>\c"
read name
if [ -z "$name" ] ; then
  name="lifepreserver"
fi
echo ""

echo "Enter the payload json:"
echo -e "{ \"action\":\"listcron\" }>\c"
read payload
if [ -z "$payload" ] ; then
  payload="{ \"action\":\"listcron\" }"
fi
echo ""

# Source our resty functions
. ./utils/resty -W "https://127.0.0.1:12151" -H "Accept: application/json" -H "Content-Type: application/json" -u ${fuser}:${fpass}

# Save output to a file in addition to stdout
ofile="/tmp/api-response"
echo "" > /tmp/api-response

# Check the reply of this REST query
echo "" | tee -a $ofile
echo "REST Request:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "PUT /${namesp}/${name}" | tee -a $ofile
echo "${payload}" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"' | tee -a $ofile

echo "" | tee -a $ofile
echo "REST Response:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
PUT /${namesp}/${name} "${payload}" -v -k 2>/tmp/.rstErr | tee -a $ofile
if [ $? -ne 0 ] ; then
  echo "Failed.. Error output:"
  cat /tmp/.rstErr
fi


# Now check the response via WebSockets
export NODE_TLS_REJECT_UNAUTHORIZED=0

echo "" | tee -a $ofile
echo "WebSocket Request:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "{ \"namespace\":\"${namesp}\", \"name\":\"${name}\", \"id\":\"fooid\", \"args\":${payload} }" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"' | tee -a $ofile

echo "" | tee -a $ofile
echo "WebSocket Response:" | tee -a $ofile
echo "-------------------------------" | tee -a $ofile
echo "{ \"namespace\":\"${namesp}\", \"name\":\"${name}\", \"id\":\"fooid\", \"args\":${payload} }" | node sendwebsocket.js "$fuser" "$fpass" | tee -a $ofile
