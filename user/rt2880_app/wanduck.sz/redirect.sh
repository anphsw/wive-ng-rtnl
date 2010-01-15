
iptables -t nat -A PREROUTING -d ! 192.168.1.1/255.255.0.0 -p tcp --dport 80 -j DNAT --to-destination 192.168.1.1:18017
iptables -t nat -A PREROUTING -d 192.168.1.1/0.0.0.0 -p udp --dport 53 -j DNAT --to-destination 192.168.1.1:18018

iptables -t nat -D PREROUTING -d ! 192.168.1.1/255.255.0.0 -p tcp --dport 80 -j DNAT --to-destination 192.168.1.1:18017
iptables -t nat -D PREROUTING -d 192.168.1.1/0.0.0.0 -p udp --dport 53 -j DNAT --to-destination 192.168.1.1:18018
