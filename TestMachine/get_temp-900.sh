#/bin/bash

ko_path=/lib/modules/$(uname -r)/kernel/drivers/hwmon
if  !  lsmod  |  fgrep w83627   -q ;   then
	modprobe w83627ehf
fi
if  !  lsmod  |  fgrep coretemp   -q ;   then
	insmod $ko_path/coretemp.ko
fi
sensors | grep temp1 |awk '{print $1 $2}'|cut -c 1-13
