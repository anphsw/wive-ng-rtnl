#!/bin/sh

PPPOE_FILE=/etc/options.pppoe

if [ ! -n "$5" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <mtu> <mru> <eth_name>"
  exit 0
fi

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_MTU="$3"
PPPOE_MRU="$4"
PPPOE_IF="$5"

echo "noauth" > $PPPOE_FILE
echo "user '$PPPOE_USER_NAME'" >> $PPPOE_FILE
echo "password '$PPPOE_PASSWORD'" >> $PPPOE_FILE
echo "nomppe" >> $PPPOE_FILE
echo "hide-password" >> $PPPOE_FILE
echo "noipdefault" >> $PPPOE_FILE
echo "defaultroute" >> $PPPOE_FILE
echo "nodetach" >> $PPPOE_FILE
echo "usepeerdns" >> $PPPOE_FILE
echo "mru $PPPOE_MRU" >> $PPPOE_FILE
echo "mtu $PPPOE_MTU" >> $PPPOE_FILE
echo "persist" >> $PPPOE_FILE
echo "ipcp-accept-remote" >> $PPPOE_FILE 
echo "ipcp-accept-local" >> $PPPOE_FILE 
echo "lcp-echo-failure 3" >> $PPPOE_FILE
echo "lcp-echo-interval 20" >> $PPPOE_FILE
echo "plugin /etc_ro/ppp/plugins/rp-pppoe.so $PPPOE_IF" >> $PPPOE_FILE

