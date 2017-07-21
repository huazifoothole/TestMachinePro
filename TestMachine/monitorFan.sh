cat ./sensor.txt | grep fan3 |awk '{print $2,$3;}' |cut -c 1-4
