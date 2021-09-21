count=0
status=999
unsuccess=0
fail=0
input="/home/ridhi/Documents/threads/pin-2.14-71313-gcc.4.4.7-linux/source/tools/ManualExamples/endrun.out"
#$PIN_ROOT/pin -t obj-intel64/ReadWriteSets.so -filter_no_shared_libs -log -log:basename pinball/foo --  $APP
export FILE=aa1
export LOG=log
export BT=bt
export OUTPUT=output
rm $OUTPUT
>$OUTPUT
rm $LOG
>$LOG
rm $BT
>$BT
time_start=$(($(date +%s%N)/1000000))
while true
do
	line=$(head -1 $input)
	if [[ $line == "race" ]]
	then
		export VAR=`cat backtrack.out` 
		rm $FILE
		timeout -sKILL 1s $PIN_ROOT/pin.sh -t obj-intel64/Shadow_random.so -filter_no_shared_libs -- $APP >> $FILE
		let status=$?
		if [ $status == 134 ]
			then 
			let fail++
			let count++
			echo "ASSERTION FAILED at run " >> $LOG
			echo $count >> $LOG
			cat backtrack.out >> $LOG
			echo "*****************************************************************************************" >> $LOG
			echo "************************************************" >> $OUTPUT
			cat $FILE >> $OUTPUT
			echo "************************************************" >> $OUTPUT
			cat backtrack.out >> bt
		elif [ $status == 11 ] 
			then
			let unsuccess++
		elif [ $status -ge 135 ]
		  then
		  let unsuccess++	
		else 
			echo "************************************************" >> $OUTPUT
			cat $FILE >> $OUTPUT
			echo "************************************************" >> $OUTPUT
			cat backtrack.out >> bt
			let count++
		fi

	else
	  time_end=$(($(date +%s%N)/1000000)) 
		break
	fi


done < "$input"
echo "Total executions:"
echo $count
echo "For buggy executions check log file. Number of buggy interleavings: "
echo $fail
let time_total=time_end-time_start
echo "Total time in milliseconds:"
echo $time_total

