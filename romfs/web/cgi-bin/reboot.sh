#!/bin/sh

echo "Content-type: text/html"
echo ""
IPADDR=`nvram_get 2860 lan_ipaddr`;
echo "<body><script lang=\"JavaScript\">function reload() { top.location.href=\"http://$IPADDR/\" }; setTimeout(reload, 40000);</script></body>"
fs backup_nvram
fs save
sync
sleep 2 && reboot &
