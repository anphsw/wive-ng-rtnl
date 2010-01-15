#!/bin/sh

CONF_DIR=/etc/ppp/
PPTP_FILE=/etc/options.pptp
CHAP_FILE=$CONF_DIR/chap-secrets

if [ ! -n "$2" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password>"
  exit 0
fi

PPTP_USER_NAME="$1"
PPTP_PASSWORD="$2"

if [ ! -d $CONF_DIR ] ; then mkdir -p $CONF_DIR; fi

if [ -n "$PPPT_USER_NAME" ] ; then
echo "# Secrets for authentication using CHAP
      # client	server	secret	IP addresses" > $CHAP_FILE
echo "\"$PPTP_USER_NAME\"  *	\"$PPTP_PASSWORD\" *" >> $CHAP_FILE
fi


echo "# Lock the port" > $PPTP_FILE
echo "lock" >> $PPTP_FILE

echo "# Authentication
# We don't need the tunnel server to authenticate itself" >> $PPTP_FILE
echo "noauth" >> $PPTP_FILE

echo "# We won't do EAP, CHAP, or MSCHAP, but we will accept MSCHAP-V2" >> $PPTP_FILE
echo "refuse-eap" >> $PPTP_FILE
echo "refuse-chap" >> $PPTP_FILE
echo "refuse-mschap" >> $PPTP_FILE

echo "# Compression 
# Turn off compression protocols we know won't be used" >> $PPTP_FILE
echo "nobsdcomp" >> $PPTP_FILE
echo "nodeflate" >> $PPTP_FILE

echo "# Encryption
# (There have been multiple versions of PPP with encryption support,
# choose with of the following sections you will use.  Note that MPPE
# requires the use of MSCHAP-V2 during authentication)" >> $PPTP_FILE
echo "#require-mppe-128" >> $PPTP_FILE
echo "#mppe required,stateless" >> $PPTP_FILE
