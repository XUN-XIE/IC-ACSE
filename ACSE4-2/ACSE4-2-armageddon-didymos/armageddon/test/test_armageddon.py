
from collections import OrderedDict
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


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
def input_data():
    input_data = {'radius': 1.,
                  'velocity': 1.0e5,
                  'density': 3000.,
                  'strength': 1e32,
                  'angle': 30.0,
                  'init_altitude':100e3,
                  'dt': 0.05,
                  'radians': False 
                 }
    input_data = {'radius': 10.,
            'velocity': 21e3,
            'density': 3000.,
            'strength': 1e5,
            'angle': 45.0,
            'init_altitude':100e3,
            'dt': 0.05,
            'radians': False }
    return input_data

@fixture(scope='module')
def result(planet, input_data):
    """Solve a default impact for the default planet"""

    result = planet.solve_atmospheric_entry(**input_data)

    return result

def test_import(armageddon):
    """Check package imports"""
    assert armageddon

def test_planet_signature(armageddon):
    """Check planet accepts specified inputs"""
    inputs = OrderedDict(atmos_func='constant',
                         atmos_filename=None,
                         Cd=1., Ch=0.1, Q=1e7, Cl=1e-3,
                         alpha=0.3, Rp=6371e3,
                         g=9.81, H=8000., rho0=1.2)

    # call by keyword
    planet = armageddon.Planet(**inputs)

    # call by position
    planet = armageddon.Planet(*inputs.values())

def test_attributes(planet):
    """Check planet has specified attributes."""
    for key in ('Cd', 'Ch', 'Q', 'Cl',
                'alpha', 'Rp', 'g', 'H', 'rho0'):
        assert hasattr(planet, key)

def test_solve_atmospheric_entry(result, input_data):
    """Check atmospheric entry solve. 

    Currently only the output type for zero timesteps."""
    
    assert type(result) is pd.DataFrame
    
    for key in ('velocity', 'mass', 'angle', 'altitude',
                'distance', 'radius', 'time'):
        assert key in result.columns

    assert result.velocity.iloc[0] == input_data['velocity']
    assert np.isclose(result.angle.iloc[0], input_data['angle'])
    assert result.altitude.iloc[0] == input_data['init_altitude']
    assert result.distance.iloc[0] == 0.0
    assert result.radius.iloc[0] == input_data['radius']
    assert result.time.iloc[0] == 0.0

def test_calculate_energy(planet, result):
    """Check energy calculation. 

    Currently only the output type for zero timesteps."""

    energy = planet.calculate_energy(result.copy())

    assert type(energy) is pd.DataFrame
    
    for key in ('velocity', 'mass', 'angle', 'altitude',
                'distance', 'radius', 'time', 'dedz'):
        assert key in energy.columns

def test_analyse_outcome(planet, result):
    """Check outcome analysis. 

    Currently only the output type for zero timesteps."""
    result = planet.calculate_energy(result.copy())
    outcome = planet.analyse_outcome(result)
    assert type(outcome) is dict 


def test_1_CRATERING(planet):
    # making a pd dataframe, where the peak dedz occurs at an altitude that should give an outcome of cratering only.
    test_data1 = {'time': [0, 0.05, 0.10, 0.15], 'velocity': [1, 2, 3, 0.7], 'mass': [100, 150, 300, 20], 'angle' : [24, 34, 81, 87.2], 'altitude': [30000,45000,-2000., 0], 'horizontal distance': [2,5,5.3,5.55], 'radius': [2.5,0.19,0.20, 0.14], 'dedz': [200,142,495.23, 103.56]}
    test_result1 = pd.DataFrame(data=test_data1)
    outcome_1 = planet.analyse_outcome(test_result1)
    #would expect the dictionary to have a length of 4. 
    assert len(outcome_1) == 4

    #would expect the dictionary to give the result (manually calculated)
    #can't test for exact impact time, but can check if it is within a tolerance of 5s.
    expected_dict_1 = {'impact_mass': 20, 'impact_speed': 0.7, 'impact_time': 5, 'outcome': 'Cratering'}
    impact_time_test = np.isclose([outcome_1["impact_time"]], expected_dict_1["impact_time"], atol=5)
    assert impact_time_test == np.array([True])
    assert outcome_1['impact_mass'] == expected_dict_1['impact_mass']
    assert outcome_1['impact_speed'] == expected_dict_1['impact_speed']
    assert outcome_1['outcome'] == expected_dict_1['outcome']


