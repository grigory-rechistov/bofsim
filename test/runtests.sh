#!/usr/bin/env bash

mkdir -p logs

let RESULT=0
for TST in test*.exe
do
    echo -n "Running $TST: "
    ./$TST 2> logs/$TST.stderr.log > logs/$TST.stdout.log
    if [ $? != 0 ]
    then
        echo "Failure";
        let RESULT++
    else
        echo "OK"
    fi
done

exit $RESULT
