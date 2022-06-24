import struct as struct
import numpy as np
import matplotlib.pyplot as plt
import re
import os
import read_data as rd
import math
from utils import *

class MarginalAnalizer():
    def __init__(self, filename, opt_params= {"pretifier_variable_names" : None} ):
        file = rd.File(filename)
        self.header, self.data = file.get_data()
        # TODO: make this to happen only when things are plotted 
        self.one_dim_axes = self.axes_for_margin_of_dim(1)
        # self.two_dim_axes = self.axes_for_margin_of_dim(2)
        # TODO: implement optional paramaters by file
        self.opt_params = opt_params

    # functions to calculate number of graphs and initialize axes
    def number_plots_for_margin_of_dim(self, dimension):
        return math.comb(self.header['Nvar'], dimension)

    def axes_for_margin_of_dim(self, dim):
        return self.setup_figure_axes(self.number_plots_for_margin_of_dim(dim))
    
    # Function to initialize the figure and axes for a given number of graphs n_total
    def setup_figure_axes(self, n_total, title_is_assigned=1):
        fig = plt.figure(figsize=(20, 10))

        # calculate the number of columns and rows for the subplots
        columns, rows = smaller_divisor_and_remainder(n_total)

        # add all labels defined positions
        axes = []
        for i in range(n_total):
            axes.append(fig.add_subplot(rows, columns, i + 1))

        return axes

    def consturct_one_dim_histogram(self, data):
        n_bins = int(self.header['Npoints'] ** (1 / 3.))
        p, edges = np.histogram(data, n_bins)
        x_random = (edges[1:] + edges[:-1]) / 2
        w = (data.max() - data.min()) / n_bins
        return x_random, p, w

    def obtain_pretty_variable_name(self, i):
        variable_names = np.array(self.header['mapping_variables_names']).flatten()
        if self.opt_params.get('pretifier_variable_names') is None:
            return variable_names[i]
        else:
            return self.opt_params['pretifier_variable_names'][variable_names[i]]

    # functions for the different marginalizations
    def one_dimensional_plot(self):
        plt.suptitle("One dimensional marginal distribution")

        for i in range(self.header['Nvar']):
            # Plot histogram 
            x_random, p, w = self.consturct_one_dim_histogram(self.data[:, i])
            self.one_dim_axes[i].bar(x_random, p, align='center', width=w,
                                     edgecolor='black', facecolor='blue', alpha=0.5)

            ## Pretify graph
            # titles
            self.one_dim_axes[i].set_title(self.obtain_pretty_variable_name(i))

            # limits
            min_v, max_v = self.header['mapping_variables_val0'][0][i], self.header['mapping_variables_valf'][0][i]
            self.one_dim_axes[i].set_xlim(min_v, max_v)


    def two_dimensional_plot(self):
        c = 0
        for i in range(self.header['Nvar']):
            for j in range(i + 1, self.header['Nvar']):
                self.two_dim_axes[c].plot(self.data[:, i : i + 1], self.data[:, j : j + 1], 'o')
                c += 1


def main():
    name = "../Quiroga_system_test1000_2.dat"    
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'}}
    analyzer = MarginalAnalizer(name, opt_params)
    analyzer.one_dimensional_plot()
    plt.show()


if __name__ == '__main__':
    main()
