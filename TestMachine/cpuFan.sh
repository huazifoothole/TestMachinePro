cat ./sensor.txt | grep fan2 |awk '{print $2,$3;}' |cut -c 1-4