def test_2_AIRBURST(planet):
    # making a pd dataframe, where the peak dedz occurs at an altitude that should give an outcome of airbursting only.
    test_data2 = {'time': [0, 0.05, 0.10, 0.15, 0.20], 'velocity': [1, 2, 3, 0.7, 0.6], 'mass': [100, 150, 300, 20, 17], 'angle' : [24, 34, 81, 87.2, 88], 'altitude': [45000, 40000, 35000, 30000, 25000], 'horizontal distance': [2,5,5.3,5.55,2.40], 'radius': [2.5,0.19,0.20, 0.14, 0.12], 'dedz': [127,142, 167.02, 200.34, 197.5]}
    test_result2 = pd.DataFrame(data=test_data2)
    outcome_2 = planet.analyse_outcome(test_result2)
    #would expect the dictionary to have a length of 4. 
    print(outcome_2)
    assert len(outcome_2) == 4

    #would expect the dictionary to give the result (manually calculated)
    expected_dict_2 = {'burst_peak_dedz': 200.34, 'burst_altitude': 30000, 'burst_total_ke_lost': 45.1/(4.184*10**12), 'outcome': 'Airburst'}
    assert outcome_2 == expected_dict_2

def test_3_AIRBURST_and_CRATERING(planet):
    # making a pd dataframe, where the peak dedz occurs at an altitude that should give an outcome of airburst and cratering.
    test_data3 = {'time': [0, 0.05, 0.10, 0.15, 0.20, 0.25], 'velocity': [1, 2, 3, 0.7, 0.6, 0.3], 'mass': [100, 150, 300, 20, 17, 0.02], 'angle' : [24, 34, 81, 87.2, 88, 88.01], 'altitude': [30000, 20000, 1599, 1230, 823, 0.0], 'horizontal distance': [2,5,5.3,5.55,5.56, 5.561], 'radius': [2.5,0.19,1.34, 0.14, 0.12, 0.11], 'dedz': [12,142, 500.92, 200.34, 197.5, 180]}
    test_result3 = pd.DataFrame(data=test_data3)
    outcome_3 = planet.analyse_outcome(test_result3)
    #would expect the dictionary to have a length of 7. 
    assert len(outcome_3) == 7

    #would expect the dictionary to give the result
    expected_dict_3 = {'burst_peak_dedz': 500.92, 'burst_altitude': 1.599, 'burst_total_ke_lost': -1300.0, 'impact_time': 5, 'impact_mass': 0.02, 'impact_speed': 0.3, 'outcome': 'Airburst and cratering'}
    impact_time_test = np.isclose([outcome_3["impact_time"]], expected_dict_3["impact_time"], atol=5)
    assert impact_time_test == np.array([True])
    assert outcome_3['impact_mass'] == expected_dict_3['impact_mass']
    assert outcome_3['impact_speed'] == expected_dict_3['impact_speed']
    assert outcome_3['outcome'] == expected_dict_3['outcome']

def test_4_no_zero_altitude_value(planet):
    # making a pd dataframe, where the peak dedz occurs at an altitude that should give an outcome of airburst and cratering 
    # and contains no altitude = 0 value (testing the df_sort variable).
    test_data4 = {'time': [0, 0.05, 0.10, 0.15, 0.20, 0.25], 'velocity': [1, 2, 3, 0.7, 0.6, 0.3], 'mass': [100, 150, 300, 20, 17, 0.02], 'angle' : [24, 34, 81, 87.2, 88, 88.01], 'altitude': [30000, 20000, 1599, 1230, 823, 0.09], 'horizontal distance': [2,5,5.3,5.55,5.56, 5.561], 'radius': [2.5,0.19,1.34, 0.14, 0.12, 0.11], 'dedz': [12,142, 500.92, 200.34, 197.5, 180]}
    test_result4 = pd.DataFrame(data=test_data4)
    outcome_4 = planet.analyse_outcome(test_result4)
    #would expect the dictionary to have a length of 7. 
    assert len(outcome_4) == 7

    #would expect the dictionary to give the result
    expected_dict_4 = {'burst_peak_dedz': 500.92, 'burst_altitude': 1.599, 'burst_total_ke_lost': -1300.0, 'impact_time': 5, 'impact_mass': 0.02, 'impact_speed': 0.3, 'outcome': 'Airburst and cratering'}
    impact_time_test = np.isclose([outcome_4["impact_time"]], expected_dict_4["impact_time"], atol=5)
    assert impact_time_test == np.array([True])
    assert outcome_4['impact_mass'] == expected_dict_4['impact_mass']
    assert outcome_4['impact_speed'] == expected_dict_4['impact_speed']
    assert outcome_4['outcome'] == expected_dict_4['outcome']

# def test_ensemble(planet, armageddon):
#     """ Check Ensemble. """
#     fiducial_impact = {'radius': 0.0,
#                        'angle': 0.0,
#                        'strength': 0.0,
#                        'velocity': 0.0,
#                        'density': 0.0}
    
#     ensemble = armageddon.ensemble.solve_ensemble(planet,
#                                                   fiducial_impact,
#                                                   variables=[], radians=False,
#                                                   rmin=8, rmax=12)

#     assert 'burst_altitude' in ensemble.columns
    

