#!/usr/local/bin/bash
# (Yes, yes, this is a bash script)
# Both resty/jsawk use bash'isms

# Set variable to call jsawk utility
JSAWK="./utils/jsawk -j js24"

which npm >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Please install npm first"
  exit 1
fi

pkg info p5-JSON >/dev/null 2>/dev/null
if [ $? -ne 0 ] ; then
  echo "Please install p5-JSON first"
  exit 1
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

# Check the reply of this REST query
echo ""
echo "REST Request:"
echo "-------------------------------"
echo "PUT /${namesp}/${name}"
echo "${payload}" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"'

echo ""
echo "REST Response:"
echo "-------------------------------"
PUT /${namesp}/${name} "${payload}" -v -k 2>/tmp/.rstErr
if [ $? -ne 0 ] ; then
  echo "Failed.. Error output:"
  cat /tmp/.rstErr
fi


# Now check the response via WebSockets
export NODE_TLS_REJECT_UNAUTHORIZED=0

echo ""
echo "WebSocket Request:"
echo "-------------------------------"
echo "{ \"namespace\":\"${namesp}\", \"name\":\"${name}\", \"id\":\"fooid\", \"args\":${payload} }" | perl -0007 -MJSON -ne'print to_json(from_json($_, {allow_nonref=>1}),{pretty=>1})."\n"'

echo ""
echo "WebSocket Response:"
echo "-------------------------------"
echo "{ \"namespace\":\"${namesp}\", \"name\":\"${name}\", \"id\":\"fooid\", \"args\":${payload} }" | node sendwebsocket.js "$fuser" "$fpass"
