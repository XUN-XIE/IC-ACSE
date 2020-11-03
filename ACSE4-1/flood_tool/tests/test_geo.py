"""Test geodetic transformations module."""

import copy

import numpy as np
from pytest import approx, mark

import flood_tool.geo as geo

def test_rad():
    """Test rad function"""
    assert geo.rad(90) == approx(np.pi/2.)
    assert geo.rad(4, 30) == approx(np.pi/40.)
    assert geo.rad(1, 7, 30) == approx(np.pi/160.)
    assert geo.rad(np.array([90, 45])) == approx(np.array((np.pi/2.,
                                                            np.pi/4.)))

def test_deg():
    """Test deg function"""
    assert geo.deg(np.pi/2.) == approx(90.0)
    assert geo.deg(np.pi/40.) == approx(4.5)
    assert geo.deg(np.pi/40., True) == approx((4, 30, 0))

def test_lat_long_to_xyz():
    """Test lat_long_to_xyz function."""
    latitude = np.array([geo.rad(52,39,27.2531)])
    longitude = np.array([geo.rad(1,43, 4.5177)])
    xyz = np.array([[3874938.849], [116218.624], [5047168.208]])

    datum = copy.copy(geo.osgb36)
    datum.F_0=1.0

    assert geo.lat_long_to_xyz(latitude, longitude, True,
                               datum = datum) == approx(xyz, rel=1.0e-8)

def test_xyz_to_lat_long():
    """Test xyz_to_lat_long function."""

    x = np.array([3874938.849])
    y = np.array([116218.624])
    z = np.array([5047168.208])

    latitude = np.array([geo.rad(52, 39, 27.2531)])
    longitude = np.array([geo.rad(1, 43, 4.5177)])

    assert np.array(geo.xyz_to_lat_long(x, y, z, True)) \
            == approx(np.array((latitude, longitude)), rel=1.0e-5)

def test_WGS84toOSGB36transform():
    """Test WGS84toOSGB36transform."""
    xyz_wgs = np.array([[3875269.71073],
                        [ 116102.03456],
                        [5047546.87963]])
    xyz_os = np.array([[3874897.094],
                       [116217.425],
                       [5047112.797]])
    assert geo.WGS84toOSGB36transform(xyz_wgs) == approx(xyz_os, rel=1.0e-5)

def test_WGS84toOSGB36():
    """Test WGS84toOSGB36."""
    lat_long_wgs = np.array([[geo.rad(52, 39, 28.71)],
                             [geo.rad(1, 42, 57.79)]])
    lat_long_os = np.array([[geo.rad(52, 39, 27.2531)],
                            [geo.rad(1, 43, 4.5177)]])

    assert np.array(geo.WGS84toOSGB36(*lat_long_wgs,
                                       True)) == approx(lat_long_os,
                                                        rel=1.0e-5)

@mark.xfail()
def test_get_easting_northing_from_lat_long():
    """Test lat_long_to_easting_northing."""
    latitude = np.array([geo.rad(52,39,27.2531)])
    longitude = np.array([geo.rad(1,43,4.5177)])

    e = 651409.903 
    n = 313177.270

    assert np.array(geo.get_easting_northing_from_lat_long(latitude,
                                                           longitude)) \
                                     == approx(np.array((e,n)),
                                               rel=1.0e-5)
