#!/bin/sh

if [ $1 -eq 3 ] ; 
then
	while [ 1 ] ; do
		echo for  hangup test
		sleep 5
	done
fi

for i in $1; do

	echo $i
	sleep 1

done

