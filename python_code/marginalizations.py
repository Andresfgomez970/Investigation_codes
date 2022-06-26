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

    def adjust_correct_proportions(self, fig):
        # Adjust correct proportions
        fig.tight_layout()
        fig.subplots_adjust(top=self.opt_params['top_title'])

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

    def add_ith_histogram_to_plot(self, i):
        x_random, p, w = self.consturct_one_dim_histogram(self.data[:, i])
        self.one_dim_axes[i].bar(x_random, p, align='center', width=w,
                                    edgecolor='black', facecolor='blue', alpha=0.5)
        return x_random, p, w


    def add_default_style_to_histogram(self, x_random, w, i):
        # titles
        variable_name = self.obtain_pretty_variable_name(i)
        self.one_dim_axes[i].set_title(variable_name + " Histogram",
                                        fontweight='bold', fontsize=16)

        # labels
        self.one_dim_axes[i].set_xlabel(variable_name, fontsize=14)
        self.one_dim_axes[i].set_ylabel('N(' + variable_name + ')', fontsize=14)

        # limits
        min_v, max_v = self.limits_for_single_file(x_random, w, i)
        self.one_dim_axes[i].set_xlim(min_v, max_v)

    def plot_individual_histograms(self):
        for i in range(self.header['Nvar']):            
            # Plot histogram 
            x_random, _, w = self.add_ith_histogram_to_plot(i)
            ## Pretify graph
            self.add_default_style_to_histogram(x_random, w, i)


    # functions for the different marginalizations
    def one_dimensional_plot(self):
        # Principal title
        plt.suptitle("One Dimensional Marginal Distribution \n for " + self.opt_params['system_name'], 
                     fontsize=20)

        self.plot_individual_histograms()

        self.adjust_correct_proportions(self.one_dim_fig)
        # save image to file
        self.one_dim_fig.savefig(self.opt_params['system_name'] + '_ond_dim_histogram.svg')

class TwoDimensionalAnalizer(BaseMarginalAnalizer):
    def __init__(self, filename, opt_params= {"pretifier_variable_names" : None} ):
        super().__init__(filename)
        self.two_dim_fig, self.two_dim_axes = self.axes_for_margin_of_dim(2)
        self.opt_params = opt_params

    def flatten_xy_values(self, i, j):
        x, y = self.data[:, i : i + 1].copy(), self.data[:, j : j + 1].copy()
        x, y = x.flatten(), y.flatten()
        return x, y

    def add_two_dim_histogram(self, c, i, j):
        x, y = self.flatten_xy_values(i, j)
        h, xed, yed, img = self.two_dim_axes[c].hist2d(x, y, cmap='jet')
        return h, xed, yed, img

    def add_labels(self, variable_name_i, variable_name_j, c):
        # add labels
        self.two_dim_axes[c].set_xlabel(variable_name_i, fontsize=11, labelpad=-8, loc='left')
        self.two_dim_axes[c].set_ylabel(variable_name_j, fontsize=11, labelpad=-8, loc='bottom')

    def add_limits(self, xed, yed, i, j, c):
        min_v_i, max_v_i = self.limits_for_single_file(xed, 0, i)
        min_v_j, max_v_j = self.limits_for_single_file(yed, 0, j)
        self.two_dim_axes[c].set_xlim(min_v_i, max_v_i)
        self.two_dim_axes[c].set_ylim(min_v_j, max_v_j)

    def add_tick_size(self, c):
        plt.setp(self.two_dim_axes[c].get_xticklabels(), fontsize=8)
        plt.setp(self.two_dim_axes[c].get_yticklabels(), fontsize=8)

    def add_bar(self, img, c):
        clb = self.two_dim_fig.colorbar(img, ax=self.two_dim_axes[c])
        clb.ax.tick_params(labelsize=8)

    def add_default_style_to_2d_hist(self, xed, yed, img, c, i, j):
        ## Pretify graph
        # add title
        variable_name_i = self.obtain_pretty_variable_name(i)
        variable_name_j = self.obtain_pretty_variable_name(j)

        if self.opt_params.get('with_title') is True:
            self.two_dim_axes[c].set_title('N(' + variable_name_i + ',' + variable_name_j + ')',
                                           fontweight='bold', fontsize=11)

        # add labels 
        self.add_labels(variable_name_i, variable_name_j, c)

        # limits and sizes of ticks
        self.add_limits(xed, yed, i, j, c)

        # add color bar
        self.add_bar(img, c)

    def two_dimensional_plot(self):
        plt.suptitle("Two Dimensional Marginal Distribution \n for " + self.opt_params['system_name'], 
                fontsize=14, y=0.99)

        c = 0
        for i in range(self.header['Nvar']):
            for j in range(i + 1, self.header['Nvar']):
                # add each histogram
                h, xed, yed, img = self.add_two_dim_histogram(c, i, j)

                # add style to each 2dhistogram
                self.add_default_style_to_2d_hist(xed, yed, img, c, i, j)

                c += 1

        self.adjust_correct_proportions(self.two_dim_fig)

        self.two_dim_fig.savefig(self.opt_params['system_name'] + '_two_dim_histogram.svg')


def one_dimensional_plot(name):
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'},
                 "system_name" : "AM 2229-735",
                 "top_title" : 0.86}
    analyzer = OneDimensionalAnalizer(name, opt_params)
    analyzer.one_dimensional_plot()
    plt.show()

def two_dimensional_plot(name):
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'},
                 "system_name" : "AM 2229-735",
                 "top_title" : 0.92,
                 "with-title" : False}
    analyzer = TwoDimensionalAnalizer(name, opt_params)
    analyzer.two_dimensional_plot()
    plt.show()


def main():
    name = "../Quiroga_system_test1000_2.dat"    
    one_dimensional_plot(name)
    two_dimensional_plot(name)


if __name__ == '__main__':
    main()
