import os
import math
import json

import numpy as np
import pandas as pd

from pytest import approx, fixture, mark

from . import BASE_PATH
from .timing import *

config_file = os.environ.get("SCORE_CONFIG_FILE",
                             os.sep.join((BASE_PATH, "data.json")))

@fixture(scope="module")
def data():
    with open(config_file, "r") as _:
        data = json.load(_)
    return data

@fixture(scope="module")
def testdb(data):
    testdb = pd.read_csv(os.sep.join([BASE_PATH]+data["test data"]))
    return testdb

with open(config_file, "r") as _:
    timeouts = json.load(_)["timeouts"]

@fixture(scope="module")
def flood_tool():
    import flood_tool

    return flood_tool

@mark.timeout(timeouts["get_easting_northing_from_lat_long"])
def test_get_easting_northing_from_lat_long(data, testdb, flood_tool, record_property, record_xml_attribute):

    name = 'get_easting_northing_from_lat_long'

    tol = data[name]['absolute_tolerance']

    output = np.array([[298169, 519487]])
    inputs = [54.560333], [-3.576252]
    time, result = timing(flood_tool.get_easting_northing_from_lat_long,
                          *inputs)

    matches = [r==approx(o, abs=tol) for r, o in zip(np.array([r.ravel() for r in result]).T, output)]
    record_property('single_lookup', (time, matches))

    input_headings = data[name]['input headings']
    output_headings = data[name]['output headings']
    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']

    args = list(testdb.iloc[idx1:idx2][input_headings].to_numpy().T)
    output = testdb.iloc[idx1:idx2][output_headings].to_numpy()
    
    time, result = timing(flood_tool.get_easting_northing_from_lat_long, *args)

    matches = [r == approx(np.array(o),
                           abs=tol) for r, o in zip(np.array(result).T,output)]
    record_property('multiple_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))

@fixture(scope="module")
def timed_tool(data, flood_tool):

    t0 = timer()

    postcode_file = os.sep.join([BASE_PATH]+data["postcode file"])
    risk_file = os.sep.join([BASE_PATH]+data["flood probability file"])
    value_file = os.sep.join([BASE_PATH]+data["property value file"])

    out = flood_tool.Tool(postcode_file,
                                risk_file, 
                                value_file)

    t1 = timer()

    return t1-t0, out


@fixture(scope="module")
def tool(timed_tool):
    return timed_tool[1]

@mark.timeout(timeouts["tool"])
def test_tool(timed_tool, record_property, record_xml_attribute):
    record_property('tool initialization', (timed_tool[0], [True]))
    record_xml_attribute('points', 100)

@mark.timeout(timeouts["get_lat_long"])
def test_get_lat_long(data, testdb, tool,
                      record_property, record_xml_attribute):

    rel = data['get_lat_long']['tolerance']

    output = np.array([[51.379129999999996, 1.3067440000000001]])
    time, result = timing(tool.get_lat_long, ['CT7 9ET'])
    matches = [r==approx(o, rel=rel) for r, o in zip(result, output)]
    record_property('single_postcode_lookup',
                    (time, matches))

    input_headings = data['get_lat_long']['input headings']
    output_headings = data['get_lat_long']['output headings']
    idx1 = data['get_lat_long']['idx1']
    idx2 = data['get_lat_long']['idx2']

    args = list(testdb.iloc[idx1:idx2][input_headings].to_numpy().ravel())
    output = testdb.iloc[idx1:idx2][output_headings].to_numpy()
    
    time, result = timing(tool.get_lat_long, args)

    matches = [r == approx(np.array(o),
                           rel=rel) for r, o in zip(result,output)]
    record_property('multiple_postcode_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data['get_lat_long']))



@mark.timeout(timeouts["get_easting_northing_flood_probability"])
def test_get_easting_northing_flood_probability(data, testdb, tool, record_property, record_xml_attribute):

    name = 'get_easting_northing_flood_probability'

    output = ['Zero']
    inputs = [[298169], [519487]]
    time, result = timing(getattr(tool, name),
                          *inputs)

    matches = [r == o for r, o in zip(result, output)]
    record_property('single_lookup', (time, matches))

    input_headings = data[name]['input headings']
    output_headings = data[name]['output headings']
    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']

    args = list(testdb.iloc[idx1:idx2][input_headings].to_numpy().T)
    output = list(testdb.iloc[idx1:idx2][output_headings].to_numpy().ravel())
    
    time, result = timing(getattr(tool, name), *args)

    matches = [r == o for r, o in zip(result,output)]
    record_property('multiple_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))


@mark.timeout(timeouts["get_sorted_flood_probability"])
def test_get_sorted_flood_probability(data, testdb, tool, record_property, record_xml_attribute):

    name = 'get_sorted_flood_probability'

    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']
    idx3 = data[name]['idx3']
    idx4 = data[name]['idx4']

    args = testdb.iloc[idx1:idx2]['Postcode'].to_numpy().ravel()
    output = testdb.iloc[idx3:idx4][['Postcode', 'Probability Band']]
    output.drop_duplicates(subset='Postcode', inplace=True)
    output.set_index('Postcode', inplace=True)
    
    time, result = timing(getattr(tool, name), args)

    assert result.index.name == 'Postcode'

    matches = list((result.index == output.index) &
                   (result['Probability Band'].to_numpy().ravel()
                    == output['Probability Band'].to_numpy().ravel()))
    record_property('multiple_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))


@mark.timeout(timeouts["get_flood_cost"])
def test_get_flood_cost(data, testdb, tool, 
                        record_property, record_xml_attribute):

    name = 'get_flood_cost'

    rel = data[name]['tolerance']

    output = np.array([4646599.42])
    time, result = timing(getattr(tool, name), ['TN8 6AB'])
    matches = [r==approx(o) for r, o in zip(result, output)]
    record_property('single_postcode_lookup',
                    (time, matches))

    input_headings = data[name]['input headings']
    output_headings = data[name]['output headings']
    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']

    args = list(testdb.iloc[idx1:idx2][input_headings].to_numpy().T)
    output = list(testdb.iloc[idx1:idx2][output_headings].to_numpy().ravel())
    
    time, result = timing(getattr(tool, name), *args)

    matches = [r == approx(o, rel) for r, o in zip(result, output)]
    record_property('multiple_postcode_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))

@mark.timeout(timeouts["get_annual_flood_risk"])
def test_get_annual_flood_risk(data, testdb, tool,
                               record_property, record_xml_attribute):

    name = 'get_annual_flood_risk'

    rel = data[name]['tolerance']

    output = np.array([193.506606])
    time, result = timing(getattr(tool, name), ['DA1 5NU'], ['Very Low'])
    matches = [r==approx(o) for r, o in zip(result, output)]
    record_property('single_postcode_lookup',
                    (time, matches))

    input_headings = data[name]['input headings']
    output_headings = data[name]['output headings']
    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']

    args = list(testdb.iloc[idx1:idx2][input_headings].to_numpy().T)
    output = testdb.iloc[idx1:idx2][output_headings].to_numpy().ravel()
    
    time, result = timing(getattr(tool, name), *args)

    matches = [r == approx(o, rel) for r, o in zip(result,output)]
    record_property('multiple_postcode_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))
  
@mark.timeout(timeouts["get_sorted_annual_flood_risk"])
def test_get_sorted_annual_flood_risk(data, testdb, tool,
                               record_property, record_xml_attribute):

    name = 'get_sorted_annual_flood_risk'

    rel = data[name]['tolerance']

    idx1 = data[name]['idx1']
    idx2 = data[name]['idx2']
    idx3 = data[name]['idx3']
    idx4 = data[name]['idx4']

    args = testdb.iloc[idx1:idx2][['Postcode']].to_numpy().ravel()
    output = testdb.iloc[idx3:idx4][['Postcode', 'Flood Risk']]
    output.drop_duplicates(subset='Postcode', inplace=True)
    output.set_index('Postcode', inplace=True)
    
    time, result = timing(getattr(tool, name), args)

    assert result.index.name == 'Postcode'

    matches = list((result.index == output.index) &
                   (result['Flood Risk'].to_numpy().ravel()
                    == approx(output['Flood Risk'].to_numpy().ravel(), rel)))

    record_property('multiple_postcode_lookup', (time, matches))

    record_xml_attribute('points', calculate_score(time, matches,
                                                   data[name]))




