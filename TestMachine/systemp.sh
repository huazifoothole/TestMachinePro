cat ./sensor.txt | grep temp2 |awk '{print $2}'|cut -c 2-3
