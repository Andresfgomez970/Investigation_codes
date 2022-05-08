# Obtain filename where the data is saved.
filename = sprintf("%s.dat", ARG1)

# Set the style of the plot
set terminal png size 1650,1262 #enhanced font 'Verdana,10' #persist

# Name the output image name for the correlations plot
set output sprintf("%sHistrograms.png", ARG1)

# Possible style; standart is assumed for now
# set style line  1 lc rgb '#0060ad' pt 5 ps 0.2 lt 1 lw 2

# Set layout of the correlation plot
set multiplot layout 3,1 rowsfirst

# Get the number of bytes of the header in the input data
dummy_command = sprintf(" awk '{if(NR==2) print $1}' %s",filename)
number_bytes = system(dummy_command)

widths(n) = word("6 18 10", n)

do for [i=1:3]{
   # Position of x axis in headers
    binwidth = widths(i) / (1000)**(1 / 3.)
    bin(x,width) = width * floor(x / width) + width / 2.0
    set boxwidth binwidth   
    set xrange[-widths(2) / 2. - 1:+widths(2) / 2. + 1]

    plot filename binary skip=number_bytes format="%lf%lf%lf%lf" every 1 using (bin(column(i), binwidth)):(1) smooth freq with boxes title " "
}
