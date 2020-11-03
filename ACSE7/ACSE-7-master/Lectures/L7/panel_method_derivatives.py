import numpy as np
from potential_flow import skew

__all__ = ['panel_method_x_derivative']


def make_dundx_core():
    """Return lambdaized sympy derivative of normal velocity

    This routine contains the symbolic formula (in sympy) to compute the normal
    velocity to a panel i due to vortex and source terms from panel j. It returns the
    derivative w.r.t 8 x and y coordinates:
    x0, x1 = xAi, yAi
    x2, x3 = xBi, yBi
    x4, x5 = xAj, yAj
    x6, x7 = xBj, yBj
    This derivative is returned as a sympy lambda function that takes in the 8 coordinates,
    vortex strength gamma (assumed the same for all panels), sigma[j] and a sign s
    which determines on which side the centre of panel i is w.r.t panel j. The lambda function
    returns the 8 derivatives."""
    import sympy
    x = sympy.symbols('x:8')
    x0, x1, x2, x3, x4, x5, x6, x7 = x
    gamma, sigma, s = sympy.symbols('gamma,sigma,s')
    acos = sympy.acos
    pi = sympy.pi
    sqrt = sympy.sqrt
    log = sympy.log

    un = (gamma*(-(x0 - x2)*(s*(x5 - x7)*acos(((x0 + x2 - 2*x4)*(x0 + x2 - 2*x6) + (x1 + x3 - 2*x5)*(x1 + x3 - 2*x7))
                                              /(sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)
                                                *sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)))
                             - (x4 - x6)*log(sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)
                                             /sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)))
                 + (x1 - x3)*(s*(x4 - x6)*acos(((x0 + x2 - 2*x4)*(x0 + x2 - 2*x6) + (x1 + x3 - 2*x5)*(x1 + x3 - 2*x7))
                                               /(sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)
                                                 *sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)))
                              + (x5 - x7)*log(sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)
                                              /sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2))))
          + sigma*((x0 - x2)*(s*(x4 - x6)*acos(((x0 + x2 - 2*x4)*(x0 + x2 - 2*x6) + (x1 + x3 - 2*x5)*(x1 + x3 - 2*x7))
                                               /(sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)
                                                 *sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)))
                              + (x5 - x7)*log(sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)
                                              /sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)))
                   + (x1 - x3)*(s*(x5 - x7)*acos(((x0 + x2 - 2*x4)*(x0 + x2 - 2*x6) + (x1 + x3 - 2*x5)*(x1 + x3 - 2*x7))
                                                 /(sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2)
                                                   *sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)))
                                - (x4 - x6)*log(sqrt((x0 + x2 - 2*x6)**2 + (x1 + x3 - 2*x7)**2)
                                                /sqrt((x0 + x2 - 2*x4)**2 + (x1 + x3 - 2*x5)**2))))
         )/(2*pi*sqrt((x0 - x2)**2 + (x1 - x3)**2)*sqrt((x4 - x6)**2 + (x5 - x7)**2))

    m = sympy.Matrix([sympy.diff(un, xi) for xi in x])
    args = x + (gamma, sigma, s)
    dundx = sympy.lambdify(args, m)
    return dundx


def make_dundx():
    """Wrapper around dundx_core."""
    dundx_core = make_dundx_core()

    def dundx(xai, xbi, xaj, xbj, gamma, sigma):
        # s = np.sign(np.dot(n, xac))
        s = np.sign(np.dot(-skew(xbj-xaj), (xai+xbi)/2.-xaj))
        return dundx_core(xai[0], xai[1], xbi[0], xbi[1], xaj[0], xaj[1], xbj[0], xbj[1], gamma, sigma, s).flatten()
    return dundx


"""panel_dundx is a function of the form:
    def panel_dundx(xai, xbi, xaj, xbj, gamma, sigma):
that computes the derivative of the normal velocity on panel i (between xai and xbi)
due to vortex and source terms based on panel j (between xaj and xbj) with strengths
gamma and sigma. resp. It returns the derivative w.r.t the 8 coordinates involved."""
panel_dundx = make_dundx()


