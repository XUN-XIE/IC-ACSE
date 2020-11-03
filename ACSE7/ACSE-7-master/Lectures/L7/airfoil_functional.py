import numpy as np
import matplotlib.pyplot as plt
import scipy.linalg as sl
import scipy.sparse as sp
import scipy.sparse.linalg as spl
import triangle
from matplotlib.tri import Triangulation, LinearTriInterpolator

def area(x):
    """Compute the area of the polygon x"""
    n = len(x)-1
    a = 0.
    for i in range(n):
        a += (x[i,0]-x[i+1,0])*(x[i,1]+x[i+1,1])/2.
    return a

def length(x):
    """Compute the length of the path described by the list of points x."""
    n = len(x)-1
    l = 0.
    for i in range(n):
        l += sl.norm(x[i+1]-x[i])
    return l

def shape_factor(x):
    """Computes the ratio l/sqrt{A}"""
    return length(x)/np.sqrt(area(x))

def grad_length(x):
    """Compute the gradient of length(x)."""
    n = len(x)-1
    dldx = np.zeros((n+1, 2))
    for i in range(n):
        dldx[i, 0] += -(x[i+1,0]-x[i,0])/sl.norm(x[i+1]-x[i])
        dldx[i+1, 0] += (x[i+1,0]-x[i,0])/sl.norm(x[i+1]-x[i])
        dldx[i, 1] += -(x[i+1,1]-x[i,1])/sl.norm(x[i+1]-x[i])
        dldx[i+1, 1] += (x[i+1,1]-x[i,1])/sl.norm(x[i+1]-x[i])
    return dldx

def grad_area(x):
    """Compute the gradient of area(x)."""
    n = len(x)-1
    dadx = np.zeros((n+1, 2))
    for i in range(n):
        dadx[i, 0] += (x[i,1]+x[i+1,1])/2.
        dadx[i+1, 0] += -(x[i,1]+x[i+1,1])/2.
        dadx[i, 1] += (x[i,0]-x[i+1,0])/2.
        dadx[i+1, 1] += (x[i,0]-x[i+1,0])/2.

    return dadx

def grad_shape_factor(x):
    dQdx = (grad_length(x) * np.sqrt(area(x)) - length(x)*grad_area(x)/np.sqrt(area(x))/2.)/area(x)
    return dQdx

class AirfoilFunctional:
    """A functional combining lift and shape_factor of an airfoil.
    
    This functional relies on the functions shape_factor(), grad_shape_factor, length(x),
    and grad_length(x) being defined elsewhere"""
    def __init__(self, verbose=True, aspect_ratio=20.0, Q_max=3.8, Q_scale=1e-2, Q_eps=1e-4):
        """
        verbose - if true print lift and shape factor and combined function value at each evaluation
        aspect_ratio - used in shape_factor: aspect ratio of ellipse that has minimal value for Q
        Q_max - maximal allowable value for shape factor Q
        Q_scale - scales the penalty parameter
        Q_eps - cap log term at this distance from Q_max
        """
        self.verbose = verbose
        self.aspect_ratio = aspect_ratio
        self.Q_max = Q_max
        self.Q_scale = Q_scale
        self.Q_eps = Q_eps
        
    def shape_factor_ar(self, x):
        """Rescaled shape_factor that is minimal for an ellipse of the specified aspect ratio."""
        x[:,1] *= self.aspect_ratio  # scale y-coordinates: ellipse -> circle
        Q = shape_factor(x)
        x[:,1] /= self.aspect_ratio   # change back y-coords
        return Q

    def grad_shape_factor_ar(self, x):
        """Gradient of shape_factor_ar"""
        x[:,1] *= self.aspect_ratio  # scale y coordinates: ellipse -> circle
        dQdx = grad_shape_factor(x)
        x[:,1] /= self.aspect_ratio  # change back y-coords
        # also rescale the y-derivatives:
        dQdx[:,1] *= self.aspect_ratio
        return dQdx

    def lift(self, sigma, x):
        """Return the lift coefficient"""
        return -2*sigma[-1] * length(x)

    def f(self, sigma, x):
        """Functional: f(sigma, x) = -lift + Q_scale * shape_factor"""
        liftx = self.lift(sigma, x)
        Q = self.shape_factor_ar(x)
        penalty = - self.Q_scale * np.log(np.maximum(self.Q_max-Q, self.Q_eps))
        if self.Q_max-Q<self.Q_eps:
            penalty += self.Q_scale * (Q-self.Q_max+self.Q_eps)/self.Q_eps

        f_value = -liftx + penalty
        if self.verbose:
            print("C_L, Q, penalty, f(sigma, x)", liftx, Q, penalty, f_value)

        return f_value
    
    def grad_x_f(self, sigma, x):
        """Partial derivative of f with respect to x"""
        # only the shape factor term explicitly depends on x
        Q = self.shape_factor_ar(x)
        g = self.Q_scale * self.grad_shape_factor_ar(x) / np.maximum(self.Q_max-Q, self.Q_eps)
        g += 2*sigma[-1] * grad_length(x)
        return g
    
    def grad_sigma_f(self, sigma, x):
        """Partial derivative of f with respect to sigma"""
        g = np.zeros_like(sigma)
        g[-1] = 2. * length(x)
        return g
