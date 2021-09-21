mkdir  -p  'Compiled'
for  x  in *.c;  do  gcc  $x  -o "Compiled/${x%.c}"  -lpthread;  done
