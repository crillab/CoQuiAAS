#!/bin/bash

# $1, opt -check or -debug
# $2, CoMSS model to check or cmd to debug
# $3, CNF instance

./minisat $3 > /dev/null 2>/dev/null
if [ $? -eq 10 ]; then exit 124; fi

if [ "$1" == "-check" ]; then
    cp $2 /tmp/resultat.res
elif [ "$1" == "-debug" ]; then
   $2 $3 > /tmp/resultat.res 2>/dev/null
   if [ $? -ne 0 ]; then 
       echo "!!Failed command: '$2' "
       exit 3; 
   fi
else
    echo "!UNKNOWN opt"
    exit 0;
fi


cat $3 | grep -v "c" > /tmp/clean.cnf

nbVar=`cat $3 | grep "^p cnf" | cut -d ' ' -f3`
nbClauses=`cat $3 | grep "^p cnf" | cut -d ' ' -f5`
list=`cat /tmp/resultat.res | grep "^v " | cut -d ' ' -f2-`

rm -f /tmp/coMssInstance.cnf

revList=""
for i in `echo $list`
do
    revList="$i $revList"
done

echo "p cnf $nbVar $nbClauses" > /tmp/instance.cnf
for i in `echo $revList`
do
    if [ $i -ne 0 ]
    then 
        sed -n "$i"p /tmp/clean.cnf >> /tmp/coMssInstance.cnf
        sed "$i"d /tmp/clean.cnf > /tmp/tmpDel.cnf
        mv /tmp/tmpDel.cnf /tmp/clean.cnf
    fi
done

nbClauses=`cat /tmp/clean.cnf | wc -l`

./minisat /tmp/clean.cnf > /dev/null 2>/dev/null
if [ $? -ne 10 ]
then
    echo "the subproblem is not a SAT"
    exit 2
fi

for i in `seq 1 \`cat /tmp/coMssInstance.cnf | wc -l\``
do
    echo "p cnf $nbVar $nbClauses" > /tmp/TMP.cnf
    sed -n "$i"p /tmp/coMssInstance.cnf >> /tmp/TMP.cnf
    cat /tmp/TMP.cnf /tmp/clean.cnf > /tmp/instanceTMP.cnf

    ./minisat /tmp/instanceTMP.cnf > /dev/null 2>/dev/null
    if [ $? -ne 20 ]
    then            
        tmp=`sed -n "$i"p /tmp/coMssInstance.cnf`
        
        printf "problem is not a coMSS: $tmp\n"
        exit 2
    fi
done

rm -f /tmp/instanceTMP.cnf

printf "OK                         \n" "$i"
exit 0
