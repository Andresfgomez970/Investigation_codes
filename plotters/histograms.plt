filename = sprintf("%s.dat",ARG1)

set terminal png size 1050,762 enhanced font 'Verdana,10' #persist
set output sprintf("%s.png",ARG1)
set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2

set multiplot layout 2,3 rowsfirst

dummy_command = sprintf(" awk '{if(NR==2) print $1}' %s",filename)
number_bytes = system(dummy_command)

dummy_command = sprintf(" awk '{if(NR==10) print $1}' %s",filename)
number_data = system(dummy_command)

n_bins = int (1 + 3.322*log(number_data)/log(10) )
#n_bins = 3 
print(n_bins)
bin(x,width) = width*floor(x*1./width) + width/2.0

do for [i=1:6]{
   line_min = 11 + i*3
   line_max = line_min + 1
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",line_min,filename)
   limit1 = system(dummy_command)
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",line_max,filename)
   limit2 = system(dummy_command)
   binwidth = (limit2-limit1)*1./n_bins
   set xrange[limit1:limit2]
   set boxwidth binwidth
   set ylabel "Histogram"

   axis_x = 10 + i*3
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",axis_x,filename)
   axis_x = system(dummy_command)
   axis_x = axis_x[2:2]	    

   if(i==1){
	set xlabel axis_x
	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using (bin($1,binwidth) ):(1) smooth freq with boxes title " "
   }
   if(i==2){
	set xlabel axis_x
   	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using ( bin($2,binwidth) ):(1) smooth freq with boxes title " "
   }
   if(i==3){
	set xlabel axis_x
    	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using ( bin($3,binwidth) ):(1) smooth freq with boxes title " "
   }
   if(i==4){
	set xlabel axis_x
   	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using ( bin($4,binwidth) ):(1) smooth freq with boxes title " "
   }
   if(i==5){
	set xlabel axis_x
    	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using ( bin($5,binwidth) ):(1) smooth freq with boxes title " "
   }
   if(i==6){
	set xlabel axis_x
    	plot filename binary skip=number_bytes format="%lf%lf%lf%lf%lf%lf%lf" every 1 using ( bin($6,binwidth) ):(1) smooth freq with boxes title " "
   }
   

}