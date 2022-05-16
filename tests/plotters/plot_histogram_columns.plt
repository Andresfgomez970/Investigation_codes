# Obtain filename where the data is saved.
filename = sprintf("%s.dat", ARG1)

# Set the style of the plot
set terminal png size 1650,1262 #enhanced font 'Verdana,10' #persist

# Name the output image name for the correlations plot
set output sprintf("%sHistrograms.png", ARG1)

# Possible style; standart is assumed for now
# set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2

# Get the number of bytes of the header in the input data
dummy_command = sprintf(" awk '{if(NR==2) print $1}' %s",filename)
number_bytes = system(dummy_command)

dummy_command = sprintf(" awk '{if(NR==10) print $1}' %s",filename)
npoints = system(dummy_command)

dummy_command = sprintf(" awk '{if(NR==12) print $1}' %s",filename)
nvar = system(dummy_command)

format_str = ""
do for [i=1:(nvar + 1)]{
    format_str = format_str."%lf"
}

# Set layout of the correlation plot
set multiplot layout nvar, 1 rowsfirst

do for [i=1:nvar]{
    # Position of x axis in headers
    stats filename binary skip=number_bytes format=format_str every 1 using i
    
    range = (STATS_max - STATS_min)
    binwidth = 1.0 / (npoints) ** (1 / 3.)
    bin(x, width) = width * floor(x / width) + width / 2.0
    set boxwidth binwidth
    set xrange[0:1]

    plot filename binary skip=number_bytes format=format_str every 1 using (bin(column(i) / range, binwidth)):(1 / (npoints * binwidth)) smooth freq with boxes title ""
}
