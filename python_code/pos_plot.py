import struct as struct
import numpy as np
import matplotlib.pyplot as plt

N_BYTES_HEADER = 3121
npoints = 10000
ndim = 7

def get_size(name):
	with open(name, "rb") as binary_file:
    	# Read the header to skipt it
		couple_bytes = binary_file.read(N_BYTES_HEADER)
		#print(couple_bytes)
	    #print(data)
		couple_bytes = binary_file.read(npoints*ndim*8)
	return len(couple_bytes) 

def open_file(name, size, npoints, ndim): 
	with open(name, "rb") as binary_file:
    	# Read the header to skipt it
		couple_bytes = binary_file.read(N_BYTES_HEADER)
		couple_bytes = binary_file.read(size)
		if (len(couple_bytes) != npoints * ndim * 8):
			print("Expected size: %d, but input size: %d" %
				  (npoints * ndim * 8, size))
			npoints = size // (ndim * 8)
		#data = np.array ( struct.unpack('d'*npoints*ndim, couple_bytes) ).reshape(npoints,ndim)
		data = np.array(struct.unpack('d'*npoints*ndim, couple_bytes)).reshape(npoints,ndim)
	return data

def print_sizes():
	sizes =  [1, 5, 10, 20, 50, 100, 500, 2000, 4000]
	base_name = "rmin_max"
	directory = "../data/rmin_sizes/quiroga/"
	for s in sizes:
		name = directory + base_name + str(s) + ".dat"
		size_file = get_size(name)
		print(s, size_file, npoints * ndim * 8)

print_sizes()
name = "../data/rmin_sizes/quiroga/rmin_max10.dat"
size = get_size(name)
data_neg = open_file(name, size, npoints, ndim)

""" name = "AM_2229_735SystemPlusX2Y0VZ0UniformConstrainedMontecarloMap1000Points.dat"
data_0 = open_file(name)

name = "AM_2229_735SystemPlusX19Y0VZ0UniformConstrainedMontecarloMap1000Points.dat"
data_pos = open_file(name)
 """


names = ["r","f","e","w","i","W"]
fig = plt.figure(figsize=(40,20))
plt.suptitle("Correlation maps (black+=0kpc, green-=19kpc, blue+=20kpc)", fontsize=20)
c = 0 
for i in range(6):
	for j in range(i+1,6):
		c +=1
		ax = fig.add_subplot(3, 5, c)
		# ax.plot(data_0[:,i:i+1],data_0[:,j:j+1], "k.",ms=1,label=names[j]+"vs"+names[i])
		# ax.plot(data_pos[:,i:i+1],data_pos[:,j:j+1], "b.",ms=1)
		ax.plot(data_neg[:,i:i+1],data_neg[:,j:j+1], "g.",ms = 1, label=names[j]+"vs"+names[i])
		ax.legend( loc="best" )

plt.savefig("correlations.png")	    
plt.show()
