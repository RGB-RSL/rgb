#!/bin/bash
args=("$@")
len=$#
BINNAME="EskilasAgent"
DIRS=( 
		"Connection"
		"Decision"
		"Radar"
		"Utilities"
		"Utilities/Types"
		"Utilities/Geometry"
		"WorldGraph" 
		"WorldModel"
		"WorldModel/Objects"
)
DEPDIR=".dep"
OBJDIR=".obj"
LIBDIR=".lib"
LOGDIR="LogFiles"
OFFLINELOGDIR="LogFiles/Offline"
if [ $len -eq 0 ]; then
	printf "Removing Log Files..."
	rm -f $LOGDIR/*.log
	rm -f $OFFLINELOGDIR/*.offline
	printf  "\t\t\t [\e[0;32m Done \e[0m]\n"
	printf "Removing Binary..."
	rm -f $BINNAME > /dev/null
	printf  "\t\t\t [\e[0;32m Done \e[0m]\n"
	printf "Removing Dep And Obj Files ..."
	for PATHS in ${DIRS[@]}
	do
	    rm -f $PATHS/$DEPDIR/*.d > /dev/null
	    rm -f $PATHS/$OBJDIR/*.o > /dev/null
	done
	printf  "\t\t [\e[0;32m Done \e[0m]\n"
	printf "Removing Lib Files..."
	rm -f $LIBDIR/* > /dev/null
	printf  "\t\t\t [\e[0;32m Done \e[0m]\n"
elif [ ${args[0]} -eq 1 ]; then
	printf "Removing Log Files..."
	rm -f $LOGDIR/*.log
	rm -f $OFFLINELOGDIR/*.offline
	printf  "\t\t\t [\e[0;32m Done \e[0m]\n"
fi
