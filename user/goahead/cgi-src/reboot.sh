#!/bin/sh

echo "Content-type: text/html"
echo ''
echo '<html>'
echo '<head><script type="text/javascript" src="/js/ajax.js"></script></head>'
echo '<body><script lang=\"JavaScript\">ajaxReloadDelayedPage(60000);</script></body>'
echo '</html>'

fs backup_nvram
fs save
sync
sleep 2 && reboot &
