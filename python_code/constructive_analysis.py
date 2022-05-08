import struct as struct
import numpy as np
import matplotlib.pyplot as plt
import re
import os

DOUBLE_SIZE = 8

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

############
# for correlations
############
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


def examples_basicU_correlation_maps(files, plot):
    # Basic plots: correlation maps and histograms
    directory = "experiments/100rmin120kpoints/Laura/"
    png_file = "Laura120kCorrelationsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": plot}
    plot_correlation_map_from_file(files["LauraUCase"], plot_params)

    directory = "experiments/100rmin120kpoints/Quiroga/"
    png_file = "Quiroga120kCorrelationsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": plot}
    plot_correlation_map_from_file(files["QuirogaUCase"], plot_params)


############
# for histograms
############
def basic_histograms(name, plot_params=None):
    data = get_data(name)

    filename = plot_params["filename"]
    r_min_max = plot_params["rmin_max"]
    plot = plot_params["plot"]

    data = get_data(name)
    npoints = len(data[:, 0])

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

    plt.savefig(filename)
    if (plot):
        plt.show()


def examples_basicU_histograms(files, plot):
    # Basic plots: correlation maps and histograms
    directory = "experiments/100rmin120kpoints/Laura/"
    png_file = "Laura120kHistogramsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": plot,
                   "rmin_max": 100}
    basic_histograms(files["LauraUCase"], plot_params)

    directory = "experiments/100rmin120kpoints/Quiroga/"
    png_file = "Quiroga120kHistigramsBasic.png"
    plot_params = {"filename": directory + png_file, "plot": plot,
                   "rmin_max": 100}
    basic_histograms(files["QuirogaUCase"], plot_params)


###############################################################################
# region aplication functions
###############################################################################
def range_condition(data, index, bounds):
    con1 = data[:, index:index + 1] > bounds[0]
    con2 = data[:, index:index + 1] < bounds[1]

    cond = con1 * con2
    cond = cond.reshape(cond.shape[0])

    return data[cond, :]


############
# for correlations in detail
############
def nu_symmetry_3regions(data, plot_params={}):
    plot_params_default = {}
    plot_params_default["filename"] = "correalations_3regionsf_default.png"
    plot_params_default["alpha"] = 0.2
    plot_params_default["limit_axes"] = 1
    plot_params_default["plot"] = 0

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

    if (plot_params["plot"]):
        plt.show()


def nu_symmetry_intervals(data, plot_params={}):
    plot_params_default = {}
    plot_params_default["filename"] = "correalations_3regionsf_default.png"
    plot_params_default["alpha"] = 0.2
    plot_params_default["limit_axes"] = 1
    plot_params_default["plot"] = 0
    plot_params_default["intervals"] = [0, np.pi, 2 * np.pi]
    plot_params_default["index"] = 1
    plot_params_default["colors"] = ["g.", "b."]

    if (plot_params is None):
        plot_params = plot_params_default
    else:
        for key in plot_params_default.keys():
            if key not in plot_params:
                plot_params[key] = plot_params_default[key]

    for i in range(len(plot_params["intervals"]) - 1):
        bounds = [plot_params["intervals"][i], plot_params["intervals"][i + 1]]
        data = range_condition(data, plot_params["index"], bounds)
    plot_params["color"] = "g."
    plot_params["alpha"] = plot_params["alpha"]
    plot_correlation_map_from_data(data, plot_params)

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

    if (plot_params["plot"]):
        plt.show()



def three_regions_correlations(files, plot):
    # 3 regiones ##############
    directory = "experiments/100rmin120kpoints/Quiroga/" \
        "basic_regions_correlations/"
    png_file = "Quiroga120k_3regions.png"

    data = get_data(files["QuirogaUCase"])
    nu_symmetry_3regions(data, {"filename": directory + png_file,
                                "ms": 0.15, "plot": plot})

    directory = "experiments/100rmin120kpoints/Laura/"\
        "basic_regions_correlations/"
    png_file = "Laura120k_3regions.png"

    data = get_data(files["LauraUCase"])
    nu_symmetry_3regions(data, {"filename": "Laura120k_3regions.png",
                                "ms": 0.1, "plot": plot})


def complete_correlation_analysis(files):
    # First let us simply plot by regions and show some properties that appear
    #   to be present in general

    three_regions_correlations(files, 0)


def main():
    files = {"LauraUCase": "../data/defaults/DefaultLauraUniform.dat",
             "QuirogaUCase": "../data/defaults/DefaultQuiroga1M.dat"}

    # examples_basicU_correlation_maps(files, plot=0)
    # examples_basicU_histograms(files, plot=1)

    complete_correlation_analysis(files)


if __name__ == '__main__':
    main()
