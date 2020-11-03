import numpy as np
import matplotlib.pyplot as plt
# the following allows us to plot triangles indicating convergence order
from mpltools import annotation
# scipy's optimization
import scipy.optimize as sop
import scipy.linalg as sl

# font sizes for plots
plt.rcParams['font.size'] = 16
plt.rcParams['font.family'] = 'sans-serif'
plt.rcParams['font.sans-serif'] = ['Arial', 'Dejavu Sans']

def plot_picard_iteration(ax, x_n, y_n, g, max_labels=6, flabel=''):
    "Function to plot the Picard algorithm and its convergence."
    # plot the iteration results
    ax.scatter(x_n, y_n, marker='x', color='red', s=30)

    # plot the convergence pattern
    x_pattern = [x_n[0]]
    f_pattern = [x_n[0]]
    for i in range(1, len(x_n)):
        x_pattern.append(x_n[i-1])
        f_pattern.append(y_n[i-1])
        x_pattern.append(x_n[i])
        f_pattern.append(y_n[i-1])
    x_pattern.append(x_n[i])
    f_pattern.append(y_n[i])
    ax.plot(x_pattern, f_pattern, 'g--')

    # plot the function over the x values considered
    xf = np.linspace(np.min(x_n)-0.1, np.max(x_n)+.2, 100)
    ax.plot(xf, g(xf), 'b', label='$y = G(x)$')
    ax.plot(xf, xf, 'k', label='$y = x$')
    ax.legend(loc='best', fontsize=14)

    # add some labels
    # figure out a reasonable offset for labels
    dxt = (np.max(x_pattern)-np.min(x_pattern))/50.
    dyt = (np.max(f_pattern)-np.min(f_pattern))/50.
    # only plot a maximum of max_labels labels, so plot doesn't get too messy
    for i in range(0, min(max_labels+1, len(x_n))):
        ax.text(x_n[i]+dxt, y_n[i]+dyt, '$x_{%d}$' % i, fontsize=16)

    ax.set_xlabel('$x$', fontsize=16)
    ax.set_ylabel('$y=G(x)$', fontsize=16)
    ax.set_title('Successive Approximations', fontsize=20)

def plot_iteration(ax, x_n, y_n, f, 
        max_labels=6, resfct = 100, include_chords=True,
        left_extra=0.01, right_extra=0.01):
    """Plot convergence of 1-dimensional iterative scheme

    ax -  the matplotlib Axes
    x_n, y_n - iteration values x^(n) and f(x^(n))
    f -   the function to minimize
    max_labels - only plot first `max_labels` labels x_i to avoid clutter
    resfct - number of points used to plot f
    include_chords - draw lines (chords) (x^n, 0) to (x^n, y^n) and (x^n,y^n) to (x^n+1,0)"""
    if include_chords:
        # create a list including chord points:
        x_c = sum([[x, x] for x in x_n[1:]], [x_n[0]])
        y_c = sum([[0, y] for y in y_n[1:]], [y_n[0]])
    else:
        x_c = x_n
        y_c = y_n
    # the iteration results
    ax.scatter(x_c, y_c, marker='x', color='red', s=30)

    # the convergence pattern
    ax.plot(x_c, y_c, color='green', ls='--')

    # add some labels
    # figure out a reasonable offset for labels
    dxt = (np.max(x_n)-np.min(x_n))/50.
    dyt = (np.max(y_n)-np.min(y_n))/50.
    # only plot a maximum of max_labels labels, so plot doesn't get too messy
    for i,(x,y) in enumerate(zip(x_n, y_n)):
        ax.text(x_n[i]+dxt, y_n[i]+dyt, '$x_{}$'.format(i), fontsize=16)
        if i == max_labels:
            break

    # the function
    x = np.linspace(np.min(x_n) - left_extra, np.max(x_n) + right_extra, resfct)
    ax.plot(x, f(x), 'b', label='$F(x)$')

    # zero line
    xlim = ax.get_xlim()
    ax.plot([xlim[0], xlim[1]], [0., 0.], 'k--')
    # add ticks for the x_n
    for x in x_n:
        ax.plot([x, x], [-dyt, dyt], 'k')
    ax.set_xlim(xlim)
    ax.set_xlabel('$x$', fontsize=16)
    ax.set_ylabel('$y=F(x)$', fontsize=16)


def plot_triangle(ax, x_n, y_n, m):
    "Add a triangle to indicate the line slope (with the slope being 1/m!)"
    # work out a central location:
    xc = (np.max(x_n) + np.min(x_n))/2.
    yc = (np.max(y_n) + np.min(y_n))/2.
    annotation.slope_marker((xc, 2*yc), (1./m, 1.), invert=False, ax=ax,
                        size_frac=0.15, pad_frac=0.05)

