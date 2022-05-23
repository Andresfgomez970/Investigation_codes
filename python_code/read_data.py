from email.quoprimime import header_check
import struct as struct
import numpy as np
import pandas as pd
from regex import D
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
        self.before_constrain_msg = True
        self.additional_functions_read = 0
        self.dict_of_add_functions = {1 : 'AuxF', 2 : 'ConstrainF'}


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

        n_lines_header = self.lines_of_header()
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
    
    def add_name_and_parameter(self, i_line, lines, header_json):
        name = lines[i_line].replace('-', '').replace('#', '').strip()
        header_json[name] = self.parse_value(lines[i_line + 1])
        return header_json

    def update_before_constain_message(self, i_line, lines, header_json):
        name = lines[i_line].replace('-', '').replace('#', '').strip()
        if name == 'ConstrainMessage':
            self.before_constrain_msg = False

    def name_of_function(self, i_line, lines):
        return lines[i_line][0] == '#' and '-----' in lines[i_line]

    def name_of_variable(self, i_line, lines):
        return lines[i_line][0] == '#' and '--' in lines[i_line]

    def update_mapping_functions(self, i_line, lines, header_json):
        name = lines[i_line].replace('-', '').replace('#', '').strip()            
        if header_json.get('mapping_functions') == None:
            header_json['mapping_functions'] = [name]
        else:
            header_json['mapping_functions'].append(name)
        return header_json
    
    def add_new_mapping_functions_list(self, header_json, list_name):
        if header_json.get(list_name) == None:
            header_json[list_name] = [[]]
        else:
            header_json[list_name].append([])
        return header_json

    def add_to_mapping_function_list(self, i_line, lines, header_json, list_name):
        name = lines[i_line].replace('-', '').replace('#', '').strip()            
        header_json[list_name][-1].append(name)
        return header_json

    def name_of_mapping_parameter(self, i_line, lines):
        return lines[i_line][0] != '#' and '--' in lines[i_line]

    def add_mapping_variable_info(self, i_line, lines, header_json):
        header_json = self.add_to_mapping_function_list(i_line, lines, header_json, 'mapping_variable_names')
        header_json = self.add_to_mapping_function_list(i_line + 1, lines, header_json, 'mapping_variable_val0')
        header_json = self.add_to_mapping_function_list(i_line + 2, lines, header_json, 'mapping_variable_valf')
        return header_json

    def add_mapping_parameter_info(self, i_line, lines, header_json):
        header_json = self.add_to_mapping_function_list(i_line, lines, header_json, 'mapping_parameters_names')
        header_json = self.add_to_mapping_function_list(i_line + 1, lines, header_json, 'mapping_parameters_values')
        return header_json

    def add_single_name(self, i_line, lines, key, header_json):
        name = lines[i_line].replace('-', '').replace('#', '').strip()            
        header_json[key] = name
        return header_json
    
    def read_param_and_value(self, i_line, lines, header_json, name_dict):
        name = lines[i_line].replace('-', '').replace('#', '').strip()            
        param_value = lines[i_line + 1].replace('-', '').replace('#', '').strip()
        if header_json.get(name_dict) == None:
            header_json[name_dict] = {name : param_value}
        else:
            header_json[name_dict][name] = param_value
        return header_json

    def to_json(self):
        header_string = self.read_header()
        # print(header_string)
        header_json = {}
        lines = header_string.split('\n')[:-1]  # -1 to remove last blank character 
        i_line = 0
        while i_line < len(lines):
            if self.name_and_parameter(len(lines), i_line, lines):
                header_json = self.add_name_and_parameter(i_line, lines, header_json)
                self.update_before_constain_message(i_line, lines, header_json)
                i_line += 2

            elif self.name_of_function(i_line, lines) and self.before_constrain_msg:
                header_json = self.update_mapping_functions(i_line, lines, header_json)
                i_line += 1
                # read until the max value of the current function
                self.add_new_mapping_functions_list(header_json, 'mapping_variable_names')
                
                self.add_new_mapping_functions_list(header_json, 'mapping_variable_val0')
                self.add_new_mapping_functions_list(header_json, 'mapping_variable_valf')
                self.add_new_mapping_functions_list(header_json, 'mapping_functions_max_value')

                self.add_new_mapping_functions_list(header_json, 'mapping_parameters_names')
                self.add_new_mapping_functions_list(header_json, 'mapping_parameters_values')

                while i_line < len(lines) and self.parse_value(lines[i_line]) != 'max_value':
                    if self.name_of_variable(i_line, lines):
                        self.add_mapping_variable_info(i_line, lines, header_json)
                        i_line += 3

                    elif self.name_of_mapping_parameter(i_line, lines):
                        header_json = self.add_mapping_parameter_info(i_line, lines, header_json)
                        i_line += 2
                if i_line < len(lines) and self.parse_value(lines[i_line]) == 'max_value':
                    header_json = self.add_to_mapping_function_list(i_line + 1, lines, header_json, 'mapping_functions_max_value')
                    i_line += 2

            elif self.name_of_function(i_line, lines):
                self.additional_functions_read += 1
                func_type = self.dict_of_add_functions[self.additional_functions_read]
                header_json = self.add_single_name(i_line, lines, func_type, header_json)
                i_line += 1
                while i_line < len(lines) and not self.name_of_function(i_line, lines):
                    header_json = self.read_param_and_value(i_line, lines, header_json, func_type + '_param')
                    i_line += 2

        print(header_string)
        print(header_json)

if __name__ == '__main__':
    name = "../data/defaults/DefaultLauraUniform.dat"
    # name = "../data/wetzel/WetzelrminFixedSimple1.dat"
    print(Header(name).to_json())

    # print(read_header(name))
    # print(read_header(name))
    # header, data = get_data(name)
    # print(header)
    # df = pd.DataFrame(data, columns=["r", "f", "e", "w", "i", "W", "d"])
    # print(df)
