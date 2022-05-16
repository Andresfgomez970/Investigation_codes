import struct as struct
import numpy as np
import matplotlib.pyplot as plt


name = "../data/rmin_sizes/quiroga/rmin_max10.dat"
size = get_size(name)
data_neg = open_file(name, size, npoints, ndim)
