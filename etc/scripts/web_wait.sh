#!/bin/sh

############################################################
# wait to goahead start					   #
############################################################

web_wait()
{
    # wait to start web and run from goahead code
    if [ ! -f /var/run/goahead.pid ]; then
      exit 0
    fi
}
