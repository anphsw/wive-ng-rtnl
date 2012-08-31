#!/bin/sh

##########################################################################################################
#
# Autodetect 3G Modem port (Wive-NG-Project)
#
##########################################################################################################

LOG="logger -t 3G"

symlink_modem() {
    if [ ! -e /dev/modem ]; then
	ln -fs /dev/$MDEV /dev/modem
	MODEMFINDED=1
    fi
}

symlink_pcui() {
    if [ ! -e /dev/pcui ]; then
	ln -fs /dev/$MDEV /dev/pcui
    fi
}

get_param() {
    # get local param always
    eval `nvram_buf_get 2860 MODEMENABLED WMODEMPORT`
    PPPDIR="/etc/ppp"
    OPTFILE="$PPPDIR/peers/dialup"
}

create_conf() {
echo "
# Modem configuration file
# format of record - vid : pid : type : data port : UI port
# vid - vendor id (four hex digits), pid - product id (four hex digits),
# type - 'C' for CDMA or 'W' for WCDMA/HSPA/HSDPA/HSUPA/GSM
# data port - main modem interface number (one digit from 0 to 9)
# UI port - command interface number (one digit from 0 to 9)
# samples - a12b:1001:W:0:2, 01fc:003b:C:3:N

# HUAWEI Technology
12d1:1001:W:0:2
12d1:1003:W:0:1
12d1:1004:W:0:2
12d1:1401:W:3:2
12d1:1402:W:0:2
12d1:1436:W:0:4
12d1:140c:W:0:3
12d1:14ac:W:0:4
12d1:1506:W:0:1

# ZTE,Incorporated
19d2:0001:W:0:2
19d2:0002:W:2:4
19d2:0003:C:0:1
19d2:0004:C:0:1
19d2:0005:C:0:1
19d2:0006:C:0:1
19d2:0007:C:0:1
19d2:0008:C:0:1
19d2:0009:C:0:1
19d2:000a:C:0:1
19d2:0012:W:2:4
19d2:0015:W:1:3
19d2:0016:W:2:1
19d2:0017:W:2:1
19d2:0018:W:2:1
19d2:0019:W:2:1
19d2:0021:W:3:1
19d2:0024:W:2:4
19d2:0025:W:2:4
19d2:0030:W:1:3
19d2:0031:W:3:1
19d2:0033:W:4:1
19d2:0037:W:3:1
19d2:0042:W:3:1
19d2:0043:W:3:2
19d2:0048:W:4:2
19d2:0049:W:4:2
19d2:0052:W:3:1
19d2:0055:W:2:4
19d2:0061:W:3:1
19d2:0063:W:3:1
19d2:0064:W:2:0
19d2:0066:W:3:1
19d2:1218:W:2:4
19d2:2002:W:3:1
19d2:2003:W:3:1
" > /etc/modems.conf

}

get_param
if [ ! -e /etc/modems.conf ]; then
    create_conf
fi

if [ -z "$ACTION" ]; then
    ACTION="add"
fi

if [ "$ACTION" = "add" -a ! -e "/dev/modem" ]; then
    for path in $(find /sys/devices -name "$MDEV" | sort -r 2>/dev/null); do
        DEVPATH=${path#/sys}
    done
    [ -z "$DEVPATH" ] && $LOG "environment variable DEVPATH is unset" && exit 1
    if [ -d /sys${DEVPATH} ]; then
	cd /sys${DEVPATH}/../..
	for var in id[PV]*; do
	    [ -r $var ] && eval "$var='$(cat $var)'"
	done
	bInterfaceNumber=$(cat /sys${DEVPATH}/../bInterfaceNumber 2>/dev/null)
	bInterfaceProtocol=$(cat /sys${DEVPATH}/../bInterfaceProtocol 2>/dev/null)
	
	modem_conf=`cat /etc/modems.conf | awk "/^${idVendor}\:${idProduct}/ {split(\\$0,a,\":\"); print \"mVendor=\"a[1] \" mProduct=\"a[2] \" mType=\"a[3] \" mModemIf=\'0\"a[4] \"\' mUserIf=\'0\"a[5] \"\'\"}"`
	eval $modem_conf
	
	if [ $bInterfaceNumber = $mModemIf ]; then 
	    symlink_modem
	fi

	if [ $bInterfaceNumber = $mUserIf ]; then 
	    symlink_pcui
	fi
	
    fi

    if [ "$MDEV" = "$WMODEMPORT" ] || [ "$WMODEMPORT" = "AUTO" -a "$MODEMFINDED" = "1" ]; then
	$LOG "Modem added. Starting helper service -$MDEV-"
	service modemhelper start
    fi
elif [ "$ACTION" = "remove" ]; then
	if [ -e "/dev/modem" ]; then
	    DEVMODEMTTY=`ls -l  /dev/modem | cut -f 2 -d ">" | sed "s/ \/dev\///"`
	fi

	if [ -e "/dev/pcui" ]; then
	    DEVPCUITTY=`ls -l  /dev/pcui | cut -f 2 -d ">" | sed "s/ \/dev\///"`
	fi

	if [ -e $OPTFILE ]; then
	    PPPDTTY=`cat $OPTFILE | grep $MDEV | sed 's/^[ \t]*//;s/[ \t]*$//' `
	else
	    PPPDTTY=''
	fi

	if [ "$DEVPCUITTY" = "$MDEV" ]; then
	    $LOG "Modem removed. Deleting symlink."
	    rm -f /dev/pcui
	fi

	if [ "$DEVMODEMTTY" = "$MDEV" ]; then
	    $LOG "Modem removed. Deleting symlink."
	    rm -f /dev/modem
	    if [ -n `cat $OPTFILE | grep /dev/modem` ]; then
		NEEDSTOPHELPER=1
	    fi
	fi

	if [ "$PPPDTTY" = "/dev/$MDEV" ]; then
		$LOG "Modem removed."
		NEEDSTOPHELPER=1
	fi

	if [ "$NEEDSTOPHELPER" = "1" ]; then
	    $LOG "Stopping helper service."
	    service modemhelper stop
	fi
fi

exit 0
