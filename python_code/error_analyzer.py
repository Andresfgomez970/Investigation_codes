from read_data import *
from marginalizations import *

class TwoDimTwoBodyProblemAnalyzer(TwoDimensionalPlotter):
    def __init__(self, filesnames, opt_params):
        self.filesnames = filesnames
        super().__init__(filesnames[0], opt_params)

    def plot_both_correalations(self):
        # first plot 
        self.two_dimensional_correlation_plot(save=False)

        # Second plot
        self.header, self.data = File(self.filesnames[1]).get_data()
        self.opt_params['color'] = 'r'
        self.two_dimensional_correlation_plot(save=False)


def error_plots(variable, files):    
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'},
                    "system_name" : "AM 2229-735 \n for " + variable + " bias of 50% and -50%",
                    "top_title" : 0.92,
                    "with-title" : False,
                    "ms" : 0.1 
                }
    TwoDimTwoBodyProblemAnalyzer(files, opt_params).plot_both_correalations()
    plt.show()

def all_error_plots():
    # in x
    files = ["../Quiroga_system_test1000_2.dat", "../Quiroga_system_test1000_2.dat"]    
    error_plots('x', files)

    # in y 

    # in vz


def main():
    all_error_plots()

if __name__ == '__main__':
    main()