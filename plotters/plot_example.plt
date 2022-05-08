filename = sprintf("%s.dat",ARG1)  #name of the file to be graph

set terminal png size 1850,962 enhanced font 'Verdana,10'
set output sprintf("%s.png",ARG1)
print(sprintf("%s.png",ARG1))
set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2    # blue

#set xrange[-16:16]
#set yrange[-16:16]

# set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2 pointtype 7 pointsize 1.5

plot filename u 1:2 w p #with linespoints linestyle 1 title filename