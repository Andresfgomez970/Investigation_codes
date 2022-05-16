filename = sprintf("%s.dat",ARG1)  #name of the file to be graph

set terminal png size 1850,962 enhanced font 'Verdana,10' 
set output sprintf("%s1Dplot.png", ARG1)
set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2    # blue
set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 1.5

#f(x) = a*exp(-(b-x)*(b-x)*c)
#a=1
#b=0.1
#c=0.5
#fit f(x) filename binary format="%lf%lf" every 1 u 1:2 via a,b, c

dummy_command = sprintf("awk '{if(NR==2) print $1}' %s", filename)
number_bytes = system(dummy_command)

plot filename binary skip=number_bytes format="%lf%lf" u 1:2  with points title filename