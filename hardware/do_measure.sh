#! /bin/bash

EXES="../counter counter_ultimate counter_swap counter_get_and_add counter_test_and_set counter_get_and_set counter_compare_and_swap"
OUT=measures_hardware.log

echo "Saving out to $OUT"

if [ -x /usr/bin/time ]
then 
	TIME='/usr/bin/time -f "  times: real %E, user %U, sys %S"'
else
	TIME=time
fi

cat /proc/cpuinfo > $OUT
echo "-----------" >> $OUT

for p  in $EXES
do
	echo -n Testing $p 
	echo >> $OUT
	echo -- \[$p\] -- >> $OUT
	for i in `seq 4`
	do
		(eval $TIME ./$p) &>> $OUT
		echo -n " [$i]" 
		sleep 1
	done
	echo

	echo --- >> $OUT
done

 
