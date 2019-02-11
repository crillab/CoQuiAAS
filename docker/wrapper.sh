#!/bin/sh

scriptDir=`dirname $0`
runsolver=$scriptDir/runsolver
CoQuiAAS=$scriptDir/CoQuiAAS

timeout="$1"
shift

time "$runsolver" -w /dev/null -C "$timeout" -d 10 "$CoQuiAAS" $@
