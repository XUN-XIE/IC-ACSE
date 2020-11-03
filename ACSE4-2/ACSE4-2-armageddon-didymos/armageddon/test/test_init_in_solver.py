import numpy as np
from collections import OrderedDict
import pandas as pd

from pytest import fixture

@fixture(scope='module')
def armageddon():
    """Perform the module import"""
    import armageddon
    return armageddon

@fixture(scope='module')
def exponential(armageddon):
    """Return a default planet with a exponential atmosphere"""
    return armageddon.Planet(atmos_func='exponential')

@fixture(scope='module')
def tabular(armageddon):
    """Return a default planet with a tabular atmosphere"""
    return armageddon.Planet(atmos_func='tabular', atmos_filename='data/AltitudeDensityTable.csv')

# something wrong has not been solved, we need to define new variables in mars Planet object!!!!!
@fixture(scope='module')
def mars(armageddon):
    """Return a default planet with a mars atmosphere"""
    return armageddon.Planet(atmos_func='mars',Cd=1.5, Ch=0.1, Q=1e7, Cl=1.24e-6, alpha=0.3,Rp=3376.2e3,g=3.924, H=11.1e3, rho0=0.02)

def test_exponential(exponential):
    """ Test for exponential rhoa"""
    z = 52
    output = 1.1922252951641372
    assert np.allclose(output, exponential.rhoa(z), 1e-8)
    
def test_tabular(tabular):
    """ Test for tabular rhoa"""
    z = 52
    output = 1.2188959101202483
    assert np.allclose(output, tabular.rhoa(z), 1e-8)

def test_mars(mars):
    """ Test for mars rhoa"""
    z = 52
    output = 0.014962894974734409
    assert np.allclose(output, mars.rhoa(z), 1e-8)