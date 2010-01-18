case $1 in
	"init")
		brctl delif br0 mesh0
		ifconfig mesh0 down
		ifconfig ra0 down
		ralink_init make_wireless_config 2860
		ifconfig ra0 up
		meshenabled=`nvram_get 2860 MeshEnabled`
		if [ "$meshenabled" = "1" ]; then
			ifconfig mesh0 up
			brctl addif br0 mesh0
			meshhostname=`nvram_get 2860 MeshHostName`
			iwpriv mesh0 set  MeshHostName="$meshhostname"
		fi
		;;
	"addlink")
		iwpriv mesh0 set MeshAddLink="$2"
		echo "iwpriv mesh0 set MeshAddLink="$2""
		;;
	"dellink")
		iwpriv mesh0 set MeshDelLink="$2"
		echo "iwpriv mesh0 set MeshDelLink="$2""
		;;
esac
