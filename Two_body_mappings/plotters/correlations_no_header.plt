# Obtain filename where the data is saved.
filename = sprintf("%s.dat", ARG1)

# Set the style of the plot
set terminal png size 1650,1262 #enhanced font 'Verdana,10' #persist

# Name the output image name for the correlations plot
set output sprintf("%sCorrelations.png",ARG1)

# Set layout of the correlation plot
set multiplot layout 3,5 rowsfirst

# For to plot 1 vs 2, 1 vs 3, ..., 1 vs 6, 2 vs 3, .., 2 vs 6, .., 5 vs 6 in 
#  order to do all possible correlations
do for [i=1:6]{
    do for [j=i+1:6]{
        plot filename binary skip=0 format="%lf%lf%lf%lf%lf%lf%lf" every 1 u i:j with points lc rgb "blue" title ""  
    }
}

