import struct as struct
import numpy as np
import pandas as pd

DOUBLE_SIZE = 8

###############################################################################
# input output functions
###############################################################################
def get_data(name):
    """
    This function will read the default binary format saved by the MontecarloStrcuturesConstrainedSampleGetNPointsFromFile
        function. In short: the header specifies the conditions in which the sample was taken. After that, the values for
        the orbital parameters accompanied by the distance to the observables ["r", "f", "e", "w", "i", "W", "d"] are 
        written continuously. 
    """
    # read header param and real size
    with open(name, "rb") as binary_file:
        # Read the header to skipt it
        header = ''
        n_lines = 1
        while(n_lines < 13):
            c = binary_file.read(1)
            if (c == b'\n'):
                n_lines += 1
            if (n_lines == 2):
                n_bytes_header = int(binary_file.read(10))
            elif (n_lines == 10):
                npoints = int(binary_file.read(20))
            elif(n_lines == 12):
                ndim = int(binary_file.read(10)) + 1

    with open(name, "rb") as binary_file:
        # Read the header to skipt it
        couple_bytes = binary_file.read(n_bytes_header)
        # Try to read npoints * ndim * DOUBLE_SIZE bytes
        size = len(binary_file.read(npoints * ndim * DOUBLE_SIZE))

    with open(name, "rb") as binary_file:
        # Read the header to skipt it
        couple_bytes = binary_file.read(n_bytes_header)
        couple_bytes = binary_file.read(size)
        if (len(couple_bytes) != npoints * ndim * 8):
            print("Expected size: %d, but input size: %d in %s" %
                  (npoints * ndim * 8, size, name))
            npoints = size // (ndim * 8)
        data = np.array(struct.unpack('d' * npoints * ndim,
                                      couple_bytes)).reshape(npoints, ndim)
    return header, data

class Header:
    def __init__(self, filename):
        self.filename = filename
        self.binary_file = self.read_file()

    def read_file(self):
        return open(self.filename, "rb")

    def next_character(self):
        pass

    def lines_of_header(self):
        actual_line, n_lines = 1, 5
        while(actual_line < n_lines):
            c = self.binary_file.read(1)
            if c == b'\n': actual_line += 1 
            if actual_line == 4:
                n_lines = int(self.binary_file.read(10))
                self.binary_file = self.read_file()
                return n_lines




    def read_header(self):
        header, actual_line = '', 0
        n_lines = self.lines_of_header() - 2
        while actual_line < n_lines:
            c = self.binary_file.read(1)
            header += c.decode('UTF-8')
            if c == '\n': print(c == '\n', c)
            if c == b'\n': actual_line += 1
        return header



if __name__ == '__main__':
    name = "../data/defaults/DefaultLauraUniform.dat"

    print(Header(name).read_header())
    # print(read_header(name))
    # print(read_header(name))
    # header, data = get_data(name)
    # print(header)
    # df = pd.DataFrame(data, columns=["r", "f", "e", "w", "i", "W", "d"])
    # print(df)
