import struct as struct
import numpy as np
import matplotlib.pyplot as plt
import re
import os

DOUBLE_SIZE = 8

###############################################################################
# General purpose functions
###############################################################################
def linear(x, *param):
    return param[0] + param[1] * x


###############################################################################
# input output functions
###############################################################################
def get_data(name):
    # read header param and real size
    with open(name, "rb") as binary_file:
        # Read the header to skipt it
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
    return data


###############################################################################
# basic plot functions
###############################################################################
def plot_correlation_map_from_data(data, plot_params=None):
    # Initialize defaults and do not change other variables already assgined
    plot_params_default = {"color": ".b", "ms": 1, "alpha": 0.2}
    plot_params_default["variables_names"] = ["r", "f", "e", "w", "i", "W"]
    plot_params_default["ms"] = 0.2
    plot_params_default["filename"] = "correalations_default.png"
    plot_params_default["plot"] = 0
    plot_params_default["limit_axes"] = 0
    plot_params_default["val_0"] = [0, 0, 0.3, 0, 0, 0]
    plot_params_default["val_f"] = [100, 2 * np.pi, 5, 2 * np.pi, np.pi,
                                    2 * np.pi]

    if (plot_params is None):
        plot_params = plot_params_default
    else:
        if (plot_params.get("axes") is None):
            plot_params["axes"] = setup_figure_axes()

        for key in plot_params_default.keys():
            if key not in plot_params:
                plot_params[key] = plot_params_default[key]

    names = plot_params["variables_names"]
    axes = plot_params["axes"]
    color = plot_params["color"]
    alpha = plot_params["alpha"]
    ms = plot_params["ms"]
    val_0 = plot_params_default["val_0"]
    val_f = plot_params_default["val_f"]

    plt.suptitle("Orbital Elements Correlation Maps", fontsize=20)
    c = 0
    for i in range(6):
        for j in range(i + 1, 6):
            if (not plot_correlation_map_from_data.legend_state):
                axes[c].plot(data[:, i:i + 1], data[:, j:j + 1], color, ms=ms,
                             alpha=alpha)
                axes[c].set_title(names[j] + " vs " + names[i], pad=0,
                                  fontweight='bold', fontsize=10)
            else:
                axes[c].set_title(names[j] + " vs " + names[i], pad=0,
                                  fontweight='bold', fontsize=10)
                axes[c].plot(data[:, i:i + 1], data[:, j:j + 1], color, ms=ms,
                             label=names[j] + "vs" + names[i], alpha=alpha)

            if (plot_params_default["limit_axes"]):
                axes[c].set_xlim(val_0[i], val_f[i])
                axes[c].set_ylim(val_0[j], val_f[j])
            c += 1

    if (c == 15):
        plot_correlation_map_from_data.legend_state = 0


def setup_figure_axes(legend_state=1):
    fig = plt.figure(figsize=(20, 10))
    plot_correlation_map_from_data.legend_state = legend_state

    axes = []
    c = 0
    for i in range(6):
        for j in range(i + 1, 6):
            c += 1
            axes.append(fig.add_subplot(3, 5, c))

    return axes


def plot_correlation_map_from_file(name, plot_params=None):
    data = get_data(name)

    plot_params_default = {}
    plot_params_default["filename"] = "correlations_default.png"
    plot_params_default["plot"] = 0

    if (plot_params is None):
        plot_params = plot_params_default
    else:
        for key in plot_params_default.keys():
            if key not in plot_params:
                plot_params[key] = plot_params_default[key]

    plot_correlation_map_from_data(data, plot_params)

    if (plot_params["plot"]):
        plt.savefig(plot_params["filename"])
        plt.show()


def correlation_map_example():
    name = "../data/rmin_sizes/quiroga/quiroga/rmin_max20.dat"
    plot_correlation_map_from_file(name, {'plot': 1, 'ms': 1,
                                          "axes": setup_figure_axes(0)})


