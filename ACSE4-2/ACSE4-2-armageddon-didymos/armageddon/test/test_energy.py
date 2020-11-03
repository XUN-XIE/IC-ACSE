""" Module to independently test the method calculate_energy in solver.py. """

import pandas as pd
import numpy as np

from pytest import fixture, mark

# Use pytest fixtures to generate objects we know we'll reuse.
# This makes sure tests run quickly

@fixture(scope='module')
def armageddon():
    """Perform the module import"""
    import armageddon
    return armageddon

@fixture(scope='module')
def planet(armageddon):
    """Return a default planet with a constant atmosphere"""
    return armageddon.Planet(atmos_func='constant')

@fixture(scope='module')
def input_data():
    """Input data for test meteor"""
    input_data = {'radius': 1.,
                  'velocity': 1.0e5,
                  'density': 3000.,
                  'strength': 1e32,
                  'angle': 30.0,
                  'init_altitude':100e3,
                  'dt': 0.05,
                  'radians': False
                 }
    return input_data

@fixture(scope='module')
def result(planet, input_data):
    """Solve a default impact for the default planet"""

    result = planet.solve_atmospheric_entry(**input_data)

    return result

def test_central_diff(planet):
     """ Test central difference scheme implemented in calculate_energy()
     using simple hand derived solution.
     """

     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     exact = np.array([122, 96.5, 74, 54.5, 38, 24.5, 14, 6.5])/(4.184e9)

     energy = planet.calculate_energy(result=result,method='central')
     assert np.allclose(energy.dedz[1:9], exact)

def test_sign(planet):
     """ Test correct sign off dedz:
         - positive for a = np.arange(10,0,-1)
         - negative for a = np.arange(0,10,1)
     """

     # energy lost to atmosphere
     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='central')
     assert (energy.dedz[1:9] > 0).all() == True

     # energy gained from atmosphere (not physical but just for check)
     vec2 = np.arange(0, 10, 1)
     result = pd.DataFrame({'mass':vec2, 'velocity':vec2, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='central')
     assert (energy.dedz[1:9] < 0).all() == True

def test_repeat_altitude(planet):
     """ Check only the first instance of a given altitude is kept.
     """

     vec = np.arange(10, 0, -1)
     vec[2] = vec[0]
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     exact = np.array([109.5, 85.5, 54.5, 38, 24.5, 14, 6.5])/(4.184e9)

     energy = planet.calculate_energy(result=result,method='central')
     assert np.allclose(energy.dedz[1:8], exact)

def test_choice_deriv(planet):
     """ Test the implementation of the choice of solver.
     In turn set method to 'forward', 'backward' and 'undetermined_coeff'.
     """

     # check forward euler
     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='forward')
     exact = np.array([135.5, 108.5, 84.5, 63.5, 45.5, 30.5, 18.5, 9.5, 3.5, 3.5])/(4.184e9)
     assert np.allclose(energy.dedz, exact)

     # check backward euler
     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='backward')
     exact = np.array([135.5, 135.5, 108.5, 84.5, 63.5, 45.5, 30.5, 18.5, 9.5, 3.5])/(4.184e9)
     assert np.allclose(energy.dedz, exact)

     # check undetermined coefficients
     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='undetermined_coeff')
     exact = np.array([135.5, 378, 268, 182, 117, 70, 38, 18, 7, 3.5])/(4.184e9)
     assert np.allclose(energy.dedz, exact)

@mark.xfail()
def test_wrong_choice(planet):
     """ Test to error occurs if method is mispelled. """

     vec = np.arange(10, 0, -1)
     result = pd.DataFrame({'mass':vec, 'velocity':vec, 'altitude':vec})
     energy = planet.calculate_energy(result=result,method='random')
