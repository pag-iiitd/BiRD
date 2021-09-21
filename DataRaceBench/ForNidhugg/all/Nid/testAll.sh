mkdir  -p  'Nid_out'
for  x  in *.ll;  do 
echo $x 
time_start=$(($(date +%s%N)/1000000))
nidhugg --pso $x >> "Nid_out/${x%.ll}.out"
time_end=$(($(date +%s%N)/1000000))
let time_total=time_end-time_start
echo "Total time in milliseconds:"
echo $time_total
done