###############################################################################
# region aplication function
###############################################################################
def range_condition(data, index, bounds):
    con1 = data[:, index:index + 1] > bounds[0]
    con2 = data[:, index:index + 1] < bounds[1]

    cond = con1 * con2
    cond = cond.reshape(cond.shape[0])

    return data[cond, :]


def region_condition(data, region_data):
    index = region_data["index"]
    param = region_data["param"]
    function = region_data["function"]
    less_than_cond = region_data["less_than_cond"]

    x, y = data[:, index[0]:index[0] + 1], data[:, index[1]:index[1] + 1]
    line = function(x, *param)
    con1 = y <= line
    con1 = con1.reshape(con1.shape[0])

    if (less_than_cond):
        return data[con1, :]
    else:
        return data[~con1, :]


###############################################################################
# intended functions to analyze: simple divisions of nu
###############################################################################
def nu_symmetry_2regions(data, plot_params={}):
    """Basic separation of strcutures in correlation maps"""
    plot_params_default = {}
    plot_params_default["filename"] = "correalations_2regionsf_default.png"
    plot_params_default["axes"] = setup_figure_axes(0)

    if (plot_params is None):
        plot_params = plot_params_default
    else:
        for key in plot_params_default.keys():
            if key not in plot_params:
                plot_params[key] = plot_params_default[key]

    # f between 0 and pi
    bounds1 = [0, np.pi]
    data1 = range_condition(data, 1, bounds1)
    plot_params["color"] = "g."
    plot_params["alpha"] = 1
    plot_correlation_map_from_data(data1, plot_params)

    # f between pi and 2*pi
    bounds2 = [np.pi, 2 * np.pi]
    data2 = range_condition(data, 1, bounds2)
    plot_params["color"] = "b."
    plot_params["alpha"] = 0.2
    plot_correlation_map_from_data(data2, plot_params)

    plt.savefig(plot_params['filename'])
    plt.show()


def nu_symmetry_3regions(data, plot_params={}):
    plot_params_default = {}
    plot_params_default["filename"] = "correalations_3regionsf_default.png"
    plot_params_default["axes"] = setup_figure_axes(0)
    plot_params_default["alpha"] = 0.2
    plot_params_default["limit_axes"] = 1

    if (plot_params is None):
        plot_params = plot_params_default
    else:
        for key in plot_params_default.keys():
            if key not in plot_params:
                plot_params[key] = plot_params_default[key]

    # f between 0 and pi
    bounds1 = [0, 2 * np.pi / 3.]
    data1 = range_condition(data, 1, bounds1)
    plot_params["color"] = "g."
    plot_params["alpha"] = plot_params["alpha"]
    plot_correlation_map_from_data(data1, plot_params)

    # f between pi and 2*pi
    bounds2 = [2 * np.pi / 3., 4 * np.pi / 3.]
    data2 = range_condition(data, 1, bounds2)

    plot_params["color"] = "b."
    plot_params["alpha"] = plot_params["alpha"]
    plot_correlation_map_from_data(data2, plot_params)

    bounds3 = [4 * np.pi / 3., 2 * np.pi]
    data3 = range_condition(data, 1, bounds3)
    plot_params["color"] = "r."
    plot_params["alpha"] = plot_params["alpha"]

    plot_correlation_map_from_data(data3, plot_params)

    plt.savefig(plot_params['filename'])
    # plt.show()


def four_regions_from_nu(data):
    bounds = [[0, np.pi / 2.], [np.pi / 2., np.pi], [np.pi, 3 * np.pi / 2.],
              [3 * np.pi / 2., 2 * np.pi]]
    color = ['g.', 'r.', 'k.', 'b.']
    for i in range(len(bounds)):
        data_p = range_condition(data, 1, bounds[i])
        plt.plot(data_p[:, 1:2], data_p[:, 5:6], color[i], ms=1)

    plt.savefig("four_regions_from_nu.png")
    plt.show()


