
# https://fasterdata.es.net/host-tuning/linux/
# https://fasterdata.es.net/host-tuning/linux/100g-tuning/

# allow TCP with buffers up to 2GB (Max allowed in Linux is 2GB-1)
echo 'net.core.rmem_max=2147483647' >> /etc/sysctl.conf
echo 'net.core.wmem_max=2147483647' >> /etc/sysctl.conf
echo 'net.core.rmem_default=1048576' >> /etc/sysctl.conf
echo 'net.core.wmem_default=1048576' >> /etc/sysctl.conf
# increase TCP autotuning buffer limits.
echo 'net.ipv4.tcp_rmem=4096 131072 1073741824' >> /etc/sysctl.conf
echo 'net.ipv4.tcp_wmem=4096 65536 1073741824' >> /etc/sysctl.conf
# recommended for hosts with jumbo frames enabled
echo 'net.ipv4.tcp_mtu_probing=1' >> /etc/sysctl.conf
# recommended to enable 'fair queueing'
echo 'net.core.default_qdisc = fq' >> /etc/sysctl.conf
# need to increase this to use MSG_ZEROCOPY
echo 'net.core.optmem_max = 1048576' >> /etc/sysctl.conf
# turn on window scaling which can be an option to enlarge the transfer window
echo 'net.ipv4.tcp_window_scaling = 1' >> /etc/sysctl.conf
# enable timestamps as defined in RFC1323
echo 'net.ipv4.tcp_timestamps = 1' >> /etc/sysctl.conf
# enable select acknowledgments
echo 'net.ipv4.tcp_sack = 1' >> /etc/sysctl.conf
# disable cache metrics on closing connections
echo 'net.ipv4.tcp_no_metrics_save = 1' >> /etc/sysctl.conf
# maximum number of packets, queued on the INPUT side
echo 'net.core.netdev_max_backlog = 5000' >> /etc/sysctl.conf
# reload
sysctl -p
