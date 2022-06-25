import struct as struct
import numpy as np
import matplotlib.pyplot as plt
import re
import os
import read_data as rd
import math
from utils import *
class BaseMarginalAnalizer():
    def __init__(self, filename):
        file = rd.File(filename)
        self.header, self.data = file.get_data()

    # functions to calculate number of graphs and initialize axes
    def number_plots_for_margin_of_dim(self, dimension):
        return math.comb(self.header['Nvar'], dimension)

    def axes_for_margin_of_dim(self, dim):
        return self.setup_figure_axes(self.number_plots_for_margin_of_dim(dim))
    
    # Function to initialize the figure and axes for a given number of graphs n_total
    def setup_figure_axes(self, n_total, title_is_assigned=1):
        # calculate the number of columns and rows for the subplots
        columns, rows = smaller_divisor_and_remainder(n_total)

        max_height = 9.8

        fig = plt.figure(figsize=(max_height * columns / rows, max_height))

        # add all labels defined positions
        axes = []
        for i in range(n_total):
            axes.append(fig.add_subplot(rows, columns, i + 1))

        return fig, axes

    def obtain_pretty_variable_name(self, i):
        variable_names = np.array(self.header['mapping_variables_names']).flatten()
        if self.opt_params.get('pretifier_variable_names') is None:
            return variable_names[i]
        else:
            return self.opt_params['pretifier_variable_names'][variable_names[i]]

    def limits_for_single_file(self, x_values, width, i):
        min_v, max_v = self.header['mapping_variables_val0'][0][i], self.header['mapping_variables_valf'][0][i]
        min_v = max([float(min_v), (x_values - width / 2).min()])
        max_v = min([float(max_v), (x_values + width / 2).max()])  
        return min_v, max_v

class OneDimensionalAnalizer(BaseMarginalAnalizer):
    def __init__(self, filename, opt_params= {"pretifier_variable_names" : None} ):
        super().__init__(filename)
        self.one_dim_fig, self.one_dim_axes = self.axes_for_margin_of_dim(1)
        self.opt_params = opt_params

    def consturct_one_dim_histogram(self, data):
        n_bins = int(self.header['Npoints'] ** (1 / 3.))
        p, edges = np.histogram(data, n_bins)
        x_random = (edges[1:] + edges[:-1]) / 2
        w = (data.max() - data.min()) / n_bins
        return x_random, p, w

    # functions for the different marginalizations
    def one_dimensional_plot(self):
        plt.suptitle("One dimensional marginal distribution \n for " + self.opt_params['system_name'], 
                     fontsize=20)


        for i in range(self.header['Nvar']):            
            # Plot histogram 
            x_random, p, w = self.consturct_one_dim_histogram(self.data[:, i])
            self.one_dim_axes[i].bar(x_random, p, align='center', width=w,
                                     edgecolor='black', facecolor='blue', alpha=0.5)

            ## Pretify graph
            # titles
            variable_name = self.obtain_pretty_variable_name(i)
            self.one_dim_axes[i].set_title(variable_name + " Histogram",
                                           fontweight='bold')

            # labels
            self.one_dim_axes[i].set_xlabel(variable_name)
            self.one_dim_axes[i].set_ylabel('N(' + variable_name + ')')

            # limits
            min_v, max_v = self.limits_for_single_file(x_random, w, i)
            self.one_dim_axes[i].set_xlim(min_v, max_v)

        self.one_dim_fig.tight_layout()
        self.one_dim_fig.subplots_adjust(top=0.86)

        # save image to file
        self.one_dim_fig.savefig(self.opt_params['system_name'] + '_ond_dim_histogram.png')



class TwoDimensionalAnalizer(BaseMarginalAnalizer):
    def __init__(self, filename, opt_params= {"pretifier_variable_names" : None} ):
        super().__init__(filename)
        self.two_dim_fig, self.two_dim_axes = self.axes_for_margin_of_dim(2)
        self.opt_params = opt_params


    def two_dimensional_plot(self):
        c = 0
        for i in range(self.header['Nvar']):
            for j in range(i + 1, self.header['Nvar']):
                self.two_dim_axes[c].plot(self.data[:, i : i + 1], self.data[:, j : j + 1], 'o')
                c += 1


def main():
    name = "../Quiroga_system_test1000_2.dat"    
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'},
                 "system_name" : "AM 2229-735"}
    analyzer = OneDimensionalAnalizer(name, opt_params)
    analyzer.one_dimensional_plot()
    plt.show()

if __name__ == '__main__':
    main()
