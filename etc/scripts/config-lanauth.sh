#!/bin/sh

LOG="echo LANAUTH: "

#include global
. /etc/scripts/global.sh

start() {
    get_param
    if [ "$1" ] || [ "$vpnEnabled" = "on" ]; then
	if [ -x /bin/lanauth ] && [ "$pwd" ] && [ "$lvl" ]; then
             $LOG "Starting lanauth $1 mode $lvl"
             lanauth -v 2 -l $lvl -p $pwd -A 0 &
	fi
    fi
}

stop() {
 $LOG "Stopping lanauth "
 killall -q lanauth
 killall -q -9 lanauth
}

reload() {
    if [ ps | grep -q lanaut[h] ]
    then
	if [ "$lvl" = "1" ]
	then
	    killall -q -USR1 lanauth
	elif [ "$lvl" = "2" ]
	then
	    killall -q -USR2 lanauth
	else
	    stop
	fi
    else
	start force
    fi
}

get_param() {
  pwd=`nvram_get 2860 vpnPassword`
  lvl=`nvram_get 2860 LANAUTH_LVL`
}


case "$1" in
       start)
           start
           ;;

       stop)
           stop
           ;;

       restart)
           stop
           start force
           ;;

	reload)
	   reload
	   ;;

       *)
           echo $"Usage: $0 {start|stop|restart|reload}"
           exit 1
esac


