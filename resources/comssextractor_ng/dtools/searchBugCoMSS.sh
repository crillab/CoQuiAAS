#!/bin/bash

# $1, opt -cnf or -wcnf
# $2, the command

nbInst=1
cpt=0


######################## CNF #######################
if [ "$1" == "-cnf" ]; then
    while [ true ]
    do
	printf "number of instances tested %d\r" "$cpt"

	if [ $nbInst -gt 20 ]
	then 
            exit 0
	fi

	./cnfuzz > /tmp/test.cnf
	./minisat /tmp/test.cnf > /dev/null 2>/dev/null

	code=$?

	if [ $code -eq 10 ]
	then
            continue
	elif [ $code -eq 20 ]
	then
            cpt=`expr $cpt + 1`
            $2 /tmp/test.cnf > /tmp/solution.res  2>/dev/null
	    if [ $? -ne 0 ]; then 
		echo "!!Failed command: '$2' "
		exit 3; 
	    fi
            ./debugCoMSS.sh -check /tmp/solution.res /tmp/test.cnf > /dev/null

            if [ $? -ne 0 ]
            then
		echo "/tmp/${nbInst}test.cnf" `cat /tmp/test.cnf | grep "^p cnf" | cut -d ' ' -f3-`
		cp /tmp/test.cnf /tmp/${nbInst}test.cnf
		nbInst=`expr $nbInst + 1`

		if [ $nbInst -gt 10 ]
		then
                    exit 0
		fi
            fi
	else
            echo "Minisat problem???"
            exit 0
	fi
    done
###################### WCNF ##########################
elif [ "$1" == "-wcnf" ]; then
    while [ true ]
    do
	printf "number of instances tested %d\r" "$cpt"

	if [ $nbInst -gt 20 ]
	then 
            exit 0
	fi

	./wcnfuzz > /tmp/test.wcnf
	./maxhs /tmp/test.wcnf >  /tmp/out.opt 2> /dev/null 

	cat /tmp/out.opt | grep "^s UNSAT" > /dev/null
	code1=$?
	
	cat /tmp/out.opt | grep "^o 0" > /dev/null
	code2=$?

	if [ $code1 -eq 0 ]
	then
            continue
	elif [ $code2 -eq 0 ]
	then
            continue
	elif [ $code1 -eq 1 ]
	then
            cpt=`expr $cpt + 1`

            $2 /tmp/test.wcnf > /tmp/solution.res  2>/dev/null
	    if [ $? -ne 0 ]; then 
		echo "!!Failed command: '$2' "
		exit 3; 
	    fi
            ./debugCoMSS4PMS.sh -check /tmp/solution.res /tmp/test.wcnf > /dev/null

            if [ $? -ne 0 ]
            then
		echo "/tmp/${nbInst}test.wcnf" `cat /tmp/test.wcnf | grep "^p wcnf" | cut -d ' ' -f3-`
		cp /tmp/test.wcnf /tmp/${nbInst}test.wcnf
		nbInst=`expr $nbInst + 1`

		if [ $nbInst -gt 10 ]
		then
                    exit 0
		fi
            fi
	else
            echo "maxhs problem???"
            exit 0
	fi
    done
else
    echo "!UNKNOWN opt: $1"
    exit 0;
fi

