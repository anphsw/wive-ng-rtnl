
///////////////UPLOAD OPTIONS/////////////////
#define RFC_ERROR 		"RFC1867 ...."
#define REFRESH_TIMEOUT         "80000"         /* 80000 = 80 secs*/
#define MEM_SIZE        	1024
#define MEM_HALF        	512

///////////////MONITORING OPTIONS/////////////
#define CPU_MEM_RRD     	"/var/cpu_mem.rrd"
#define SAMPLE          	2
#define RRDTOOL         	"/bin/rrdtool"

///////////////UPDATE OPTIONS/////////////////
#define IH_MAGIC    0x27051956
#define IH_NMLEN    32 
#define MIN_SPACE_FOR_FIRMWARE  (1024 * 1024 * 8) // minimum space for firmware upload                                       
#define MIN_FIRMWARE_SIZE       (1048576) 	  // minium firmware size(1MB)                                

#define DEFAULT_LAN_IP 		"192.168.1.1"
