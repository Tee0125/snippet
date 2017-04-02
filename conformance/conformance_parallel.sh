#!/bin/sh

num_process=4
timeout=10

cmd=./test.sh

function execute_cmd 
{
	# store session name
	session=$2
	echo ${session} start

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

	# mark session done
	echo ${session} done
	rm ${session}

	return $result
}

function execute_list_parallel
{
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

		cont=1

		while [ $cont -eq 1 ]
		do
			for ((i = 0 ; i < $num_process ; i++))
			do
				session=session${i}.run

				if [ ! -f ${session} ]
				then
					echo ${line} Starting

					touch ${session}
					execute_cmd ${param} ${session} &

					cont=0

					break
				fi
			done

			if [ $cont -eq 1 ]
			then
				sleep 1
			fi
		done
	done < $1

	wait
}

function env_cleanup
{
	for ((i = 0; i < $num_process ; i++))
	do
		session=session${i}.run

		if [ -f ${session} ]
		then
			rm ${session}
		fi
	done
}

# cleanup session files
env_cleanup

# execute conformance test
execute_list_parallel $1


