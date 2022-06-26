import sys
from marginalizations import *

class OneDimensionalSymmetryAnalyzer(OneDimensionalPlotter):
    def __init__(self, file, opt_params):
        super().__init__(file, opt_params)


class OneDimTwoBodyProblemAnalyzer(OneDimensionalSymmetryAnalyzer):
    def __init__(self, file, opt_params):
        super().__init__(file, opt_params)

    def obtain_symmetic_indexes(self):
        nu_interval_1 = less_condition(self.data[:, 1], np.pi)
        w_interval_1 = less_condition(self.data[:, 3], np.pi)
        i_interval_1 = less_condition(self.data[:, 4], np.pi / 2)
        return [i_interval_1, w_interval_1, nu_interval_1]

    def obtain_region_names(self):
        region_names = ["0-pi/0-pi/0-pi2", "pi-2pi/0-pi/0-pi2",
                        "pi-2pi/pi-2pi/0-pi2", "0-pi/0-pi/pi2-pi"]
        s_region_names = ["pi-2pi/pi-2pi/pi2-pi", "0-pi/pi-2pi/pi2-pi",
                          "0-pi/0-pi/pi2-pi", "pi-2pi/pi-2pi/0-pi2"]

        return region_names, s_region_names

    def obtain_region_and_s_indeces(self, intervals):
        n_regions = 2 ** 3 // 2  # Considered by pairs
        # Construct the regions
        region_list, s_region_list = [], []
        for i_region in range(n_regions):
            # bools to save each ragion indices and its symmetric correspondence
            region = True
            s_region = True
            # go over all resgions, when region is 1, it corresponds to the 
            #   region that is in invervals. Its symmetric is constructed.
            configurations = '{0:03b}'.format(i_region)

            # Do reversed to count n order and cosntruct the region to be plotted
            for configuration in range(len(configurations)):
                # in any case obtain the region indices and its corresponding symetric region
                if (int(configurations[configuration])):
                    region *= ~intervals[configuration]
                    s_region *= intervals[configuration]
                else:
                    region *= intervals[configuration]
                    s_region *= ~intervals[configuration]
            region_list.append(region)
            s_region_list.append(s_region)

        return region_list, s_region_list

    def plot_complementary_symmetic_region(self, filter, filter_s, 
                                           colors=['r', 'b'], save=True):
        ## Plot regions 0 and 3
        copy_data = self.data.copy()

        # Plot region 1
        self.opt_params['color_histogram'] = colors[0]
        self.data = copy_data[filter, :].copy()
        self.one_dimensional_plot(save=False)

        # Plot region 2
        self.opt_params['color_histogram'] = colors[1]
        self.data = copy_data[filter_s, :].copy()
        self.one_dimensional_plot(save)

        self.data = copy_data.copy()

    def plot_of_iwnu_symmetry(self):
        intervals = self.obtain_symmetic_indexes()
        #region_names, s_region_names = self.obtain_region_names()
        region_list, s_region_list = self.obtain_region_and_s_indeces(intervals)

        system_base_name = self.opt_params['system_name']
        
        # virtual plot for correct regions
        self.opt_params['system_name'] = system_base_name + ' regions 0-1' 
        self.plot_complementary_symmetic_region(region_list[0], s_region_list[3], 
                                                colors=['r', 'b'], save=True)
        plt.show()
    
        self.one_dim_fig, self.one_dim_axes = self.axes_for_margin_of_dim(1)
        self.opt_params['system_name'] = system_base_name + ' regions 3-4' 
        self.plot_complementary_symmetic_region(region_list[1], s_region_list[2], 
                                                colors=['green', 'gray'], save=True)
        plt.show()


# illustrate examples of use for the classes
def one_dimensional_plot(name):
    opt_params= {"pretifier_variable_names" : {'rmin': 'q', 'f': 'ν', 'e': 'e', 'i': 'i', 'w': 'ω', 'W' : 'Ω'},
                 "system_name" : "AM 2229-735",
                 "top_title" : 0.86}
    two_body_analyzer = OneDimTwoBodyProblemAnalyzer(name, opt_params)
    two_body_analyzer.plot_of_iwnu_symmetry()

def two_dimensional_plot(name):
    pass

def main():
    name = "../Quiroga_system_test1000_2.dat"    
    one_dimensional_plot(name)


if __name__ == '__main__':
    main()