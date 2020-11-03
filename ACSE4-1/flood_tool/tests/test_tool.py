"""Test tool.py functions."""

import copy

import numpy as np
from pytest import approx, mark

from flood_tool.tool import *

X = Tool(risk_file = "flood_tool/resources/flood_probability.csv",
    values_file = "flood_tool/resources/property_value.csv",
    postcode_file= "flood_tool/resources/postcodes.csv")

def test_get_easting_northing_flood_probability():
    """Test get_easting_northing_flood_probability."""
    assert(X.get_easting_northing_flood_probability([441107],[406879]) == ['High'])


def test_get_annual_flood_risk():
    assert(X.get_annual_flood_risk(['ME124JS'],['High']) == approx([12525.2068]))

def test_get_flood_cost():
    assert(X.get_flood_cost(["ME124JS"]) == approx([2505041.36]))
    assert(X.get_flood_cost(["PR1 0JL"]) == approx([0]))