def dtdx(xa, xb):
    """Derivative of normalized tangent vector between xa and xb w.r.t the 4 coordinates.

    Returned as a 2x2 array containing the derivatives w.r.t xa, ya, xb and yb resp."""
    (x0, x1), (x2, x3) = xa, xb
    return np.array([
        [
            [-(x1 - x3)**2/((x0 - x2)**2 + (x1 - x3)**2)**(3/2),
             (x0 - x2)*(x1 - x3)/((x0 - x2)**2 + (x1 - x3)**2)**(3/2)],
            [(x1 - x3)**2/((x0 - x2)**2 + (x1 - x3)**2)**(3/2),
             -(x0 - x2)*(x1 - x3)/((x0 - x2)**2 + (x1 - x3)**2)**(3/2)],

        ], [
             [(x0 - x2)*(x1 - x3)/((x0 - x2)**2 + (x1 - x3)**2)**(3/2),
              -(x0 - x2)**2/((x0 - x2)**2 + (x1 - x3)**2)**(3/2)],
             [-(x0 - x2)*(x1 - x3)/((x0 - x2)**2 + (x1 - x3)**2)**(3/2),
              (x0 - x2)**2/((x0 - x2)**2 + (x1 - x3)**2)**(3/2)]
        ]])


def dndx(xa, xb):
    """Derivative of normal vector of panel between xa and xb w.r.t the 4 coordinates.

    Returned as a 2x2 array containing the derivatives w.r.t xa, ya, xb and yb resp."""
    dt = dtdx(xa, xb)
    return np.array([dt[1], -dt[0]])


def panel_method_x_derivative(sigma, x, u_inf, alpha):
    """Derivative of panel method linear system w.r.t x coordinates of vertices.

    If the linear system to be solved for the panel method is written as g(x, sigma)=A(x)sigma-b(x)
    where A(x) and b(x) are the matrix and rhs-vector which depend on the coordinates of the
    vertices x, this return the *partial* derivative dg(x, sigma)/dx.
    In a system with n panels, gamma represent n+1 equations: n equations enforcing no normal
    flow on the panels and 1 equation to enforce the Kutta condition. The vector x can be thought
    of as a (n+1)x2 array for the n+1 vertices. The derivative is therefore returned as a
    (n+1)x(n+1)x2 array. If dx is a perturbation in x (hence dx.shape is also (n+1)x2), then
    the derivative in the direction dx is computes as:
        dgdx = panel_method_x_derivative(x, sigma, u_inf, alpha)
        g = np.tensordot(dgdx, dx, 2)  # contract over last 2 indices."""
    n = x.shape[0]-1
    gamma = sigma[n]

    u_inf_vec = [u_inf*np.cos(alpha), u_inf*np.sin(alpha)]

    dgdx = np.zeros((n+1, n+1, 2))

    # derivative of normal velocities on panels i due to vortex gamma and source
    # sigma on panel j
    for i in range(n):
        for j in range(n):
            if j == i:
                continue

            dundx = panel_dundx(x[i], x[i+1], x[j], x[j+1], gamma, sigma[j]).reshape(4, 2)
            dgdx[i, i] += dundx[0]  # derivative w.r.t xA_i=x[i]
            dgdx[i, i+1] += dundx[1]  # w.r.t. xB_i=x[i+1]
            dgdx[i, j] += dundx[2]   # w.r.t  xA_j=x[j]
            dgdx[i, j+1] += dundx[3]  # w.r.t xB_j=x[j+1]

        # derivative of free stream: n \cdot u_inf  (n depends on coordinates)
        dgdx[i, i:i+2, :] += np.tensordot(u_inf_vec, dndx(x[i], x[i+1]), 1)

    # Kutta condition:
    # by symmetry of the panel method, if we swap (gamma, sigma) <-> (-sigma, gamma)
    # we obtain the tangential velocity on each panel. Here we only need to consider
    # panels 0 and n-1 which are the 2 panels at the trailing edge to which the
    # Kutta condition is applied
    for i in [0, n-1]:
        for j in range(n):
            if j == i:
                continue

            dutdx = panel_dundx(x[i], x[i+1], x[j], x[j+1], -sigma[j], gamma).reshape(4, 2)
            dgdx[n, i] += dutdx[0]
            dgdx[n, i+1] += dutdx[1]
            dgdx[n, j] += dutdx[2]
            dgdx[n, j+1] += dutdx[3]

        # derivative of free stream contr. to Kutta condition: t \cdot u_inf  (t depends on coordinates)
        dgdx[n, i:i+2, :] += np.tensordot(u_inf_vec, dtdx(x[i], x[i+1]), 1)
    return dgdx
