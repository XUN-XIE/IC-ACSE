import scipy.interpolate as si
import numpy as np
import pandas as pd
import solver as sl
import matplotlib.pyplot as plt

def calculate_mse(radius, strength, basic_z, basic_ek, err_method='distance'):
    '''
    calculate the error function of fit model to the original model
    
    Parameters
    ----------
    
    radius : float
        asteroid radius
    
    strength : float
        asteroid strength
    
    Returns
    -------
    mse: float
        square distance between burst points of fit model and raw data
    '''
    limit = 1
    
    com_z, com_ek = fit_out(radius, strength)
    if com_z is None:
        return np.inf
    
    com_DF = pd.DataFrame({'com_z':com_z, 'com_ek':com_ek})

#     basic_DF = pd.read_csv('../data/ChelyabinskEnergyAltitude.csv')
#     basic_DF = basic_DF.loc[basic_DF.iloc[:,1]>limit]
#     basic_z, basic_ek = basic_DF.iloc[:,0].to_numpy(), basic_DF.iloc[:,1].to_numpy()
    
    
    if err_method == 'mse':
        upper = np.max(basic_z)
        lower = np.min(basic_z)

        # add the points in fit model that are blank between range of altitude in original model
        if np.max(com_z) < upper:
            add_max = upper
            add_min = np.max(com_z)
            number = int(add_max - add_min)+1
            zeros = np.zeros(number)
            add_z = np.linspace(add_max-0.05, add_min+0.05, number)

            add_DF = pd.DataFrame({'com_z':add_z, 'com_ek':zeros})
            com_DF = add_DF.append(com_DF, ignore_index=True, sort=False)

        if np.min(com_z) > lower:
            add_max = np.min(com_z)
            add_min = lower
            number = int(add_max - add_min)+1
            zeros = np.zeros(number)
            add_z = np.linspace(add_min+0.05, add_max-0.05, number)

            add_DF = pd.DataFrame({'com_z':add_z, 'com_ek':zeros})
            com_DF = com_DF.append(add_DF, ignore_index=True, sort=False)

        # slice the points that are outside the range of altitude in original model
        # which is used for later interpolation
        com_DF = com_DF.loc[com_DF.iloc[:,0] < upper]
        com_DF = com_DF.loc[com_DF.iloc[:,0] > lower]
        com_z = com_DF.iloc[:,0].to_numpy()
        com_ek = com_DF.iloc[:,1].to_numpy()

        lp = si.interp1d(basic_z, basic_ek)
        interpolate_ek = lp(com_z)
        diff_ek = com_ek - interpolate_ek
#         plt.plot(com_ek, com_z)
        mse = np.linalg.norm(diff_ek)/np.sqrt(diff_ek.shape)
    
    if err_method == 'distance':
        basic_burst_ek = np.max(basic_ek)
        basic_burst_z = basic_z[np.where(basic_ek==basic_burst_ek)]
        com_burst_ek = com_ek.max()
        com_burst_z = com_z[np.where(com_ek==com_burst_ek)]

        distance_burst = (com_burst_z - basic_burst_z)**2 + (com_burst_ek-basic_burst_ek)**2

        mse = distance_burst
    
    return mse

def fit_out(radius, strength):
    """
    calculate altitude and energy applying solver
    
    Parameters
    ----------
    
    radius: float
        ateroid radius
    
    strength: float
        ateroid strength
    
    Returns
    -------
    
    com_z: array
        the altitude sliced to range after brea-up
    
    com_ek: array
        energy per heiight sliced to range after brea-up
    """
    
    earth = sl.Planet()
    out = earth.solve_atmospheric_entry(
        radius=radius, angle=18.3, strength=strength, velocity=19.2e3, density=3300)
    out = earth.calculate_energy(out)

    altitude = out.iloc[:,3]
    energy = out.iloc[:,-1]

    v_a = pd.DataFrame({'z':altitude, 'ek':energy})
    valid = v_a.loc[v_a.iloc[:,-1] > 1]

    if valid.size == 0:
        return None, None

    com_z = valid.iloc[:,0].to_numpy()/1e3
    com_ek = valid.iloc[:,1].to_numpy()
    
    return com_z, com_ek

def output(mse_arr, radius, strength):
    """
    calculate altitude and energy applying solver
    
    Parameters
    ----------
    
    radius: array
        test array of ateroid radius
    
    strength: array
        test array of ateroid strength
    
    Returns
    -------
    
    None
    """
    
    min_mse = np.min(mse_arr)
    min_index = np.argmin(mse_arr)
    r_best = radius[min_index%10]
    str_best = strength[min_index//10]
    com_z, com_ek = fit_out(r_best, str_best)

    
    plt.plot(com_ek, com_z, label='fit model')
    plt.xlabel('Energy per unit height (Kt km$^{-1}$)')
    plt.ylabel('Altitude (m)')
    plt.legend()
    print('best radius = %.3f' %r_best)
    print('best strength = %.3e' %str_best)
    print('miniimum error = %.3f' %min_mse)