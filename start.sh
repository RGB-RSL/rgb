#! /bin/bash
args=("$@")
len=$#
#MAX_AGENT_NUM elements: FB FS PF PO AT AC 
MAX_AGENT_NUM=( 30 5 30 5 30 5 )
IP="localhost"
AGENT_BINARY=EskilasAgent
#killall -9 $AGENT_BINARY > /dev/null
rm -f ./LogFiles/*.log
rm -f ./LogFiles/Offline/*.offline
counter=0
while [ $counter -lt 6 ]; do
	if [ $counter -lt $len ] ; then
        if [ ${args[$counter]} != "-" ] ; then
            if [ ${args[$counter]} != "-all" ] ; then
				if [ ${args[$counter]} != "-1" ] ; then
                	let MAX_AGENT_NUM[$counter]=${args[$counter]}
				fi
            fi
        fi
	fi
	let counter=counter+1
done
if [ $len -ge 7 ] ; then
	IP=${args[6]}
fi

typeCounter=1
while [ $typeCounter -le 6 ]; do
        agentCounter=1
        while [ $agentCounter -le ${MAX_AGENT_NUM[typeCounter-1]} ]; do
                ./$AGENT_BINARY $typeCounter $IP 2> /dev/null &
                let agentCounter=agentCounter+1
        done
        let typeCounter=typeCounter+1
done
