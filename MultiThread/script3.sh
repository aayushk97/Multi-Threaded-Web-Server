#!/bin/sh


x=2
res1=$(date +%s.%N)

HOST='10.52.8.238 8888'

#CMD='GET / HTTP/1.0'

(
echo open "$HOST"
sleep 2
echo "GET /New/2.txt HTTP/1.1" 
sleep 2
echo "GET /New/2.txt HTTP/1.1" 
sleep 2
echo "GET /New/2.txt HTTP/1.1" 
sleep 2
echo "GET /New/2.txt HTTP/1.1" 
sleep 2
echo "GET /New/2.txt HTTP/1.1" 
sleep 2
#echo "exit"
) | telnet
sleep 2
		

res2=$(date +%s.%N)
dt=$(echo "$res2 - $res1" | bc)
dd=$(echo "$dt/86400" | bc)
dt2=$(echo "$dt-86400*$dd" | bc)
dh=$(echo "$dt2/3600" | bc)
dt3=$(echo "$dt2-3600*$dh" | bc)
dm=$(echo "$dt3/60" | bc)
ds=$(echo "$dt3-60*$dm" | bc)

LC_NUMERIC=C printf "Total runtime: %d:%02d:%02d:%02.4f\n" $dd $dh $dm $ds


