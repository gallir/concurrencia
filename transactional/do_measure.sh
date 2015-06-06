#! /bin/bash

#EXES="array_base array_tinystm intel/array_transaction intel/array_hle intel/array_rtm"
EXES="array_mutex"
N=$1
OUT=logs/measures_i5_mutex_$N.log

cp d$N.h defs.h

make all > /dev/null


echo "Saving out to $OUT"

if [ -x /usr/bin/time ]
then
    TIME='/usr/bin/time -f "  times: real %E, user %U, sys %S"'
else
    TIME=time
fi

echo "-----------" > $OUT

for p  in $EXES
do
    echo -n Testing $p 
    echo >> $OUT
    echo -- \[$p\] -- >> $OUT
    for i in `seq 5`
    do
        (eval $TIME ./$p) &>> $OUT
        echo -n " [$i]" 
        sleep 1
    done
    echo

    echo --- >> $OUT
done

