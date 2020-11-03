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

def axes_caption(ax, text, **kwargs):
    return ax.text(.5, -.2, text, transform=ax.transAxes, verticalalignment='top', horizontalalignment='center', wrap=True)

def figure1():
    fig, ax = plt.subplots(1,3, figsize=(16,4))
    xr = np.linspace(-5, 5)

    def f(x):
        return (1-x)*(x+2)**2
    ax[0].plot(xr, f(xr), 'b', label='$f(x)=(1-x)(x+2)^2$')
    ax[0].legend()
    axes_caption(ax[0], 'A function that is not bounded\n from below.')
    
    def f(x):
        return np.exp(-x)
    ax[1].plot(xr, f(xr), 'b', label='$f(x)=e^{-x}$')
    ax[1].legend()
    axes_caption(ax[1], '''A function that is bounded
    from below, $f(x)>0$, but it
    does not attain a minimum
    in any point.''')

    def f(x):
        return (x+3)**2*(x-3)**2
    ax[2].plot(xr, f(xr), 'b', label='$f(x)=(x+3)^2(x-3)^2$')
    ax[2].legend()
    axes_caption(ax[2], '''The minimum of this function
    is 0. This minimum is attained
    in two points
    $x=-3$ and $x=3$.''')

def figure2():
    # set up grid of 2x2 figures
    fig, ((ax11, ax12), (ax21, ax22)) = plt.subplots(2, 2, figsize=(12, 8))
    # add some padding otherwise axes the labels can overlap with the next subplot
    fig.tight_layout(w_pad=4)

    x=np.linspace(-7, 7, 100)

    def f(x):
        return x**4 + 8*x**3/3. - 50*x**2 - 200*x
    def fp(x):
        return 4*x**3 + 8*x**2 - 100*x - 200
    extrema = np.array([-5., -2., 5.])

    ax11.plot(x, f(x), 'b', label='$f(x)$')
    ax11.plot(extrema, f(extrema), 'b+', markersize=10, linewidth=2)
    ax11.legend()
    ax21.plot(x, fp(x), 'b', label="$f'(x)$")
    ax21.plot([-7, 7], [0, 0], 'k')
    ax21.legend()

    ax12.plot(x, x**3, 'b', label='$x^3$')
    ax12.plot(0.0, 0.0, 'b+', markersize=10, linewidth=2)
    ax12.legend()
    ax22.plot(x, 3*x**2, 'b', label='$3x^2$')
    ax22.plot([-7, 7], [0, 0], 'k')
    ax22.legend(loc='best')


def figure3():
    # set up grid of 2x2 figures
    fig, ax = plt.subplots(2, 2, figsize=(12, 7))
    # add some padding otherwise axes the labels can overlap with the next subplot
    fig.tight_layout(w_pad=4)

    x=np.linspace(-7, 7, 100)
    ax[0,0].plot(x, np.abs(x), 'b', label='$f(x)=|x|$')
    ax[0,0].legend()
    ax[1,0].plot([-7,0], [-1, -1], 'b', label="$f'(x)$")
    ax[1,0].plot([0,7], [1, 1], 'b')
    ax[1,0].plot([0,0,], [-1,1], 'bo', fillstyle='none') # FIXME: should be open
    ax[1,0].plot([-7, 7], [0, 0], 'k')
    ax[1,0].legend()

    x = np.linspace(-1, 1, 100)
    ax[0,1].plot(x, np.exp(-1/x**2), label='$g(x)=e^{-1/x^2}$')
    ax[0,1].plot(0.0, 0.0, 'b+', markersize=10, linewidth=2)
    ax[0,1].legend()
    ax[1,1].plot(x, 2*np.exp(-1/x**2)/x**3, label="$g'(x)$")
    ax[1,1].plot([-1, 1], [0, 0], 'k')
    ax[1,1].legend();


