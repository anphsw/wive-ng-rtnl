#!/bin/sh

LOG="echo LANAUTH: "

start() {
    get_param
    if [ -x /sbin/lanauth ] && [ "$pwd" ] && [ "$lvl" ]; then
       if [ "$1" ] || [ "$start_on_boot" ] ; then
             $LOG "Starting lanauth $1"
             /sbin/lanauth -v 2 -l $lvl -p $pwd -A 0
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
  start_on_boot=`nvram_get 2860 LANAUTH_START`
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


