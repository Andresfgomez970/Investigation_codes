filename = ARG1  #name of the file to be graph

n_data = ARG2
n_bins = int (1 + 3.322*log(n_data)/log(10) )
width = 6./n_bins
bin(x,width) = width * floor(x * 1. / width) + width / 2.0
plot ARG1 u ( bin($1,width) ):(1) smooth freq with boxes title " "