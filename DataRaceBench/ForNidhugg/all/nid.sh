mkdir  -p  'Nid'
for  x  in *.c;  do  
time_start=$(($(date +%s%N)/1000000))
clang -emit-llvm -S -o "Nid/${x%.c}.ll" $x
time_end=$(($(date +%s%N)/1000000))
let time_total=time_end-time_start
echo $x
echo "Total time in milliseconds:"
echo $time_total
done
