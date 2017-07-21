#!/bin/bash
cputemp= cat ./sensor.txt | grep temp1 |awk '{print $2}'|cut -c 2-7
systemp= cat ./sensor.txt | grep temp2 |awk '{print $2}'|cut -c 2-7
fan= cat ./sensor.txt | grep fan3 |awk '{print $2,$3;}' |cut -c 1-10
AVCC= cat ./sensor.txt | grep AVCC |awk '{print $2,$3;}'|cut -c 2-9

echo $cputemp
echo $systemp
echo $fan
echo $AVCC
