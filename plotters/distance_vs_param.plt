filename = sprintf("%s.dat",ARG1)

set terminal png size 1050,762 enhanced font 'Verdana,10' #persist
set output sprintf("%s.png",ARG1)
set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2

set multiplot layout 2,3 rowsfirst

dummy_command = sprintf(" awk '{if(NR==2) print $1}' %s",filename)
number_bytes = system(dummy_command)

#doing plots of distance_vs_param

por2(i) = i*2

do for [i=1:6]{
   #set yrange[0:10]
   plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 u i:7 with points title " "	
} 