def nu_symmetry_single_plots(data):
    """Single plots: intended to see symmetries"""
    names = ["r", "f", "e", "w", "i", "W"]

    # f between 0 and pi
    bounds1 = [0, np.pi]
    data1 = range_condition(data, 1, bounds1)

    # f between pi and 2 * pi
    bounds2 = [np.pi, 2 * np.pi]
    data2 = range_condition(data, 1, bounds2)
    colors = ['g.', 'b.']
    # data2[:, 1] -= 2 * np.pi
    # data2[:, 3] += 2 * np.pi

    val_0 = [0, 0, 0.3, 0, 0, 0]
    val_f = [100, 2 * np.pi, 5, 4 * np.pi, np.pi, 2 * np.pi]

    for i in range(6):
        for j in range(i + 1, 6):
            fig, axs = plt.subplots(1, 3, figsize=(15, 7))
            axs[0].set_title("0-pi range")
            axs[0].plot(data1[:, i:i + 1], data1[:, j:j + 1], colors[0], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[0].legend(loc="best")
            axs[0].set_xlim(val_0[i], val_f[i])
            axs[0].set_ylim(val_0[j], val_f[j])

            axs[1].set_title("pi-2pi range")
            axs[1].plot(data2[:, i:i + 1], data2[:, j:j + 1], colors[1], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[1].legend(loc="best")
            axs[1].set_xlim(val_0[i], val_f[i])
            axs[1].set_ylim(val_0[j], val_f[j])

            axs[2].set_title("All range")
            axs[2].plot(data1[:, i:i + 1], data1[:, j:j + 1], colors[0], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[2].plot(data2[:, i:i + 1], data2[:, j:j + 1], colors[1], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[2].legend(loc="best")
            axs[2].set_xlim(val_0[i], val_f[i])
            axs[2].set_ylim(val_0[j], val_f[j])

            plt.savefig("%sVS%srmin_max100" % (names[j], names[i]))
            plt.show()


def nu_symmetry_single_plots_regions(name):
    data = get_data(name)
    """Single plots: intended to see symmetries"""
    names = ["r", "f", "e", "w", "i", "W"]

    # regions 1
    bounds1 = [0, 2 * np.pi / 3]
    data1 = range_condition(data, 1, bounds1)

    # region 2
    bounds2 = [2 * np.pi / 3., 4 * np.pi / 3.]
    data2 = range_condition(data, 1, bounds2)
    colors = ['g.', 'b.', 'r.']

    # region 3
    bounds3 = [4 * np.pi / 3., 6 * np.pi / 3.]
    data3 = range_condition(data, 1, bounds3)

    val_0 = [0, 0, 0.3, 0, 0, 0]
    val_f = [100, 2 * np.pi, 5, 2 * np.pi, np.pi, 2 * np.pi]

    for i in range(6):
        for j in range(i + 1, 6):
            fig, axs = plt.subplots(1, 4, figsize=(15, 7))
            axs[0].set_title("region 1")
            axs[0].plot(data1[:, i:i + 1], data1[:, j:j + 1], colors[0], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[0].legend(loc="best")
            axs[0].set_xlim(val_0[i], val_f[i])
            axs[0].set_ylim(val_0[j], val_f[j])

            axs[1].set_title("region 2")
            axs[1].plot(data2[:, i:i + 1], data2[:, j:j + 1], colors[1], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[1].legend(loc="best")
            axs[1].set_xlim(val_0[i], val_f[i])
            axs[1].set_ylim(val_0[j], val_f[j])

            axs[2].set_title("region 3")
            axs[2].plot(data3[:, i:i + 1], data3[:, j:j + 1], colors[2], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[2].legend(loc="best")
            axs[2].set_xlim(val_0[i], val_f[i])
            axs[2].set_ylim(val_0[j], val_f[j])

            axs[3].set_title("All")
            axs[3].plot(data1[:, i:i + 1], data1[:, j:j + 1], colors[0], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[3].plot(data2[:, i:i + 1], data2[:, j:j + 1], colors[1], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            axs[3].plot(data3[:, i:i + 1], data3[:, j:j + 1], colors[2], ms=3,
                        label=names[j] + "vs" + names[i], alpha=0.2)
            # axs[3].legend(loc="best")
            axs[3].set_xlim(val_0[i], val_f[i])
            axs[3].set_ylim(val_0[j], val_f[j])

            plt.savefig("%sVS%srmin_max100" % (names[j], names[i]))
            plt.show()


###############################################################################
# Other separations for nu an plot for differents sizes
###############################################################################
def refine_second_separation(data):
    """ This is a fail when considered until 100 """
    m = (2.4 - 2.2) / (12.2 - 8.5)
    b = np.pi / 2.
    bounds3 = [0, np.pi]
    region_data = {"function": linear, "param": (b, m), "index": [0, 1],
                   "less_than_cond": True}
    data3 = range_condition(data, 1, bounds3)
    data3 = region_condition(data, region_data)
    plt.plot(data3[:, 0:1], data3[:, 1:2], "g.", ms=1)

    data4 = range_condition(data, 1, bounds3)
    region_data["less_than_cond"] = False
    data4 = region_condition(data4, region_data)
    plt.plot(data4[:, 0:1], data4[:, 1:2], "r.", ms=1)
    plt.plot(data[:, 0:1], data[:, 1:2], "b.", ms=1, alpha=0.1)

    x = np.linspace(data4[:, 0:1].min(), data4[:, 0:1].max())
    plt.plot(x, region_data['function'](x, *region_data["param"]))

    plt.show()


def correlations_different_sizes():
    # General information of the data
    path = "../data/rmin_sizes/quiroga/quiroga/"
    sizes = [1, 5, 10, 20, 50, 100, 500, 2000, 4000]
    plot = 1

    for i in range(len(sizes)):
        name = path + "rmin_max%s.dat" % (sizes[i])
        base_name = re.findall(r'/(\w+)', name)[-1]
        info_input = {"base_name": base_name}

        data = get_data(name)
        four_regions_from_nu_correlations(data, info_input, plot)


def four_regions_from_nu_correlations(data, info_input, plot):
    base_name = info_input["base_name"]

    if (plot == 1):
        names = ["r", "f", "e", "w", "i", "W"]
        axes = setup_figure_axes()
        plot_params = {"variables_names": names, "axes": axes}

    data_list = []

    bounds = [[0, np.pi / 2.], [np.pi / 2., np.pi], [np.pi, 3 * np.pi / 2.],
              [3 * np.pi / 2., 2 * np.pi]]
    color = ['k.', 'b.', 'r.', 'g.']

    # Construct and plot all separation together
    for i in range(len(bounds)):
        data_p = range_condition(data, 1, bounds[i])
        data_list.append(data_p)
        if (plot == 1):
            plot_params["color"] = color[i]
            plot_params["alpha"] = 0.2
            plot_correlation_map_from_data(data_p, plot_params)

    if (plot):
        plt.savefig("correlations_%s.png" % (base_name))
        # plt.show()

    # Construct and plot symmetry separations
    if (plot == 1):
        names = ["r", "f", "e", "w", "i", "W"]
        axes = setup_figure_axes()
        plot_params = {"variables_names": names, "axes": axes}

    for i in [0, 3]:
        if (plot == 1):
            plot_params["color"] = color[i]
            plot_params["alpha"] = 0.2
            plot_correlation_map_from_data(data_list[i], plot_params)

    if (plot):
        plt.savefig("correlations_sym(0,pi2)_%s.png" % (base_name))
        # plt.show()

    # Construct and plot symmetry separations
    if (plot == 1):
        names = ["r", "f", "e", "w", "i", "W"]
        axes = setup_figure_axes()
        plot_params = {"variables_names": names, "axes": axes}

    for i in [1, 2]:
        if (plot == 1):
            plot_params["color"] = color[i]
            plot_params["alpha"] = 0.2
            plot_correlation_map_from_data(data_list[i], plot_params)

    if (plot):
        plt.savefig("correlations_sym(pi2,pi)_%s.png" % (base_name))
        # plt.show()

    return data_list


def four_regions_division_in_progress(name):
    data = get_data(name)

    #####################################################
    # Analysize different intervals

    # # Define general plot parameters
    plot = 1
    if (plot == 1):
        names = ["r", "f", "e", "w", "i", "W"]
        axes = setup_figure_axes()
        plot_params = {"variables_names": names, "axes": axes}

    # #######################
    # Plot particular cases of f between 0 and pi
    m = (2.4 - 2.2) / (12.2 - 8.5)
    b = np.pi / 2.
    bounds3 = [0, np.pi]
    region_data = {"function": linear, "param": (b, m), "index": [0, 1],
                   "less_than_cond": True}

    # Upper region of f between 0 and pi
    data3 = range_condition(data, 1, bounds3)
    data3 = region_condition(data, region_data)
    if (plot == 1):
        plot_params["color"] = "k."
        plot_params["alpha"] = 0.2
        plot_correlation_map_from_data(data3, plot_params)

    # Down region of f between pi and 2pi
    data4 = range_condition(data, 1, bounds3)
    region_data["less_than_cond"] = False
    data4 = region_condition(data4, region_data)
    if (plot == 1):
        plot_params["color"] = "b."
        plot_params["alpha"] = 0.2
        plot_correlation_map_from_data(data4, plot_params)

    # #######################
    # # Plot particular cases of f between 0 and pi
    m = - (2.4 - 2.2) / (12.2 - 8.5)
    b = 3 * np.pi / 2.
    bounds5 = [np.pi, 2 * np.pi]
    region_data["less_than_cond"] = True

    # Down region
    data5 = range_condition(data, 1, bounds5)
    region_data["param"] = (b, m)
    data5 = region_condition(data5, region_data)
    if (plot == 1):
        plot_params["color"] = "r."
        plot_params["alpha"] = 0.2
        plot_correlation_map_from_data(data5, plot_params)

    # Upper region
    data6 = range_condition(data, 1, bounds5)
    region_data["param"] = (b, m)
    region_data["less_than_cond"] = False
    data6 = region_condition(data6, region_data)
    if (plot == 1):
        plot_params["color"] = "g."
        plot_params["alpha"] = 0.2
        plot_correlation_map_from_data(data6, plot_params)
        plt.savefig("correaltions_frlines20rmin.png")
        plt.show()

    # nu_symmetry_2regions(data, plot_params)
    # plt.show()
    # four_regions_from_nu_correlations(data, plot)
    # nu_symmetry_single_plots(data)


def basic_histograms(name, info_input):
    base_name = info_input["base_name"]
    r_min_max = info_input["rmin_max"]

    npoints, ndim = 10000, 7
    data = get_data(name, npoints, ndim)
    names = ["r", "f", "e", "w", "i", "W"]
    val_0 = [0, 0, 0.3, 0, 0, 0]
    val_f = [r_min_max, 2 * np.pi, 5, 2 * np.pi, np.pi, 2 * np.pi]

    n_bins = int(npoints ** (1 / 3.))

    fig, axs = plt.subplots(2, 3, figsize=(20, 10))
    for i in range(data.shape[1] - 1):
        plt.subplot(2, 3, i + 1)
        p, edges = np.histogram(data[:, i], n_bins)  # ,density=True)
        x_random = (edges[1:] + edges[:-1]) / 2
        w = (data[:, i].max() - data[:, i].min()) / n_bins
        plt.title(names[i])
        plt.bar(x_random, p, align='center', alpha=0.5, width=w,
                edgecolor="k", color="b")

        plt.xlim(val_0[i], val_f[i])

    plt.savefig("basic_histograms%s.png" % (base_name))
    plt.show()


def histograms_regions_compared(name):
    npoints, ndim = 10000, 7
    data = get_data(name, npoints, ndim)
    names = ["r", "f", "e", "w", "i", "W"]
    val_0 = [0, 0, 0.3, 0, 0, 0]
    val_f = [100, 2 * np.pi, 5, 2 * np.pi, np.pi, 2 * np.pi]

    n_bins = int(npoints ** (1 / 3.))

    nu_interval_1 = (data[:, 1] < np.pi)
    nu_interval_1 = nu_interval_1.reshape(nu_interval_1.shape[0])
    w_interval_1 = (data[:, 3] < np.pi).reshape(nu_interval_1.shape[0])
    i_interval_1 = (data[:, 4] < np.pi / 2.).reshape(nu_interval_1.shape[0])

    intervals = [i_interval_1, w_interval_1, nu_interval_1]

    color = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']

    region_names = ["0-pi/0-pi/0-pi2", "pi-2pi/0-pi/0-pi2",
                    "pi-2pi/pi-2pi/0-pi2", "0-pi/0-pi/pi2-pi"]
    s_region_names = ["pi-2pi/pi-2pi/pi2-pi", "0-pi/pi-2pi/pi2-pi",
                      "0-pi/0-pi/pi2-pi", "pi-2pi/pi-2pi/0-pi2"]
    n_regions = 2 ** 3 // 2  # Considered by pairs
    # Construct the regions
    region_list = []
    s_region_list = []
    for i_region in range(n_regions):
        region = True
        s_region = True
        flip = '{0:03b}'.format(i_region)
        print("n: \n", flip)
        # Do reversed to count n order and cosntruct the region to be plotted
        for on_interval in range(len(flip)):
            if (int(flip[on_interval])):
                region *= ~intervals[on_interval]
                s_region *= intervals[on_interval]
            else:
                region *= intervals[on_interval]
                s_region *= ~intervals[on_interval]
        region_list.append(region)
        s_region_list.append(s_region)

    # Graph for each variable in all the regions to compare
    for i_var in range(data.shape[1] - 1):
        fig, axs = plt.subplots(2, 2, figsize=(10, 10))
        axs = axs.reshape(axs.shape[0] * axs.shape[1])
        for r in range(len(region_list)):
            region = region_list[r]
            # density is ignored to compare the proportions with care
            p, edges = np.histogram(data[region, i_var], n_bins)
            x_random = (edges[1:] + edges[:-1]) / 2
            w = (data[region, i_var].max() - data[region, i_var].min()) /\
                n_bins
            axs[r].set_title(names[i_var])
            axs[r].bar(x_random, p, align='center', alpha=0.5, width=w,
                       edgecolor="k", color=color[i_region],
                       label=region_names[r])
            axs[r].set_xlim(val_0[i_var], val_f[i_var])

            s_region = s_region_list[r]
            p, edges = np.histogram(data[s_region, i_var], n_bins)
            x_random = (edges[1:] + edges[:-1]) / 2
            w = (data[s_region, i_var].max() - data[s_region, i_var].min()) /\
                n_bins
            axs[r].set_title(names[i_var])
            axs[r].bar(x_random, p, align='center', alpha=0.5, width=w,
                       edgecolor="k", color=color[i_region * 2 + 1],
                       label=s_region_names[r])
            axs[r].set_xlim(val_0[i_var], val_f[i_var])
            axs[r].legend(loc="best")

        plt.savefig("Histogram%srmin_max100" % (names[i_var]))
        plt.show()


def histograms_analysis(name):
    npoints, ndim = 10000, 7
    data = get_data(name, npoints, ndim)
    names = ["r", "f", "e", "w", "i", "W"]
    val_0 = [0, 0, 0.3, 0, 0, 0]
    val_f = [100, 2 * np.pi, 5, 2 * np.pi, np.pi, 2 * np.pi]

    n_bins = int(npoints ** (1 / 3.))

    nu_interval_1 = (data[:, 1] < np.pi)
    nu_interval_1 = nu_interval_1.reshape(nu_interval_1.shape[0])
    w_interval_1 = (data[:, 3] < np.pi).reshape(nu_interval_1.shape[0])
    i_interval_1 = (data[:, 4] < np.pi / 2.).reshape(nu_interval_1.shape[0])

    intervals = [i_interval_1, w_interval_1, nu_interval_1]
    string_debug = ["i_in", "w_i", "nu_i"]

    color = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']

    fig, axs = plt.subplots(2, 3, figsize=(10, 10))
    axs = axs.reshape(axs.shape[0] * axs.shape[1])
    n_regions = 2 ** 3 // 2  # Considered by pairs
    for i_region in range(n_regions):
        region = True
        s_region = True
        flip = '{0:03b}'.format(i_region)
        print("n: \n", flip)
        # Do reversed to count n order and cosntruct the region to be plotted
        for on_interval in range(len(flip)):
            if (int(flip[on_interval])):
                region *= ~intervals[on_interval]
                s_region *= intervals[on_interval]
                print("region *=%s" % string_debug[on_interval])
            else:
                region *= intervals[on_interval]
                s_region *= ~intervals[on_interval]
                print(intervals[on_interval])
                print(region)
                print("region *=%s" % string_debug[on_interval])

        for i_var in range(data.shape[1] - 1):
            # density is ignored to compare the proportions with care
            p, edges = np.histogram(data[region, i_var], n_bins)
            x_random = (edges[1:] + edges[:-1]) / 2
            w = (data[region, i_var].max() - data[region, i_var].min()) /\
                n_bins
            axs[i_var].set_title(names[i_var])
            axs[i_var].bar(x_random, p, align='center', alpha=0.5, width=w,
                           edgecolor="k", color=color[i_region])
            axs[i_var].set_xlim(val_0[i_var], val_f[i_var])

            p, edges = np.histogram(data[s_region, i_var], n_bins)
            x_random = (edges[1:] + edges[:-1]) / 2
            w = (data[s_region, i_var].max() - data[s_region, i_var].min()) /\
                n_bins
            axs[i_var].set_title(names[i_var])
            axs[i_var].bar(x_random, p, align='center', alpha=0.4, width=w,
                           edgecolor="k", color=color[i_region])
            axs[i_var].set_xlim(val_0[i_var], val_f[i_var])

    plt.savefig("histograms_analysis_pair2.png")
    plt.show()


def make_all_histograms():
    path = "../data/rmin_sizes/quiroga/quiroga/"
    sizes = [1, 5, 10, 20, 50, 100, 500, 2000, 4000]

    for i in range(len(sizes)):
        name = path + "rmin_max%s.dat" % (sizes[i])
        base_name = re.findall(r'/(\w+)', name)[-1]
        info_input = {"base_name": base_name}
        info_input["rmin_max"] = sizes[i]

        basic_histograms(name, info_input)


def correlations_symmetry_regions(name):
    npoints, ndim = 10000, 7
    data = get_data(name, npoints, ndim)
    names = ["r", "f", "e", "w", "i", "W"]

    nu_interval_1 = (data[:, 1] < np.pi)
    nu_interval_1 = nu_interval_1.reshape(nu_interval_1.shape[0])
    w_interval_1 = (data[:, 3] < np.pi).reshape(nu_interval_1.shape[0])
    i_interval_1 = (data[:, 4] < np.pi / 2.).reshape(nu_interval_1.shape[0])

    intervals = [i_interval_1, w_interval_1, nu_interval_1]

    color = ['b.', 'g.', 'r.', 'c.', 'm.', 'y.', 'k.', 'w.']

    # region_names = ["0-pi/0-pi/0-pi2", "pi-2pi/0-pi/0-pi2",
    #                 "pi-2pi/pi-2pi/0-pi2", "0-pi/0-pi/pi2-pi"]
    # s_region_names = ["pi-2pi/pi-2pi/pi2-pi", "0-pi/pi-2pi/pi2-pi",
    #                   "0-pi/0-pi/pi2-pi", "pi-2pi/pi-2pi/0-pi2"]
    n_regions = 2 ** 3 // 2  # Considered by pairs
    # Construct the regions
    region_list = []
    s_region_list = []
    for i_region in range(n_regions):
        region = True
        s_region = True
        flip = '{0:03b}'.format(i_region)
        print("n: \n", flip)
        # Do reversed to count n order and cosntruct the region to be plotted
        for on_interval in range(len(flip)):
            if (int(flip[on_interval])):
                region *= ~intervals[on_interval]
                s_region *= intervals[on_interval]
            else:
                region *= intervals[on_interval]
                s_region *= ~intervals[on_interval]
        region_list.append(region)
        s_region_list.append(s_region)

    names = ["r", "f", "e", "w", "i", "W"]
    axes = setup_figure_axes()
    plot_params = {"variables_names": names, "axes": axes}

    for i in range(n_regions):
        plot_params["color"] = color[0]
        plot_params["alpha"] = 0.2
        plot_correlation_map_from_data(data[region_list[i], :], plot_params)
        plot_params["color"] = color[1]
        plot_correlation_map_from_data(data[s_region_list[i], :], plot_params)

        plt.savefig("pair_symmaric_region_plot_region%d.png" % i)
        plt.show()
        axes = setup_figure_axes()
        plot_params = {"variables_names": names, "axes": axes}


def three_regions_homework():
    # 3 regiones ##############
    data = get_data("../data/defaults/DefaultQuiroga1M.dat")
    nu_symmetry_3regions(data, {"filename": "Quiroga120k_3regions.png",
                                "ms": 0.15})

    data = get_data("../data/defaults/DefaultLauraUniform.dat")
    nu_symmetry_3regions(data, {"filename": "Laura120k_3regions.png",
                                "ms": 0.1})


def plot_errors():
    os.system("touch ../data/defaults/plus_minus/filenames.txt")
    command = "ls ../data/defaults/plus_minus/DefaultQuirogaUniform*p.dat >  ../data/defaults/plus_minus/filenames.txt"
    os.system(command)
    names = np.genfromtxt("../data/defaults/plus_minus/filenames.txt",
                          dtype=str)
    for i_name in range(len(names)):
        data = get_data(names[i_name])
        name_png = names[i_name][:-4] + ".png"
        nu_symmetry_3regions(data, {"filename": name_png, "ms": 0.1,
                                    "alpha": 1})


def fist_analysis():
    files = {"LauraUCase": "../data/defaults/DefaultLauraUniform.dat",
             "QuirogaUCase": "../data/defaults/DefaultQuiroga1M.dat"}

    # Basic plots: correlation maps and histograms
    directory = "experiments/100rmin120kpoints/Laura/"
    png_file = "Laura120kCorrelationsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": 1}
    plot_correlation_map_from_file(files["LauraUCase"], plot_params)

    directory = "experiments/100rmin120kpoints/Quiroga/"
    png_file = "Quiroga120kCorrelationsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": 1}
    plot_correlation_map_from_file(files["QuirogaUCase"], plot_params)

    # three_regions_homework()
    # name = "../data/defaults/DefaultQuiroga1M.dat"
    # nu_symmetry_single_plots_regions(name)

    # name = "../data/defaults/DefaultLauraUniform.dat"
    # nu_symmetry_single_plots_regions(name)

    # plot_errors()
    pass


def main():
    print()

    # fist_analysis()
    #####
    # Plot correlation map of rmin_size20 for Quiroga

    # inv:  this give me a first idea o strcuture in correlations maps
    # correlation_map_example()
    ###

    #####
    # Basic anlysis of correlation maps
    # data = get_data("../data/rmin_sizes/quiroga/quiroga/rmin_max100.dat")

    # inv:  this give me a first idea of strcuture for different regions
    # nu_symmetry_2regions(data)

    # inv: this plot by plot permit  identification of symmetrie
    # data = get_data("../data/defaults/DefaultQuiroga1M.dat")
    # nu_symmetry_single_plots(data)

    # inv:  this give me a first idea of strcuture for different regions
    # nu_symmetry_3regions(data)
    # refine_second_separation(data)
    ###

    #####
    # Sizes analysis
    # correlations_different_sizes()

    name = "../data/rmin_sizes/quiroga/quiroga/rmin_max100.dat"
    # four_regions_division_in_progress(name)

    # make_all_histograms()

    # histograms_analysis(name)
    # histograms_regions_compared(name)

    # correlations_symmetry_regions(name)

    plot_correlation_map_from_file(name)

    # tareas()
    pass


if __name__ == '__main__':
    main()
