#!/bin/bash


# $1, opt -check or -debug
# $2, CoMSS model to check or cmd to debug
# $3, CNF instance

./maxhs $3 > /tmp/out.opt 2> /dev/null
cat /tmp/out.opt | grep "^s UNSATISFIABLE"


if  [ $? -eq 0 ]; then   
# hard part is unsat
exit 124; 
fi

cat /tmp/out.opt | grep "^o 0" > /dev/null

if [ $? -eq 0 ]; then
# hard and soft part are sat
exit 124;
fi
 
rm -f /tmp/out.opt

if [ "$1" == "-check" ]; then
    cp $2 /tmp/resultat.res
elif [ "$1" == "-debug" ]; then
   $2 $3 > /tmp/resultat.res 2>/dev/null
   if [ $? -ne 0 ]; then 
       echo "!!Failed command: '$2' "
       exit 3; 
   fi
else
    echo "!UNKNOWN opt: $1"
    exit 1;
fi


cat $3 | grep -v "c" > /tmp/clean.wcnf
cp /tmp/clean.wcnf /tmp/init.wcnf

nbVar=`cat $3 | grep "^p wcnf" | cut -d ' ' -f3`
nbClauses=`cat $3 | grep "^p wcnf" | cut -d ' ' -f4`
weight=`cat $3 | grep "^p wcnf" | cut -d ' ' -f5` 
list=`cat /tmp/resultat.res | grep "^v " | cut -d ' ' -f2-`

rm -f /tmp/coMssInstance.wcnf

revList=""
for i in `echo $list`
do
    revList="$i $revList"
done

echo "p wcnf $nbVar $nbClauses $weight" > /tmp/instance.wcnf
for i in `echo $revList`
do
    if [ $i -ne 0 ]
    then
	w=`sed $i'!d' /tmp/init.wcnf | cut -d ' ' -f1`
	if [ "$w" -ge "$weight" ]; then
	    echo "!WRONG CoMSS, Clause at indice $i is a HARD one"
	    exit 2;
	fi
        sed -n "$i"p /tmp/clean.wcnf >> /tmp/coMssInstance.wcnf
        sed "$i"d /tmp/clean.wcnf > /tmp/tmpDel.wcnf
        mv /tmp/tmpDel.wcnf /tmp/clean.wcnf
    fi
done

nbClauses=`cat /tmp/clean.wcnf | wc -l`

sed -i "1i`echo "p wcnf $nbVar $nbClauses $weight"`" /tmp/clean.wcnf;


./maxhs /tmp/clean.wcnf | grep "^o 0" > /dev/null 
if [ $? -ne 0 ]
then
    echo "the subproblem is UNS"
    exit 2
fi


for i in `seq 1 \`cat /tmp/coMssInstance.wcnf | wc -l\``
do
    echo "p wcnf $nbVar $nbClauses $weight" > /tmp/TMP.wcnf
    sed -n "$i"p /tmp/coMssInstance.wcnf >> /tmp/TMP.wcnf
    cat /tmp/TMP.wcnf /tmp/clean.wcnf > /tmp/instanceTMP.wcnf

    ./maxhs /tmp/instanceTMP.wcnf | grep "^o 1" > /dev/null 
    if [ $? -ne 0 ]
    then            
        tmp=`sed -n "$i"p /tmp/coMssInstance.wcnf`
        
        printf "problem is not a coMSS: $tmp\n"
        exit 2
    fi
done

rm -f /tmp/instanceTMP.wcnf


printf "OK   \n"
exit 0
