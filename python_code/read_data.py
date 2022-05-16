from email.quoprimime import header_check
import struct as struct
import numpy as np
import pandas as pd
from utils import *

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
        # set up file from the beginning again
        self.binary_file = self.read_file()

        actual_line, lines_info_line = 1, 4
        while actual_line <= lines_info_line:
            c = self.binary_file.read(1) 
            if c == b'\n': actual_line += 1
            if actual_line == 4:
                n_lines = int(self.binary_file.read(10))
                return n_lines

    def read_header(self):
        header, actual_line = '', 0

        n_lines_header = 51 # self.lines_of_header()
        # set up file from the beginning again
        self.binary_file = self.read_file()

        while actual_line < n_lines_header:
            c = self.binary_file.read(1)
            if c == b'\n': actual_line += 1
            header += c.decode('UTF-8')
        return header

    def parse_value(self, value):
        if is_int(value):
            return int(value)
        elif is_float(value):
            return float(value)
        else:
            return value.replace('\r', '').strip()

    def name_and_parameter(self, n_lines, i_line, lines):
        return lines[i_line][0] == '#' and ((i_line < n_lines - 2 and lines[i_line + 2][0] == '#') or i_line == n_lines - 2) and '-----' in lines[i_line]
    
    def name_of_function(self, i_line, lines):
        return lines[i_line][0] == '#' and '-----' in lines[i_line]

    def to_json(self):
        header_string = self.read_header()
        # print(header_string)
        header_json = {}
        lines = header_string.split('\n')[:-1]  # -1 to remove last blank character 
        i_line = 0
        while i_line < len(lines):
            if self.name_and_parameter(len(lines), i_line, lines):
                name = lines[i_line].replace('-', '').replace('#', '').strip()
                header_json[name] = self.parse_value(lines[i_line + 1])
                print(lines[i_line])
                print(lines[i_line + 1])
                i_line += 2

            elif self.name_of_function(i_line, lines):
                name = lines[i_line].replace('-', '').replace('#', '').strip()
                print(lines[i_line])
                i_line += 1
                while i_line < len(lines) and self.parse_value(lines[i_line]) != 'max_value' and not self.name_of_function(i_line, lines):
                    print(lines[i_line])
                    i_line += 1
                if i_line < len(lines) and self.parse_value(lines[i_line]) == 'max_value':
                    print(lines[i_line])
                    print(lines[i_line + 1])
                    i_line += 2


            else:
                i_line += 1
            # elif lines[i_line][0] == '#':
            #     # create dictionary with name of function
            #     name = lines[i_line].replace('-', '').replace('#', '').strip()
            #     header_json[name] = {}
            #     # add all its ranges and parameters
            #     in_function_reading = True
            #     local_actual_line = i_line
            #     while in_function_reading:
            #         local_actual_line += 1
            #         if local_actual_line >= len(lines) or lines[local_actual_line] == '#': in_function_reading = False
                    
            #         if self.name_with_range(len(lines), local_actual_line, lines):
            #             while local_actual_line < len(lines) or lines[local_actual_line] != '#': 
            #                 if header_json[name] == {}:   
            #                     param_name = lines[local_actual_line].replace('-', '').replace('#', '').strip()
            #                     header_json[name]['param_names'] = [param_name]
            #                     header_json[name]['val_0'] = [self.parse_value(lines[local_actual_line + 1])]
            #                     header_json[name]['val_f'] = [self.parse_value(lines[local_actual_line + 2])]
            #                 else:
            #                     param_name = lines[local_actual_line].replace('-', '').replace('#', '').strip()
            #                     header_json[name]['param_names'].append(param_name)
            #                     header_json[name]['val_0'].append(self.parse_value(lines[local_actual_line + 1]))
            #                     header_json[name]['val_f'].append(self.parse_value(lines[local_actual_line + 2]))
            #                 local_actual_line += 2
                    
                    
                    
            #         elif self.param_with_value(len(lines), local_actual_line, lines):
            #             if header_json[name] == {}:   
            #                 param_name = lines[local_actual_line].replace('-', '').replace('#', '').strip()
            #                 header_json[name]['param_names'] = [param_name]
            #                 header_json[name]['values'] = [self.parse_value(lines[local_actual_line + 1])]
            #             else:
            #                 param_name = lines[local_actual_line].replace('-', '').replace('#', '').strip()
            #                 header_json[name]['param_names'].append(param_name)
            #                 header_json[name]['values'].append(self.parse_value(lines[local_actual_line + 1]))
            #             local_actual_line += 1
            #         else:
            #             add_info = lines[local_actual_line].replace('-', '').replace('#', '').strip()
            #             header_json[name][add_info] = self.parse_value(lines[local_actual_line + 1])


        # print(header_string)
        # print(header_json)

if __name__ == '__main__':
    name = "../data/wetzel/WetzelrminFixedSimple1.dat"
    print(Header(name).to_json())

    # print(read_header(name))
    # print(read_header(name))
    # header, data = get_data(name)
    # print(header)
    # df = pd.DataFrame(data, columns=["r", "f", "e", "w", "i", "W", "d"])
    # print(df)
