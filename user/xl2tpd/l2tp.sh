#!/bin/sh
CONF_DIR=/etc/l2tp
CONF_FILE=$CONF_DIR/l2tpd.conf
L2TP_FILE=$CONF_DIR/options.l2tpd
CHAP_FILE=$CONF_DIR/chap-secrets

if [ ! -n "$3" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <srv_ip> <user> <password>"
  exit 0
fi

L2TP_SERV_IP="$1"
L2TP_USER_NAME="$2"
L2TP_PASSWORD="$3"

if [ ! -d $CONF_DIR ] ; then mkdir -p $CONF_DIR; fi

echo "
[global]
port = 1701
auth file = $CHAP_FILE
access control = no
rand source = dev

[lac seubras]
lns = $L2TP_SERV_IP
redial = yes
redial timeout = 15
max redials = 5
require chap = yes
refuse pap = no
refuse authentication = no
require authentication = yes
name = $L2TP_USER_NAME
ppp debug = no
pppoptfile = $L2TP_FILE " > $CONF_FILE

echo "noauth
proxyarp
defaultroute" > $L2TP_FILE

if [ -n "$L2TP_USER_NAME" ] ; then
echo "# Secrets for authentication using CHAP
      # client	server	secret	IP addresses" > $CHAP_FILE
echo "\"$L2TP_USER_NAME\"  *	\"$L2TP_PASSWORD\" *" >> $CHAP_FILE
fi

