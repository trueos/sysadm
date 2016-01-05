#!/usr/local/bin/bash
# (Yes, yes, this is a bash script)
# Both resty/jsawk use bash'isms

# Set variable to call jsawk utility
JSAWK="./utils/jsawk -j js24"

echo "Enter your username:"
echo -e ">\c"
read fuser

echo ""
echo "Enter your password:"
echo -e ">\c"
read -s fpass
echo ""

# Source our resty functions
. ./utils/resty -W "http://127.0.0.1:12151" -H "Accept: application/json" -H "Content-Type: application/json" -u ${fuser}:${fpass}

# Check the reply of this REST query
PUT /sysadm/lifepreserver '{ "action":"listcron" }' -v 2>/tmp/.rstErr
if [ $? -ne 0 ] ; then
  echo "Failed.. Error output:"
  cat /tmp/.rstErr
fi
