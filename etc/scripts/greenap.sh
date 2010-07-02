setGreenAP()
{
	start=`nvram_get 2860 GreenAPStart1`
	end=`nvram_get 2860 GreenAPEnd1`
	action=`nvram_get 2860 GreenAPAction1`
	if [ "$action" = "WiFiOFF" ]; then
		echo "$start * * * ip link set ra0 down" >> /etc/crontabs/admin
		echo "$end * * * ip link set ra0 up" >> /etc/crontabs/admin
	elif [ "$action" = "TX25" ]; then
		echo "$start * * * greenap.sh txpower 25" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX50" ]; then
		echo "$start * * * greenap.sh txpower 50" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX75" ]; then
		echo "$start * * * greenap.sh txpower 75" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	fi
	start=`nvram_get 2860 GreenAPStart2`
	end=`nvram_get 2860 GreenAPEnd2`
	action=`nvram_get 2860 GreenAPAction2`
	if [ "$action" = "WiFiOFF" ]; then
		echo "$start * * * ip link set ra0 down" >> /etc/crontabs/admin
		echo "$end * * * ip link set ra0 up" >> /etc/crontabs/admin
	elif [ "$action" = "TX25" ]; then
		echo "$start * * * greenap.sh txpower 25" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX50" ]; then
		echo "$start * * * greenap.sh txpower 50" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX75" ]; then
		echo "$start * * * greenap.sh txpower 75" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	fi
	start=`nvram_get 2860 GreenAPStart3`
	end=`nvram_get 2860 GreenAPEnd3`
	action=`nvram_get 2860 GreenAPAction3`
	if [ "$action" = "WiFiOFF" ]; then
		echo "$start * * * ip link set ra0 down" >> /etc/crontabs/admin
		echo "$end * * * ip link set ra0 up" >> /etc/crontabs/admin
	elif [ "$action" = "TX25" ]; then
		echo "$start * * * greenap.sh txpower 25" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX50" ]; then
		echo "$start * * * greenap.sh txpower 50" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX75" ]; then
		echo "$start * * * greenap.sh txpower 75" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	fi
	start=`nvram_get 2860 GreenAPStart4`
	end=`nvram_get 2860 GreenAPEnd4`
	action=`nvram_get 2860 GreenAPAction4`
	if [ "$action" = "WiFiOFF" ]; then
		echo "$start * * * ip link set ra0 down" >> /etc/crontabs/admin
		echo "$end * * * ip link set ra0 up" >> /etc/crontabs/admin
	elif [ "$action" = "TX25" ]; then
		echo "$start * * * greenap.sh txpower 25" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX50" ]; then
		echo "$start * * * greenap.sh txpower 50" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	elif [ "$action" = "TX75" ]; then
		echo "$start * * * greenap.sh txpower 75" >> /etc/crontabs/admin
		echo "$end * * * greenap.sh txpower normal" >> /etc/crontabs/admin
	fi
}

case $1 in
	"init")
		killall -q crond
		mkdir -p /var/spool/cron/crontabs
		rm -f /etc/crontabs/admin
		cronebl="0"
		action=`nvram_get 2860 GreenAPAction1`
		if [ "$action" != "Disable" ] && [ "$action" != "" ]; then
			start=`nvram_get 2860 GreenAPStart1`
			cronebl="1"
			greenap.sh setchk $start
		fi
		action=`nvram_get 2860 GreenAPAction2`
		if [ "$action" != "Disable" ] && [ "$action" != "" ]; then
			start=`nvram_get 2860 GreenAPStart2`
			cronebl="1"
			greenap.sh setchk $start
		fi
		action=`nvram_get 2860 GreenAPAction3`
		if [ "$action" != "Disable" ] && [ "$action" != "" ]; then
			start=`nvram_get 2860 GreenAPStart3`
			cronebl="1"
			greenap.sh setchk $start
		fi
		action=`nvram_get 2860 GreenAPAction4`
		if [ "$action" != "Disable" ] && [ "$action" != "" ]; then
			start=`nvram_get 2860 GreenAPStart4`
			cronebl="1"
			greenap.sh setchk $start
		fi
		if [ "$cronebl" = "1" ]; then
			crond
		fi
		;;
	"setchk")
		if [ "$2" -lt "1" ]; then
			if [ "$3" -lt "1" ]; then
				hour=23
			else
				hour=`expr $3 - 1`
			fi
			minute=`expr 60 + $2 - 1`
		else
			hour=$3
			minute=`expr $2 - 1`
		fi
		echo "$minute $hour * * * greenap.sh chkntp" >> /etc/crontabs/admin
		;;
	"chkntp")
		cat /etc/crontabs/admin | sed '/ifconfig/d' > /etc/crontabs/admin
		cat /etc/crontabs/admin | sed '/txpower/d' > /etc/crontabs/admin
		index=1
		while [ "$index" -le 10 ]
		do
			ntpvalid=`nvram_get 2860 NTPValid`
			if [ "$ntpvalid" = "1" ]; then
				setGreenAP
				break;
			else
				index=`expr $index + 1`
				sleep 5
			fi
		done
		killall -q crond
		crond
		;;
	"txpower")
		if [ "$2" = "normal" ]; then
			ralink_init gen 2860
			ip link set ra0 down
			ip link set ra0 up
		else
			cat /etc/Wireless/RT2860/RT2860.dat | sed '/TxPower/d' > /etc/Wireless/RT2860/RT2860.dat
			echo "TxPower=$2" >> /etc/Wireless/RT2860/RT2860.dat
			ip link set ra0 down
			ip link set ra0 up
		fi
		;;
esac
