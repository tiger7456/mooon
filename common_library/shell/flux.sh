#!/bin/sh
# Writed by yijian on 2008-3-20
# Linux网卡流量统计工具
# 可带一个参数：网卡名，如eth0或eth1等
# 输出格式：统计时间,入流量(Kbps),入流量(Mbps),出流量(Kbps),出流量(Mbps)

# Please edit the followings
EthXname=eth0 # Interface name
StatFreq=2 # Seconds

if test $# -eq 1; then
	EthXname=$1
fi
echo "Destination: $EthXname"

# Don't change
influx_kbps=0
outflux_kbps=0
influx_mbps=0
outflux_mbps=0
unsigned_long_max=4294967295

Ethname=`cat /proc/net/dev|grep $EthXname|awk -F"[: ]+" '{ printf("%s", $2); }'`
if test "$EthXname" != "$Ethname"; then
	echo "Please set EthXname first before running"
	echo "Usage: flux.sh ethX"
	echo "Example: flux.sh eth0"
	exit
fi

influx1_byte=`cat /proc/net/dev|grep $EthXname|awk -F"[: ]+" '{ printf("%d", $3); }'`
outflux1_byte=`cat /proc/net/dev|grep $EthXname|awk -F"[: ]+" '{ printf("%d", $11); }'`

echo "Date,IN-Kbps,IN-Mbps,OUT-Kbps,OUT-Mbps"
while test 2 -gt 1;
do
	sleep $StatFreq
	#influx2_byte=`cat /proc/net/dev|grep $EthXname|awk -F"[: ]+" '{ printf("%d", $3); }'`
	#outflux2_byte=`cat /proc/net/dev|grep $EthXname|awk -F"[: ]+" '{ printf("%d", $11); }'`
	inout_bytes=`awk -F"[: ]+" /eth1/'{ printf("%s %s", $3, $11) }' /proc/net/dev`
	inout_bytes_array=($inout_bytes)
	influx2_byte=${inout_bytes_array[0]}
	outflux2_byte=${inout_bytes_array[1]}

	dd=`date +'%Y-%m-%d/%H:%M:%S'`
	if test $influx2_byte -ge $influx1_byte; then
		let influx_byte=$influx2_byte-$influx1_byte
	else
		let influx_byte=$unsigned_long_max-$influx1_byte
		let influx_byte=$influx_byte+$influx2_byte
	fi
	if test $outflux2_byte -ge $outflux1_byte; then
		let outflux_byte=$outflux2_byte-$outflux1_byte
	else
		let outflux_byte=$unsigned_long_max-$outflux1_byte
		let outflux_byte=$outflux_byte+$outflux2_byte
	fi
		
	let influx_byte=$influx_byte/$StatFreq
	let outflux_byte=$outflux_byte/$StatFreq
	# TO bps
	let influx_bps=$influx_byte*8
	let outflux_bps=$outflux_byte*8
	# To kbps
	let influx_kbps=$influx_bps/1024
	let outflux_kbps=$outflux_bps/1024
	# To mbps
	let influx_mbps=$influx_kbps/1024
	let outflux_mbps=$outflux_kbps/1024
	# SHOW on screen

	# COLUMN: Date,IN-Kbps,IN-Mbps,OUT-Kbps,OUT-Mbps
	echo "$dd,${influx_kbps}Kbps,${influx_mbps}Mbps,${outflux_kbps}Kbps,${outflux_mbps}Mbps"
	
	let influx1_byte=influx2_byte
	let outflux1_byte=outflux2_byte
done