def figure4():
    from mpl_toolkits.mplot3d import Axes3D

    A = np.array([[1., 0], [0., -1]])

    fig = plt.figure(figsize=(8,8))
    ax1 = fig.add_subplot(1, 1, 1, projection='3d')

    x = np.linspace(-10, 10, 20)
    y = np.linspace(-10, 10, 20)
    # xy are the locations of the horizontal grid, it is a Nx x Ny x 2 array
    xy = np.array(np.meshgrid(x, y)).T
    # at every point x_ij compute the product \sum_k x_ijk A_kl
    # axes argument refers to last axis (-1) of x and first (0) of A
    xT_A = np.tensordot(xy, A, axes=(-1,0))
    # compute f = x^T A x, with fxy_ij = \sum_l xT_A_ijl x_ijl
    fxy = (xT_A * xy).sum(axis=-1)

    ax1.plot_surface(xy[:,:,0], xy[:,:,1], fxy);
    ax1.set_xlabel('x-axis', size=24, labelpad=10)
    ax1.set_ylabel('y-axis', size=24, labelpad=10);

def elliptic_matrix(major_axis, major_radius, minor_radius):
    """
    Derive the matrix A such that 0.5 x^T A X + b x = 1 is an ellipse
    
    major_axis: vector indicating major axis of ellipse
    major_radius, minor_radius: major and minor radius of ellipese
    """
    # Q is a matrix whose rows are the eigenvectors of A
    Q = np.empty((2,2))
    # first row is the normalized major_axis vector
    Q[0, :] = np.array(major_axis)/sl.norm(major_axis)
    # second row the orthogonal minor_axis vector
    Q[1, 0] = -Q[0, 1]
    Q[1, 1] = Q[0,0]
    D = np.diag([2./major_radius**2, 2./minor_radius**2])
    return Q.T @ D @ Q

def quadratic_function(A, b, constant=0.):
    """Create function f(x)=0.5 x^T A X - b x + constant
    
    Function is created such that x can be any rank>=1 array
    where the last axis is assumed to be the coordinate dimension."""
    def f(x):
        xT_A = np.tensordot(x, A, axes=(-1,0))
        xT_A_x = (xT_A*x).sum(axis=-1)
        bx = np.tensordot(b, x, axes=(0,-1))
        return 0.5*xT_A_x - bx + constant
    return f


def sample_points_on_contour(ax, cval, A, b):
    """Draw 4 sample points on a contour cval of the quadratic function f(x)=0.5 x^T A x -b x, where b = A xs"""
    # we'll do this by writing x = xs + xi1 v1 + xi2 v2,
    # where xs is the minimum of f and b = A xs
    # and v1, v2 are the normalized eigenvectors of A with eigenvalues l1 and l2
    # by substitution we can then derive that for points on the contour:
    # 0.5 l1 xi1^2 + 0.5 l2 xi2^2 = gamma^2 with
    xs = sl.solve(A, b)
    gamma = np.sqrt((0.5*np.dot(xs,np.dot(A,xs)) + cval))

    # work-back the ellipse parameters:
    w, v= sl.eigh(A)
    v_maj = v[0]
    v_min = v[1]
    R_maj = np.sqrt(2./w[0])
    R_min = np.sqrt(2./w[1])

    sample_x = []

    # a point in minor direction, xi1=0, xi2=gamma
    x = np.array(xs) + gamma*R_min*v_min
    sample_x.append(x)

    # a point in major direction, xi1=gamma, xi2=0
    x = np.array(xs) + gamma*R_maj*v_maj
    sample_x.append(x)

    # two inbetween points with xi2=mu*xi1
    mu = 1.
    xi1 = gamma/np.sqrt(1/R_maj**2 + mu**2/R_min**2)
    x = np.array(xs) + xi1*v_maj + xi1*mu*v_min
    sample_x.append(x)

    # this is the worst case according to Shewchuck:
    mu = R_min**2/R_maj**2
    xi1 = gamma/np.sqrt(1/R_maj**2+mu**2/R_min**2)
    x = np.array(xs) + xi1*v_maj + xi1*mu*v_min
    sample_x.append(x)

    return sample_x

def figure5(x, A, b):
    """HOMEWORK"""

    f = quadratic_function(A, b)
    r = b- A @ x
    alpha = np.linspace(0, 10, 50)
    xy = (x + alpha[:,np.newaxis]*r[np.newaxis:])
    fig, ax = plt.subplots()
    ax.plot(alpha, f(xy), 'r')

    alpha_star = np.dot(r, r)/np.dot(r, A @ r)
    f_star = f(x + alpha_star*r)
    ax.plot(alpha_star, f_star, 'ko')
    ax.annotate(r'$\hat f(\alpha^*)$', xy=(alpha_star-0.1, f_star+0.1), xytext=(2,3),
                arrowprops={'arrowstyle': '->'})
    ax.set_xlabel(r"$\alpha$")
    ax.set_ylabel(r"$\hat f(\alpha)$");

