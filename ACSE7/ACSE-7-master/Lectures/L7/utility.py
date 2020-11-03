# the following allows us to plot triangles indicating convergence order
from mpltools import annotation
import numpy as np
import matplotlib.pyplot as plt
import scipy.linalg as sl
import scipy.optimize as sop

def taylor_test(f, grad_f, x, h0, max_iter=20, plot_convergence=True, print_convergence=True):
    """Taylor test to verify that the function grad_f is the derivative of the function `f`
    
    We test:
    
       f(x + h) = f(x) + grad_f(x).h + O(h^2)
    
    f, grad_f  - function and its derivative to test
    x          - point in which to test
    h0         - initial perturbation
    max_iter   - number of times that h is halved
    plot_convergence - whether to plot the convergence of the Taylor residual f(x+h)-f(x)-grad_f(x).h
    print_convergence - prints the order of convergence between subsequent iterations."""


    h = h0.copy()
    residuals = []
    hnorms = []
    dfdx = grad_f(x)
    for i in range(max_iter):
        r = f(x + h) - f(x) - np.sum(dfdx * h)
        residuals.append(sl.norm(r))
        hnorms.append(sl.norm(h))
        h /= 2.

    if plot_convergence:
        fig, ax = plt.subplots(1,2, figsize=(16,4))
        ax[0].semilogy(residuals)
        ax[0].set_xlabel('iteration')
        ax[0].set_ylabel('Taylor residual $|r|$')
        ax[0].set_xticks(range(0,max_iter,2))
        ax[1].loglog(hnorms, residuals)
        ax[1].set_xlabel('perturbation size $\|h\|$')
        ax[1].set_ylabel('Taylor residual $|r|$')

        # NOTE: slope_marker does not seem to work in semilogy plots
        annotation.slope_marker((1e-3, 1e-4), (2, 1), invert=True, ax=ax[1], size_frac=.2)

    if print_convergence:
        residuals = np.array(residuals)
        print('Order of convergence, log(r(h_i)/r(h_{i+1}))/log(2):')
        print(np.log(residuals[:-1]/residuals[1:])/np.log(2))


class ShapeOptimisationProblem:
    """Shape optimisation based on a polygon, in which the vertices are moved vertically only
    
    The first and last vertex of this polygon, described a list of vertices
    should be the same and will not be changed during the optimisation."""
    def __init__(self, x0, f_user, grad_f_user, store_function_evaluations=True, store_gradient_evaluations=True):
        """
        x0     - the initial polygon of n edges, described by a shape (n+1,2) array of vertex positions,
                 where the first and last vertex should be the same to describe a closed polygon
        f_user      - the function that computes the objective values, should take a shape (n+1,2) array
        grad_f_user - function that computes the gradient of f, should take and return a shape (n+1,2) array
        store_function_evaluations, store_gradient_evaluations - if either set to true, self.x_i stores
                                                                 a list of (n+1,2) arrays representing
                                                                 the polygons for which the function or 
                                                                 gradient was evaluated."""        
        self.f_user = f_user
        self.grad_f_user = grad_f_user
        self.x0 = x0.copy()  # we keep a copy of the original polygon
        self.x = x0.copy()  # this is the copy we are actually going to change
        self.n = len(x0)-1
        self.x_i = []  # stores subsequent iterations when asked for
        self.store_function_evaluations = store_function_evaluations
        self.store_gradient_evaluations = store_gradient_evaluations
        
    def minimize(self, *args, **kwargs):
        """Wrapper around scipy.optimize.minimize
        
        Passes the correct function, gradient and initial guess. Any further arguments are also passed on
        to scipy's minimize"""
        # make a copy of x0, so that if we call minimize() again
        # we again start from the same initial guess provided in __init__()
        self.x = self.x0.copy()
        # now turn that into a flat vector
        x0_vec = self.get_x_vec()
        return sop.minimize(self.f, x0_vec, jac=self.jac, *args, **kwargs)
        
    def set_x(self, x_vec):
        """Set self.x based on the (n-1)-vector used by scipy minimize"""
        # copy into self.x, leaving first and last vertex unchanged:
        self.x[1:self.n, 1] = x_vec
        
    def get_x_vec(self):
        """Obtain the (n-1)-vector from self.x"""
        return self.x[1:self.n,1].copy()
                
    def f(self, x_vec):
        """Wrapper function f, called by scipy's minimize"""
        self.set_x(x_vec)
        if self.store_function_evaluations:
            self.x_i.append(self.x.copy())
        f = self.f_user(self.x)
        return f
    
    def jac(self, x_vec):
        """Wrapper gradient function, called by scipy's minimize"""
        self.set_x(x_vec)
        if self.store_gradient_evaluations:
            self.x_i.append(self.x.copy())
        grad = self.grad_f_user(self.x)
        return grad[1:self.n,1].copy()  # skip gradient associated with first and last vertex
