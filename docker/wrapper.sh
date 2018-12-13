#!/bin/sh

scriptDir=`dirname $0`
runsolver=$scriptDir/runsolver
CoQuiAAS=$scriptDir/CoQuiAAS
lbx=$scriptDir/lbx

timeout="$1"
shift

"$runsolver" -C "$timeout" -d 10 "$CoQuiAAS" -lbx "$lbx" $@
