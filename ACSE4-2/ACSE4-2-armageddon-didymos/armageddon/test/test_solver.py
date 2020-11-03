
from collections import OrderedDict
import pandas as pd
import numpy as np
from scipy.integrate import odeint
from scipy.integrate import solve_ivp

from pytest import fixture

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
def planet_analytical(armageddon):
    """Return a default planet with a analytical exponential atmosphere"""
    return armageddon.Planet(Cd=1., Ch=0.1, Q=1e7, Cl=0, alpha=0.3, Rp=1e10,
                 g=0, H=8000., rho0=1.2, atmos_func='exponential')


@fixture(scope='module')
def input_data():
    input_data = {'radius': 10.,
              'velocity': 21e3,
              'density': 3000.,
              'strength': 1e5,
              'angle': 45.0,
              'init_altitude':100e3,
              'dt': 0.01,
              'radians': False }
    return input_data

@fixture(scope='module')
def result(planet, input_data):
    """Solve a default impact for the default planet"""

    result = planet.solve_atmospheric_entry(**input_data)

    return result

def test_solve_atmospheric_entry_with_scipy_odeint(planet, result, input_data):
    """Check atmospheric entry solve. 

    Currently only the output type for zero timesteps."""
    
    assert type(result) is pd.DataFrame
    
    for key in ('velocity', 'mass', 'angle', 'altitude',
                'distance', 'radius', 'time'):
        assert key in result.columns

    t = np.arange(0, 1000, input_data['dt'])
    init_state = (input_data['velocity'], input_data['density'] * 4 / 3 * np.pi * (input_data['radius']**3),
                input_data['angle'] * np.pi / 180, input_data['init_altitude'], 0., input_data['radius'])
    P1 = odeint(planet.dmove_odeint, init_state, t, args=([input_data['strength'], input_data['density']],))

    result = planet.solve_atmospheric_entry(input_data['radius'], input_data['velocity'], input_data['density'], input_data['strength'], 
                    input_data['angle'], init_altitude=input_data['init_altitude'], dt=input_data['dt'], radians=input_data['radians'])

    result_df = pd.DataFrame({'velocity': P1[:, 0],
                                'mass': P1[:, 1],
                                'angle': P1[:, 2]*180/np.pi,
                                'altitude': P1[:, 3],
                                'distance': P1[:, 4],
                                'radius': P1[:, 5],
                                'time': t}, index=range(len(P1)))

    len_numerical = min(len(result), len(result_df))
    # check values
    assert np.allclose(result_df.iloc[:len_numerical].velocity.values, result[:len_numerical].velocity.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].mass.values, result[:len_numerical].mass.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].angle.values, result[:len_numerical].angle.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].altitude.values, result[:len_numerical].altitude.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].distance.values, result[:len_numerical].distance.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].radius.values, result[:len_numerical].radius.values, rtol=1e-4)
    assert np.allclose(result_df.iloc[:len_numerical].time.values, result[:len_numerical].time.values, rtol=1e-4)

def test_solve_atmospheric_entry_with_analytic(planet_analytical, result, input_data):
    """Check atmospheric entry solve. 

    Currently only the output type for zero timesteps."""
    
    assert type(result) is pd.DataFrame
    
    for key in ('velocity', 'mass', 'angle', 'altitude',
                'distance', 'radius', 'time'):
        assert key in result.columns

    result = planet_analytical.solve_atmospheric_entry(input_data['radius'], input_data['velocity'], input_data['density'], np.inf, 
                input_data['angle'], init_altitude=input_data['init_altitude'], dt=input_data['dt'], radians=input_data['radians'], analytical=True)

    test_list0 = np.array([input_data['velocity'], 4/3*np.pi*input_data['radius']**3*input_data['density'], input_data['angle'], input_data['radius']]) 
    v_position = planet_analytical.v_z(test_list0, result.altitude.values, radians=input_data['radians'])

    # check velocity - altitude
    assert np.allclose(v_position, result.velocity.values)

    # check de_dz
    dE_dz_position = planet_analytical.dE_dz(test_list0, result.altitude.values)
    result_energy = planet_analytical.calculate_energy(result)

    print(dE_dz_position)
    assert np.allclose(dE_dz_position, result_energy.dedz.values, rtol=1e-01)



