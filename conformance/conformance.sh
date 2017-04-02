#!/bin/sh

timeout=10
cmd=./test.sh

function execute_cmd 
{
	# execute & get pid of command
	${cmd} $1 > /dev/null &
	pid=$!

	#
	result=0

	# wait until timout or command done
	tick=0
	cont=1

	while [ $cont -ne 0 ]
	do
		# done?
		jobs -l | grep " ${pid} Running" > /dev/null

		if [ $? -ne 0 ]
		then
			# [FIXME] check result, and update result if required
			# wait ${pid}
			# if [ $? -ne 0 ] ; then
			#    # do something
			# fi
			cont=0

			# escape from loop
			continue
		fi

		# check timeout
		if [ $tick -gt $timeout ]
		then

			# kill process
			kill ${pid}

			# update result
			result=2

			# escape from loop
			cont=0

			continue
		fi
		
		# update tick
		sleep 1;
		let tick++

	done

	return $result
}

function execute_list
{
	match=0
	mismatch=0
	hang=0
	all=0

	# called w/o argument
	if [ $# -ne 1 ]
	then
		echo "execute_list require argument"
		return
	fi

	# non-exsit list file
	if [ ! -f "$1" ]
	then
		echo "listfile($1) is not exist" 
		return
	fi

	echo start conformance on $1

	while read line
	do
		# trim parameter
		param=`echo ${line} | sed -e s/^[[:space:]]*//g -e s/[[:space:]].*$//g`

		echo Try ${param}

		execute_cmd ${param}
		result=$?

		echo ${param} done

		if [ $result -eq 0 ]
		then
			# matched
			echo result is matched
			let match++
		elif [ $result -eq 1 ]
		then
			# mismatched
			echo result is mismatched
			let mismatch++
		else
			# hangup
			echo hangup is detected
			let hang++
		fi

		let all++

	done < $1

	echo ""
	echo "Total count: ${all}"
	echo "Matched count: ${match}"
	echo "Mismatched count: ${mismatch}"
	echo "Hangup count: ${hang}"
}

function quit
{
	# kill sub processes (which is not recieved interrupt)
	list=`jobs -l | grep -v Interrupt | awk '{ print $2 }'`

	for x in ${list}
	do 
		kill $x
	done

	# kill this script
	exit
}

# trap CTRL-C
trap quit INT

# execute conformance test
execute_list $1

