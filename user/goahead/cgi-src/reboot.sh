#!/bin/sh
echo "Content-type: text/html"
echo ""
echo "<body>rebooting</body>"
fs backup_nvram
fs save
sync
reboot &