def steepest_descent(A, b, x):
    """Perform one steepest descent step"""
    r = b - A @ x
    alpha_star = np.dot(r, r)/np.dot(r, A @ r)
    return x + alpha_star*r

def figure6(x0, A, b):

    f = quadratic_function(A, b)
    xs = sl.solve(A, b)

    fig = plt.figure(figsize=(12,6))
    ax = []
    ax.append(plt.subplot2grid((1,5), (0,0), colspan=2))
    ax.append(plt.subplot2grid((1,5), (0,2), colspan=3))
    x = np.linspace(-5, 5, 50)
    y = np.linspace(-5, 5, 50)
    # create a Nx x Ny x 2 (Nx=Ny=50) array of grid points
    xy = np.array(np.meshgrid(x, y)).T
    c = ax[0].contour(xy[:,:,0], xy[:,:,1], f(xy), 25)

    def arrow(ax, x, dx):
        ax.plot(x[0], x[1], 'k.')
        ax.annotate("", xy=x+dx, xytext=x,
               arrowprops={'arrowstyle': '->'})

    # this is the worst case according to Shewchuck:
    r0 = A @ x0 - b
    x1 = steepest_descent(A, b, x0)
    r1 = A @ x1 - b
    x2 = x1 - 10*r1
    ax[0].plot([x0[0], x1[0]], [x0[1], x1[1]], 'r')
    ax[0].plot([x1[0], xs[0]], [x1[1], xs[1]], 'b')
    ax[0].plot([x1[0], x2[0]], [x1[1], x2[1]], 'k--')
    arrow(ax[0], x0, -r0*2)
    arrow(ax[0], x1, -r1*2)
    ax[0].annotate(r"$\bf x^{(0)}$", xy=x0, fontsize=20,
                   horizontalalignment='right', verticalalignment='top')
    ax[0].annotate(r"$\bf x^{(1)}$", xy=x1, fontsize=20,
                   horizontalalignment='left', verticalalignment='top')
    ax[0].annotate(r"$\bf x^{*}$", xy=xs, fontsize=20,
                   horizontalalignment='right', verticalalignment='bottom')
    ax[0].grid()
    ax[0].axis('equal')

    Phi = sl.cholesky(A)
    xyh = np.tensordot(xy, Phi, axes=(-1,1))

    ax[1].contour(xyh[:,:,0], xyh[:,:,1], f(xy), 25)
    xh0 = Phi @ x0
    xh1 = Phi @ x1
    xhs = Phi @ xs
    xh2 = Phi @ x2
    ax[1].plot([xh0[0], xh1[0]], [xh0[1], xh1[1]], 'r')
    ax[1].plot([xh1[0], xhs[0]], [xh1[1], xhs[1]], 'b')
    ax[1].plot([xh1[0], xh2[0]], [xh1[1], xh2[1]], 'k--')
    arrow(ax[1], xh0, -Phi @ r0 * 2)
    arrow(ax[1], xh1, -Phi @ r1 * 2)
    ax[1].annotate(r"$\hat{\bf x}^{(0)}$", xy=xh0, fontsize=20,
                   horizontalalignment='right', verticalalignment='top')
    ax[1].annotate(r"$\hat{\bf x}^{(1)}$", xy=xh1, fontsize=20,
                   horizontalalignment='left', verticalalignment='top')
    ax[1].annotate(r"$\hat{\bf x}^{*}$", xy=xhs, fontsize=20,
                   horizontalalignment='left', verticalalignment='bottom')

    ax[1].axis('equal')

    from matplotlib.patches import ConnectionPatch
    con = ConnectionPatch(xyA=xhs+(-0.1,0.3), coordsA='data',
                          xyB=xs+(0.1,0.3), coordsB='data',
                          axesA=ax[1], axesB=ax[0],
                          arrowstyle='<|-,head_width=1,head_length=2', linewidth=2, fill=True,
                          facecolor='k', edgecolor='k',
                          connectionstyle='arc3,rad=.5')
    ax[1].add_artist(con)
    ax[1].annotate(r"$\bf\hat x=\Phi x$", xy=(-3, 2.5), fontsize=24)
    plt.show()

