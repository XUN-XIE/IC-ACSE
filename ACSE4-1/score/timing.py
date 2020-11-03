import xml.etree.ElementTree as etree
import json
import os
from time import perf_counter as timer

import numpy as np

from . import BASE_PATH

__all__ = ['timing', 'calculate_score', 'process_results', 'timer']

def timing(func, *args, repeat=5, **kwargs):
    import timeit

    tmp_globals = {'func':func, 'args':args}

    stmt = "out = func(*args)"
    timer = timeit.Timer(stmt, globals=tmp_globals)
    time = timer.repeat(repeat, number=1)

    return np.min(time), func(*args)

def calculate_score(time, matches, test_data):

    if 't0' in test_data:
        return int(test_data['score']*accuracy_weighting(matches)
                   *time_weighting(test_data['t0'],
                                   test_data['t1'],
                                   time))
    else:
        return int(test_data['score']*accuracy_weighting(matches))
                                

def accuracy_weighting(matches):
    return np.array(matches).sum()/len(matches)

def time_weighting(t0,t1,t):
    return (1.0+np.tanh(np.log(t0/t)/t1))/2.0


def process_results(filename, outfile=None):

    def passed(ele):
        p = 'failure' not in {_.tag for _ in ele.getchildren()}
        p = p and 'error' not in {_.tag for _ in ele.getchildren()}

        return p

    def times(ele):
        try:
            return {e.attrib['name']:e.attrib['value']
                    for e in ele.find('properties')}
        except:
            return None

    def score(ele):
        if 'points' in ele.attrib:
            return int(ele.attrib['points'])
        else:
            return 0

    config_file = os.environ.get("SCORE_CONFIG_FILE",
                                 os.sep.join((BASE_PATH, "data.json")))

    with open(config_file, "r") as _:
        timeouts = json.load(_)["timeouts"]
    
    output = {}

    with open(filename, "r") as results:
        tree = etree.fromstring(results.read())

    if tree.tag == "testsuites":
        tree = tree[0]

    for ele in tree:
        output[ele.attrib["name"]] = (passed(ele), times(ele), score(ele))

    total = 0
    total_effective_time = 0.0

    save ={}

    for name, (success, children, points) in output.items():
        total += points

        effective_time = 0.0

        name = name[5:]
        print("%s: %s, %s points"%(name, ("test errored", "test ran")[success], points))
        save[name] = {'ran': success, 'score':points}
        if not success:
            save["effective_time"] = timeouts[name]
            print("\t\teffective time %0.3e seconds"%timeouts[name])
            total_effective_time += timeouts[name]
        if children is None:
            continue
        for subname, val in children.items():
            jsubname = subname
            subname = subname.replace('_', ' ')
            try:
                t, result= eval(val)
            except:
                print("\t%s: test error"%subname)
                success = False
            save[name][jsubname] = {'result': [bool(r) for r in result]}
            if t:
                save[name][jsubname]['time']=t
                effective_time += t
                print("\t%s: time %0.3e, %s of %s"%(subname, t, 
                                                    np.array(result).sum(),
                                                    len(result)))
                if np.array(result).sum()/len(result)<0.5:
                    success = False

        if not success:
            effective_time = timeouts[name]

        print("\t\teffective time %0.3e seconds"%effective_time)
        save[name]["effective_time"] = effective_time
        total_effective_time += effective_time

    if outfile:
        with open(outfile, 'w') as _:
            json.dump(save, _, indent=2)

    print("total: %s, effective time %0.3e seconds"%(total,
                                                     total_effective_time))
