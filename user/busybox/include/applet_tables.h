/* This is a generated file, don't edit */

#define NUM_APPLETS 114

const char applet_names[] ALIGN1 = ""
"[" "\0"
"[[" "\0"
"arp" "\0"
"arping" "\0"
"ash" "\0"
"awk" "\0"
"basename" "\0"
"brctl" "\0"
"bunzip2" "\0"
"bzcat" "\0"
"bzip2" "\0"
"cat" "\0"
"chmod" "\0"
"chpasswd" "\0"
"clear" "\0"
"cp" "\0"
"cut" "\0"
"date" "\0"
"dd" "\0"
"df" "\0"
"dmesg" "\0"
"dnsdomainname" "\0"
"dos2unix" "\0"
"du" "\0"
"dumpleases" "\0"
"echo" "\0"
"egrep" "\0"
"env" "\0"
"ether-wake" "\0"
"expr" "\0"
"false" "\0"
"fdisk" "\0"
"fgrep" "\0"
"find" "\0"
"free" "\0"
"ftpget" "\0"
"ftpput" "\0"
"fuser" "\0"
"grep" "\0"
"gunzip" "\0"
"gzip" "\0"
"halt" "\0"
"head" "\0"
"hostname" "\0"
"ifconfig" "\0"
"init" "\0"
"insmod" "\0"
"ionice" "\0"
"ip" "\0"
"kill" "\0"
"killall" "\0"
"killall5" "\0"
"klogd" "\0"
"ln" "\0"
"logger" "\0"
"login" "\0"
"logread" "\0"
"ls" "\0"
"lsmod" "\0"
"mdev" "\0"
"mesg" "\0"
"microcom" "\0"
"mkdir" "\0"
"mknod" "\0"
"modprobe" "\0"
"more" "\0"
"mount" "\0"
"netstat" "\0"
"nice" "\0"
"nslookup" "\0"
"passwd" "\0"
"ping" "\0"
"ping6" "\0"
"poweroff" "\0"
"printf" "\0"
"ps" "\0"
"pwd" "\0"
"rdate" "\0"
"reboot" "\0"
"renice" "\0"
"rm" "\0"
"rmdir" "\0"
"rmmod" "\0"
"route" "\0"
"sed" "\0"
"seq" "\0"
"sh" "\0"
"sleep" "\0"
"sort" "\0"
"sum" "\0"
"sync" "\0"
"sysctl" "\0"
"syslogd" "\0"
"tail" "\0"
"telnetd" "\0"
"test" "\0"
"time" "\0"
"top" "\0"
"touch" "\0"
"traceroute" "\0"
"true" "\0"
"udhcpc" "\0"
"udhcpd" "\0"
"umount" "\0"
"uname" "\0"
"unix2dos" "\0"
"uptime" "\0"
"vconfig" "\0"
"vi" "\0"
"wc" "\0"
"wget" "\0"
"xargs" "\0"
"yes" "\0"
"zcat" "\0"
;

int (*const applet_main[])(int argc, char **argv) = {
test_main,
test_main,
arp_main,
arping_main,
ash_main,
awk_main,
basename_main,
brctl_main,
bunzip2_main,
bunzip2_main,
bzip2_main,
cat_main,
chmod_main,
chpasswd_main,
clear_main,
cp_main,
cut_main,
date_main,
dd_main,
df_main,
dmesg_main,
hostname_main,
dos2unix_main,
du_main,
dumpleases_main,
echo_main,
grep_main,
env_main,
ether_wake_main,
expr_main,
false_main,
fdisk_main,
grep_main,
find_main,
free_main,
ftpgetput_main,
ftpgetput_main,
fuser_main,
grep_main,
gunzip_main,
gzip_main,
halt_main,
head_main,
hostname_main,
ifconfig_main,
init_main,
insmod_main,
ionice_main,
ip_main,
kill_main,
kill_main,
kill_main,
klogd_main,
ln_main,
logger_main,
login_main,
logread_main,
ls_main,
lsmod_main,
mdev_main,
mesg_main,
microcom_main,
mkdir_main,
mknod_main,
modprobe_main,
more_main,
mount_main,
netstat_main,
nice_main,
nslookup_main,
passwd_main,
ping_main,
ping6_main,
halt_main,
printf_main,
ps_main,
pwd_main,
rdate_main,
halt_main,
renice_main,
rm_main,
rmdir_main,
rmmod_main,
route_main,
sed_main,
seq_main,
ash_main,
sleep_main,
sort_main,
sum_main,
sync_main,
sysctl_main,
syslogd_main,
tail_main,
telnetd_main,
test_main,
time_main,
top_main,
touch_main,
traceroute_main,
true_main,
udhcpc_main,
udhcpd_main,
umount_main,
uname_main,
dos2unix_main,
uptime_main,
vconfig_main,
vi_main,
wc_main,
wget_main,
xargs_main,
yes_main,
gunzip_main,
};
const uint16_t applet_nameofs[] ALIGN2 = {
0x0000,
0x0002,
0x0005,
0x0009,
0x0010,
0x0014,
0x0018,
0x0021,
0x0027,
0x002f,
0x0035,
0x003b,
0x003f,
0x0045,
0x004e,
0x0054,
0x0057,
0x005b,
0x0060,
0x0063,
0x0066,
0x006c,
0x007a,
0x0083,
0x0086,
0x0091,
0x0096,
0x009c,
0x00a0,
0x00ab,
0x00b0,
0x00b6,
0x00bc,
0x00c2,
0x00c7,
0x00cc,
0x00d3,
0x00da,
0x00e0,
0x00e5,
0x00ec,
0x00f1,
0x00f6,
0x00fb,
0x0104,
0x010d,
0x0112,
0x0119,
0x0120,
0x0123,
0x0128,
0x0130,
0x0139,
0x013f,
0x0142,
0x8149,
0x014f,
0x0157,
0x015a,
0x0160,
0x0165,
0x016a,
0x0173,
0x0179,
0x017f,
0x0188,
0x018d,
0x0193,
0x019b,
0x01a0,
0x81a9,
0x41b0,
0x41b5,
0x01bb,
0x01c4,
0x01cb,
0x01ce,
0x01d2,
0x01d8,
0x01df,
0x01e6,
0x01e9,
0x01ef,
0x01f5,
0x01fb,
0x01ff,
0x0203,
0x0206,
0x020c,
0x0211,
0x0215,
0x021a,
0x0221,
0x0229,
0x022e,
0x0236,
0x023b,
0x0240,
0x0244,
0x424a,
0x0255,
0x025a,
0x0261,
0x0268,
0x026f,
0x0275,
0x027e,
0x0285,
0x028d,
0x0290,
0x0293,
0x0298,
0x029e,
0x02a2,
};
#define MAX_APPLET_NAME_LEN 13
