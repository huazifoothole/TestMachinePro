#/bin/bash
var=$(grep localIP  networkIP.ini |awk -F '=' '{print $2}')
var2=$(grep netInterface networkIP.ini |awk -F '=' '{print $2}')
ifconfig $var2 $var
touch testFile
