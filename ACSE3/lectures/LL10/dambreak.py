# See Water Waves by Stoker
# and section 4.1.1 of "SWASHES: a compilation of Shallow Water
# Analytic Solutions for Hydraulic and Environmental Studies" Delestre et al,
# https://arxiv.org/pdf/1110.0288.pdf


import numpy as np

g = 9.81

def dambreak_characteristics(h_up, h_down):
    """Calculate flat-bed dambreak characterics:

    Returns shock speed, water level and discharge per unit width of the
    state between the shock and rarefaction wave of a dam break over a wet bed.
    h_up and h_down are the waterlevels upstream and downstream."""
    assert h_up > h_down
    # celerities
    c_up = np.sqrt(g*h_up)
    c_down = np.sqrt(g*h_down)
    # non-dim. celerity (as fraction of c_up)
    alpha_down = c_down/c_up
    # magic:
    pol = [1, 0, -9*alpha_down**2, 16*alpha_down**2,
           -alpha_down**2*(alpha_down**2 + 8), 0, alpha_down**6]
    alpha_m = np.roots(pol)

    # there is supposed to be only one real root between c_up and c_down:
    cm = np.real(alpha_m[(alpha_down<alpha_m) & (alpha_m<1) & np.isreal(alpha_m)])*c_up
    hm = cm**2/g
    # continuity of Riemann inv. u+2c:
    um = 2*(c_up - cm)
    qm = um*hm
    s = qm/(hm - h_down)
    # check based on jump condition for momentum: s [q] = [qu+0.5 h**2]
    np.testing.assert_almost_equal(s, (qm*um + 0.5*g*(hm**2 - h_down**2))/qm)

    return s, hm, qm

def dambreak_solution(x, hl, hr, x0, t):
    """
    Compute analytical (Ritter/Stoker) dambreak solution

    Computes analytical dambreak solution on a set of points in a 1d domain
    The dambreak starts at t=0 with h=hl for x<x0 and h=hr for x>x0."""
    if t<=0.0:
      return np.where(x<x0, hl, hr)

    h_up = max(hl, hr)
    h_down = min(hl, hr)
    c = np.sqrt(g*h_up)
    if h_down > 0.0:
        s, hm, qm = dambreak_characteristics(h_up, h_down)
        um = qm/hm
    else:
        s = 2*c
        hm = 0
        um = 0
    if hl<hr:
        # the solution below assumes hl=h_up, hr=h_down (i.e. hl>hr)
        # we get the hl<hr solution by flipping the sign of the speeds involved
        c = -c
        s = -s
        um = -um

    # rarefaction curve, based on:
    # - self-similar => linear in x-x0/t
    # - u=0 in x = x0-c*t
    # - ???
    u_rar = 2*(c + (x-x0)/t)/3.
    # based on u_rar + 2 c_rar = 2 c_up
    h_rar = (c - u_rar/2)**2/g

    h = np.select([(x-x0)/s > t, h_rar > h_up, h_rar > hm],
                  [h_down, h_up, h_rar], hm)
    u = np.select([(x-x0)/s > t, h_rar > h_up, h_rar > hm],
                  [0, 0, u_rar], um)

    return h, u
