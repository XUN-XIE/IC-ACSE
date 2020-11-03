import numpy as np # for the sqrt function

def solve_quadratic(a,b,c):
    """Solve a quadratic equation ax^2+bx+c=0 in the reals"""
    if 0<b**2-4.0*a*c:
        # two solutions
        return ((-b-np.sqrt(b**2-4.0*a*c))/(2.0*a),(-b+np.sqrt(b**2-4.0*a*c))/(2.0*a))
    elif 0==b**2-4.0*a*c:
        # one solution
        return -b/(2.0*a)
    else:
        # no solutions
        return None