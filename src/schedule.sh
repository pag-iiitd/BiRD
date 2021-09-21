count=0
unsuccess=0
input="/home/ridhi/Documents/threads/pin-2.14-71313-gcc.4.4.7-linux/source/tools/ManualExamples/endrun.out"
#$PIN_ROOT/pin -t obj-intel64/ReadWriteSets.so -filter_no_shared_libs -log -log:basename pinball/foo --  $APP
export FILE=aa1 
export LOG=log
rm $LOG
>$LOG
while true
do
	line=$(head -1 $input)
	if [[ $line == "race" ]]
	then
	export VAR=`cat backtrack.out` 
	rm $FILE
	timeout -sKILL 3s $PIN_ROOT/pin.sh -t obj-intel64/Shadow_random.so -- $APP >> $FILE
	    if [ $? != 0 ]; then 
		if grep -q 'Assertion' aa1 
		then
 		    echo "ASSERTION FAILED at run" >> $LOG
 		    echo $count >> $LOG
		    cat backtrack.out >> $LOG
 		    echo "*****************************************************************************************" >> $LOG
 		fi
		let unsuccess++
	    else
		if grep -q 'Assertion' aa1 
		then
 		    echo "ASSERTION FAILED at run" >> $LOG
 		    echo $count >> $LOG
		    cat backtrack.out >> $LOG
 		    echo "*****************************************************************************************" >> $LOG
 		fi
		cat backtrack.out >> bt
		let count++
	    fi
	#echo "$VAR"
	else 
	break
	fi
echo $count
echo $unsuccess
done < "$input"


