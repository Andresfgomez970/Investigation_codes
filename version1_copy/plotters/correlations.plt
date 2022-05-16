# Obtain filename where the data is saved.
filename = sprintf("%s.dat",ARG1)

# Set the style of the plot
set terminal png size 1650,1262 #enhanced font 'Verdana,10' #persist

# Name the output image name for the correlations plot
set output sprintf("%sCorrelations.png",ARG1)

# Possible style; standart is assumed for now
# set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2

# Set layout of the correlation plot
set multiplot layout 3,5 rowsfirst

# Get the number of bytes of the header in the input data
dummy_command = sprintf(" awk '{if(NR==2) print $1}' %s", filename)
number_bytes = system(dummy_command)

total = -1

number_plus = sprintf(" awk '{if(NR==32) print $15}' %s",filename)
number_plus = system(number_plus)
title_str = sprintf(" Correlations Maps for AM-2229 System %s", number_plus)	    
print (title_str)
#set multiplot layout 3,5 title title_str
set label title_str at screen 0.5,0.99 center front font 'Verdana,18'

do for [i=1:6]{
   # Position of x axis in header
   axis_x = 10 + i*3
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",axis_x,filename)
   axis_x = system(dummy_command)
   axis_x = axis_x[2:2]
   min_x = 11 + i*3
   max_x = min_x + 1
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",min_x,filename)
   min_x = system(dummy_command)*1.
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",max_x,filename)
   max_x = system(dummy_command)*1.
   set xrange[min_x: max_x]
   set xtics min_x,(max_x-min_x)*1./2.,max_x
   do for [j=i+1:6]{
      # Position of y axis in header
      axis_y = 10 + j*3
      dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",axis_y,filename)
      axis_y = system(dummy_command)
      #total =  total + 1
      #set origin total%5*0.2, total/5 * 0.33
      #set size 0.20,0.1
      #set lmargin 10
      #set rmargin 10 
      axis_y = axis_y[2:2]
      min_y = 11 + j*3
      max_y = min_y + 1
      dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",min_y,filename)
      min_y = system(dummy_command)*1.
      dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",max_y,filename)
      max_y = system(dummy_command)*1.
      set yrange[min_y: max_y]
      set xlabel axis_x
      set ylabel axis_y
      plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 u i:j with points lc rgb "blue" title ""  
   }
} 





