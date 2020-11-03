import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from dask import delayed, compute
from multiprocessing import cpu_count

def run_multi(combs, planet, radians, start_index):
    altitudes = []
    for i in range(combs.shape[1]):
        radius, angle, strength, velocity, density = tuple(combs[:, i])
        try:
            result, outcome = planet.impact(radius, velocity, density, strength, angle, radians=radians)
            flag = False
        except Exception:
            # print("NEGATIVE dedz -- %s/%s" % (i, sample_num), tuple(combination[:, i]))
            flag = True
        if not flag and "burst_altitude" in outcome.keys():
            altitudes.append(outcome["burst_altitude"])
        else:
            altitudes.append(0.0)
    return pd.DataFrame({'radius': combs[0],
                         'angle': combs[1], 
                         'strength': combs[2], 
                         'velocity': combs[3],
                         'density': combs[4],
                         'burst_altitude': altitudes}, index=range(start_index, start_index + len(altitudes)))

def read_split(combination, i, comblen, num_cores):
    return combination[:, i*comblen : (i+1)*comblen] if i < num_cores - 1 else combination[:, (num_cores - 1)*comblen : ]

def samplling(sample_num, variables, fiducial_impact):
    combination = np.zeros((5, sample_num))
    if 'radius' in variables:
        combination[0, :] = np.random.uniform(8, 12, sample_num)
    else:
        combination[0, :] = fiducial_impact['radius']

    if 'angle' in variables:
        radians = np.linspace(0, np.pi / 2., sample_num)
        prob = np.abs(- 2 * np.sin(radians) * np.cos(radians))
        # divide the sum of probability to make the sum of which to 1, which is required by next function 'choice'.
        prob /= np.sum(prob)
        combination[1, :] = angle = np.random.choice(np.linspace(0, 90, sample_num), sample_num, p=prob)
    else:
        combination[0, :] = fiducial_impact['angle']

    if 'strength' in variables:
        strength_for_plot = np.random.uniform(3, 7, sample_num)
        combination[2, :] = 10 ** strength_for_plot
    else:
        strength_for_plot = np.log10(fiducial_impact['strength'])
        combination[2, :] = fiducial_impact['strength']

    if 'velocity' in variables:
        v = np.arange(0, 51)
        a = 11.
        prob = (2. / np.pi) ** 0.5 * v ** 2 / a ** 3 * np.exp(- v ** 2 / 2. / a ** 2)
        v_list = np.random.choice(v, sample_num, p=prob/np.sum(prob))
        v_esc = 11.
        combination[3, :] = np.sqrt(v_esc ** 2 + v_list ** 2) * 1e3
    else:
        v_list = combination[3, :] = fiducial_impact['velocity']

    if 'density' in variables:
        combination[4, :] = np.random.normal(3000, 1000, sample_num)
    else:
        combination[4, :] = fiducial_impact['density']

    return combination, strength_for_plot, v_list


def solve_ensemble(
        planet,
        fiducial_impact={
            'radius': 10, 'angle': 45,
            'velocity': 21e3, 'density': 3000.,
            'strength': 1e5
        },
        variables=['radius', 'angle', 'strength', 'velocity', 'density'],
        radians=False,
        rmin=8, rmax=12, visual=False, sample_num=10):
    """
    Run asteroid simulation for a distribution of initial conditions and
    find the burst distribution

    Parameters
    ----------

    planet : object
        The Planet class instance on which to perform the ensemble calculation

    fiducial_impact : dict
        Dictionary of the fiducial values of radius, angle, strength, velocity
        and density

    variables : list
        List of strings of all impact parameters to be varied in the ensemble
        calculation

    rmin : float, optional
        Minimum radius, in m, to use in the ensemble calculation,
        if radius is one of the parameters to be varied.

    rmax : float, optional
        Maximum radius, in m, to use in the ensemble calculation,
        if radius is one of the parameters to be varied.

    Returns
    -------

    ensemble : DataFrame
        DataFrame with columns of any parameters that are varied and the
        airburst altitude
    """

    if "angle" in variables:
        radians = False
    combination, strength_for_plot, v_list = samplling(sample_num, variables, fiducial_impact)
    num_cores = cpu_count()
    comblen = int(combination.shape[1] / num_cores)
    tmp_result = [None] * num_cores
    for i in range(num_cores):
        data = delayed(read_split)(combination, i, comblen, num_cores)
        tmp_result[i] = delayed(run_multi)(data, planet, radians, i*comblen)
    t_result = compute(*tmp_result, scheduler="processes")
    result = pd.concat(t_result)

    if visual:
        fig, axes = plt.subplots(3, 2, figsize=(12, 8))
        plot_distribution(fig, axes, combination, strength_for_plot, v_list)
        altitude = np.array(result.burst_altitude)
        alt_max = np.max(altitude)
        axes[2, 1].hist(altitude)
        axes[2, 1].set_title('airburst altitude')
        axes[2, 1].set_xlabel("altitude (m)")
        plt.show()
    return result

def plot_distribution(fig, axes, combination, strength_for_plot, v_list):
    axes[0, 0].hist(combination[0, :], range=(np.min(combination[0, :]), np.max(combination[0, :])))
    axes[0, 0].set_title('radius')
    axes[0, 0].set_xlabel("r (m)")

    axes[0, 1].hist(combination[1, :], np.arange(0, 91, 3))
    axes[0, 1].set_title('angle')
    axes[0, 1].set_xlabel("deg")

    axes[1, 0].hist(strength_for_plot, np.linspace(3, 7, 100))
    axes[1, 0].set_title('strength')
    axes[1, 0].set_xlabel("log(Y) (Pa)")

    axes[1, 1].hist(v_list, np.arange(0, 50))
    axes[1, 1].set_title('velocity')
    axes[1, 1].set_xlabel("v (km/s)")

    axes[2, 0].hist(combination[4, :], np.arange(0, 6000, 100))
    axes[2, 0].set_title('density')
    axes[2, 0].set_xlabel("density (kg/m3)")
