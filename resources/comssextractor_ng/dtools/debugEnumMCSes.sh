#!/bin/bash

# $1, opt -check or -debug
# $2, CoMSS model to check or cmd to debug
# $3, CNF instance


if [ "$1" == "-check" ]; then
    cp $2 /tmp/resultat.res
elif [ "$1" == "-debug" ]; then
   $2 $3 > /tmp/resultat.res 2>/dev/null
   if [ $? -ne 0 ]; then 
       echo "!!failed command: '$2' "
       exit 3; 
   fi
else
    echo "!UNKNOWN opt: $1"
    exit 0;
fi

instance=$3
ext="${instance##*.}"
if [ "$ext" == "cnf" ]; then
    debuger="./debugCoMSS.sh"
elif [ "$ext" == "wcnf" ]; then
   debuger="./debugCoMSS4PMS.sh"
else
    echo "!ERROR format: .$ext"
    exit 0;
fi


#cat /tmp/resultat.res | grep -F "c MCS" > /tmp/enum.res
cat /tmp/resultat.res | grep -F "v " > /tmp/enum.res

nb1=`cat /tmp/enum.res | sort -u | wc -l`
nb2=`cat /tmp/enum.res | wc -l`

if [ $nb1 -ne $nb2  ]; then
    echo "!! redundant coMSS"
    exit 3;
fi
    
cpt=0
while read i
do
    cpt=`expr $cpt + 1` 

    #mcs=`echo "$i" | cut -d ' ' -f4-`
    #echo "v $mcs" > /tmp/sol.res
    mcs=`echo "$i"`
    echo "$mcs" > /tmp/sol.res

    $debuger -check /tmp/sol.res $3 > /dev/null 
    if [ $? -ne 0 ]; then
	echo "! WRONG MCS($cpt)"
	exit 2;
    fi
    printf "number of coMSS tested %d\r" "$cpt"

done < /tmp/enum.res

printf "\n"

########################### CNF #######################
if [ "$ext" == "cnf" ]; then
    cat $3 | grep -v "^c" > /tmp/block.cnf
    cat /tmp/block.cnf | grep -v "^p" > /tmp/TMP.cnf
    mv /tmp/TMP.cnf /tmp/block.cnf 
    nbInitVar=`cat $3 | grep "^p cnf" | cut -d ' ' -f3`
    nbVar=$nbInitVar

    ### Add selectors
    rm -f /tmp/TMP.cnf
    touch /tmp/TMP.cnf
    while read i
    do
	nbVar=`expr $nbVar + 1`
	echo "$nbVar $i" >> /tmp/TMP.cnf
    done < /tmp/block.cnf
    mv /tmp/TMP.cnf /tmp/block.cnf

    ### Add clauseBlock	
    while read i
    do
	#list=`echo "$i" | cut -d ' ' -f3-` ###c MCS(i): 1 2 3 ...n 0
	list=`echo "$i" | cut -d ' ' -f2-` ###v 1 2 3 ...n 0

	revList=""
	for j in `echo $list`
	do
	    if [ $j -ne 0 ]; then  revList="$j $revList"; fi
	done

	clBlock='0'
	for j in `echo $revList`
	do
	    lit=`expr $nbInitVar + $j`
	    clBlock="-$lit $clBlock" 
	done
	echo "$clBlock" >> /tmp/block.cnf
    done < /tmp/enum.res

    nbClauses=`cat /tmp/block.cnf | wc -l`
    sed -i "1ip cnf $nbVar $nbClauses" /tmp/block.cnf

    ./minisat /tmp/block.cnf > /dev/null 2> /dev/null
    if [ $? -ne 20 ]; then 
	echo "!!WARNING, missing MCSes"
	#exit 1
    fi
else ##################### WCNF ######################
    cat $3 | grep -v "^c" > /tmp/block.wcnf
    cat /tmp/block.wcnf | grep -v "^p" > /tmp/TMP.wcnf
    mv /tmp/TMP.wcnf /tmp/block.wcnf 
    nbInitVar=`cat $3 | grep "^p wcnf" | cut -d ' ' -f3`
    weight=`cat $3 | grep "^p wcnf" | cut -d ' ' -f5`
    nbVar=$nbInitVar

    ### Add selectors to soft clauses
    rm -f /tmp/TMP.wcnf
    touch /tmp/TMP.wcnf
    while read i
    do
	w=`echo "$i" | cut -d ' ' -f1`
	if [ $w -lt $weight ]; then ## soft
	    nbVar=`expr $nbVar + 1`
	    cl=`echo "$i" | cut -d ' ' -f2-`
	    echo "$w $nbVar $cl" >> /tmp/TMP.wcnf
	else ## hard
	    echo "$i" >> /tmp/TMP.wcnf
	fi
    done < /tmp/block.wcnf
    mv /tmp/TMP.wcnf /tmp/block.wcnf

    while read i
    do
	#list=`echo "$i" | cut -d ' ' -f3-` ###c MCS(i): 1 2 3 ...n 0
	list=`echo "$i" | cut -d ' ' -f2-` ###v 1 2 3 ...n 0

	revList=""
	for j in `echo $list`
	do
	    if [ $j -ne 0 ]; then  revList="$j $revList"; fi
	done

	clBlock='0'
	for j in `echo $revList`
	do
	    lit=`sed $j'!d' /tmp/block.wcnf | cut -d ' ' -f2`
	    clBlock="-$lit $clBlock" 
	done
	echo "$weight $clBlock" >> /tmp/block.wcnf
    done < /tmp/enum.res

    nbClauses=`cat /tmp/block.wcnf | wc -l`
    sed -i "1ip wcnf $nbVar $nbClauses $weight" /tmp/block.wcnf

    ./maxhs /tmp/block.wcnf | grep "^o 0" 2> /dev/null
    if [ $? -eq 0 ]; then 
	echo "!!WARNING, missing MCSes"
	exit 1
    fi 
   
fi

echo "OK "
exit 0
