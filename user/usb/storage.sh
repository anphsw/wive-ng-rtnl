#!/bin/sh

##################################################################
#################THIS IS STUB/DEPRECATED LOGIC####################
####################REMOVE IN FUTURE##############################
##################################################################

exit 0

#
# usage: storage.sh
#

PART1=""

echo "run storage.sh"
for part in a b c d e f g h i j k l m n o p q r s t u v w x y z
do
	for index in 1 2 3 4 5 6 7 8 9
	do
		if [ -e "/media/sd$part$index" ]; then
			PART1="/media/sd$part$index"
			break;
		fi
	done
	if [ "$PART1" != "" ]; then
		break;
	fi
done

setFtp()
{
	ftpport=`nvram_get 2860 FtpPort`
	maxuser=`nvram_get 2860 FtpMaxUsers`
	loginT=`nvram_get 2860 FtpLoginTimeout`
	stayT=`nvram_get 2860 FtpStayTimeout`
	stupid-ftpd-common.sh "$ftpport" "$maxuser" "$loginT" "$stayT"
	echo "stupid-ftpd-common.sh "$ftpport" "$maxuser" "$loginT" "$stayT""
	admID=`nvram_get 2860 Login`
	admPW=`nvram_get 2860 Password`
	stupid-ftpd-user.sh "$admID" "$admPW" / 3 A
	echo "stupid-ftpd-user.sh "$admID" "$admPW" / 3 A"
	anonymous=`nvram_get 2860 FtpAnonymous`
	if [ "$anonymous" = "1" ]; then
		stupid-ftpd-user.sh anonymous "*" /tmp 3 D	
		echo "stupid-ftpd-user.sh anonymous "*" /tmp 3 D"
	fi
	if [ -e "$PART1" ]; then
		for index in 1 2 3 4 5 6 7 8
		do
			user=`nvram_get 2860 "User$index"`
			ftpuser=`nvram_get 2860 "FtpUser$index"`
			if [ "$user" ] && [ "$ftpuser" = "1" ]; then
				pw=`nvram_get 2860 "UserPasswd$index"`
				max=`nvram_get 2860 "FtpMaxLogins$index"`
				mode=`nvram_get 2860 "FtpMode$index"`
				stupid-ftpd-user.sh "$user" "$pw" "$PART1/$user" "$max" "$mode"
				echo "stupid-ftpd-user.sh "$user" "$pw" "$PART1/$user" "$max" "$mode""
			fi
		done
	fi
}

setSmb()
{
	smbnetbios=`nvram_get 2860 SmbNetBIOS`
	smbwg=`nvram_get 2860 HostName`
	samba.sh "$smbnetbios" "$smbwg" 
	echo "samba.sh "$smbnetbios" "$smbwg""
	admID=`nvram_get 2860 Login`
	admPW=`nvram_get 2860 Password`
	smbpasswd -a "$admID" "$admPW"
	echo "smbpasswd -a "$admID" "$admPW""
	allusers="$admID"
	if [ -e "$PART1" ]; then
		for index in 1 2 3 4 5 6 7 8
		do
			user=`nvram_get 2860 "User$index"`
			smbuser=`nvram_get 2860 "SmbUser$index"`
			if [ "$user" ] && [ "$smbuser" = "1" ]; then
				pw=`nvram_get 2860 "UserPasswd$index"`
				smbpasswd -a "$user" "$pw"
				echo "smbpasswd -a "$user" "$pw""
				allusers="$allusers $user"
			fi
		done
		if [ ! -e "$PART1/public" ]; then
			storage.sh adddir "$PART1/public"
			echo "storage.sh adddir "$PART1/public""
		fi
		samba_add_dir.sh Public "$PART1/public" "$allusers"
		echo "samba_add_dir.sh Public "$PART1/public" "$allusers""
	fi
}

case $1 in
	"adddir")
		if [ -n "$2" ]; then
			mkdir -p "$2"
			chmod 777 "$2"
		fi
		;;
	"deldir")
		if [ -n "$2" ]; then
			rm -rf "$2"
		fi
		;;
	"reparted")
		fdisk -D /dev/sda
		echo "fdisk -D /dev/sda"
		sleep 1
		if [ "$2" -gt "0" ]; then
			fdisk /dev/sda -p 1 -v $2 
			echo "fdisk /dev/sda -p 1 -v $2 "
		fi
		sleep 1
		if [ "$3" -gt "0" ]; then
			fdisk /dev/sda -p 2 -v $3 
			echo "fdisk /dev/sda -p 2 -v $3"
		fi
		sleep 1
		if [ "$4" -gt "0" ]; then
			fdisk /dev/sda -p 3 -v $4 
			echo "fdisk /dev/sda -p 3 -v $4"
		fi
		sleep 1
		if [ "$5" -gt "0" ]; then
			fdisk /dev/sda -p 4 -v $5 
			echo "fdisk /dev/sda -p 4 -v $5"
		fi
		sleep 1
		fdisk -r /dev/sda
		echo "fdisk -r /dev/sda"
		sleep 1
		mkdosfs -F 32 /dev/sda1
		echo "mkdosfs -F 32 /dev/sda1"
		sleep 1
		mkdosfs -F 32 /dev/sda2
		echo "mkdosfs -F 32 /dev/sda2"
		sleep 1
		mkdosfs -F 32 /dev/sda3
		echo "mkdosfs -F 32 /dev/sda3"
		sleep 1
		mkdosfs -F 32 /dev/sda4
		echo "mkdosfs -F 32 /dev/sda4"
		sleep 1
		reboot
		;;	
	"ftp")
		killall -q stupid-ftpd
		ftpenabled=`nvram_get 2860 FtpEnabled`
		if [ "$ftpenabled" = "1" ]; then
			setFtp
			stupid-ftpd
			echo "stupid-ftpd"
		fi
		;;
	"samba")
		killall -q nmbd
		killall -q smbd
		smbenabled=`nvram_get 2860 SmbEnabled`
		if [ "$smbenabled" = "1" ]; then
			setSmb
		fi
		;;
	"media")
		killall -q ushare
		media_enabled=`nvram_get 2860 mediaSrvEnabled`
		media_name=`nvram_get 2860 mediaSrvName`
		if [ "$media_enabled" = "1" ]; then
			ushare.sh $media_name "$2" "$3" "$4" "$5"
			echo "ushare.sh $media_name "$2" "$3" "$4" "$5""
			ushare -D
			echo "ushare -D"
		fi
		;;
esac
