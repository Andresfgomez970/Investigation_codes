list_names = "../data/mappings/list3"

dummy_command = sprintf(" wc %s | awk '{if(NR==%d) print $1}'",list_names,1)
nfiles = system(dummy_command)

do for[i=1:nfiles]{
   dummy_command = sprintf(" awk '{if(NR==%d) print $1}' %s",i,list_names)
   name = system(dummy_command)
   name = name[:strlen(name)-4] 
   dummy_command = sprintf("gnuplot -c correlations.plt ../data/mappings/%s ",name) 
   #print (dummy_command)
   system(dummy_command)
}  





