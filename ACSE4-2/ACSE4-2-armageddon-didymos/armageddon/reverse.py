""" Module to compute meteor radius and strength from energy per km altitude
data. Uses Chelyabinsk data for testing purpose.
"""
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def analyse_data(planet, 
                 rho_m, theta, velocity_entry, datapath,
                 r_tol=1e-2, r_min=1e-2, r_max=4.5e3,
                 Y_tol=1e3, Y_min=1e3, Y_max=1e12,
                 plotting=True, step=False):

    """ Function to fit asteroid parameters (radius r and strength Y) to
    a given dataset.
    
    Parameters
    ----------
    planet: 
        Class 
        
    rho_m: float
        Meteor density in kg/m-3
        
    theta: float
        Meteor entry angle in degrees
        
    velocity_entry: float
        Meteor entry velocity in m/s
        
    datapath: string
        Path to csv file containing event altitude in km in first column and
        event energy per km altitude in the second column in Kt/km
    
    r_tol: float, optional
        Tolerance of radius result. As default, the radius is given to the 
        closest cm (1e-2 m)
    
    r_min: float, optional
        Estimated minimum possible initial radius for the event. As default,
        r_min is taken as default r_tol
    
    r_max: float, optional
        Estimated maximum possible initial radius for the event. As default,
        r_max is taken as the radius of a one per 100Ma event (4500m)
        
    Y_tol: float, optional
        Tolerance of strength result. As default, the strength is given to
        the nearest kPa (1e3 Pa)
        
    Y_min: float, optional
        Estimated minimum possible material strength. As default, Y_min is
        taken as default Y_tol
        
    Y_max: float, optional
        Estimated maximum possible material strength. As default, Y_max is
        taken as the strength of graphine (1e12Pa)
        
    plotting: boolean, optional
        If plotting is 'True', plotting is turned on. By default, plotting is
        'True'.
        
    step: boolean, optional
        If step is 'True' and plotting is 'True', returns a plot including
        the intermediate convergence.
    Returns
    -------
    None
    """
    
    # import data
    data = pd.read_csv(datapath)
    data.columns = ['altitude','dedz']
    if plotting == True:
        fig = plt.figure(figsize=(12,6))
        plt.plot(data.dedz,data.altitude,'k-',label='Dataset from csv file',linewidth=3)
        
    # initialise interval edges
    r_prev = (r_min+r_max)/2
    r_new = r_prev+2*r_tol
    r_prev2 = r_new
    Y_prev = (Y_min+Y_max)/2
    Y = Y_prev
    Y_new = Y_prev+2*Y_tol
    Y_prev2 = Y_new
    
    j = 0
    while ((abs(r_prev-r_new) > r_tol) and (abs(r_prev-r_prev2) > r_tol)) or ((abs(Y_prev-Y_new) > Y_tol) and (abs(Y_prev-Y_prev2) > Y_tol)):
        
        # on each run reset new possible interval 
        if j != 0 :
            r_min = min(r_new-100*r_tol, r_min)
            r_max = max(r_new+100*r_tol, r_max)
            Y_min = min(Y_new-1000*Y_tol, Y_min)
            Y_max = max(Y_new+1000*Y_tol, Y_max)
        
        # update tracking and previous result
        j += 1
        r_prev2 = r_prev
        Y_prev2 = Y_prev
        Y_prev = Y_new
        r_prev = r_new
        
        # converge radius values
        print(j,'th loop through radius')
        for i in range(20): 
            r = (r_min+r_max)/2
            Y = Y_prev
            print('Radius: %.2f m' %r)
            
            result = planet.solve_atmospheric_entry(radius=r, angle=theta, strength=Y, velocity=velocity_entry, density=rho_m)
            result = planet.calculate_energy(result,method='central')
            
            #print('Peak dedz:',result.dedz.max(),'Kt/km, Expected peak dedz:', data.dedz.max(),'Kt/km')
            if result.dedz.max() < data.dedz.max():
                r_min = r
                #print('Radius must increase')
            else: 
                r_max = r
                #print('Radius must decrease')
            r_new = (r_min+r_max)/2
            
            # if met tolerance, exit for loop
            if abs(r_new-r) < r_tol:
                break
        
        # converge strength values
        print(j,'th loop through strength')
        for i in range(20): 
            Y = (Y_min+Y_max)/2
            r = r_new
            print('Strength: %.4g Pa' %Y)
            
            result = planet.solve_atmospheric_entry(radius=r, angle=theta, strength=Y, velocity=velocity_entry, density=rho_m)
            result = planet.calculate_energy(result,method='central')
            
            #print('Peak altitude:',result.altitude[result.dedz.idxmax()]/1e3,'km, Expected altitude:', data.altitude[data.dedz.idxmax()],'km')
            if result.altitude[result.dedz.idxmax()]/1e3 > data.altitude[data.dedz.idxmax()]:
                Y_min = Y
                #print('Strength must increase')
            else: 
                Y_max = Y
                #print('Strength must decrease')
            Y_new = (Y_min+Y_max)/2
            
            # if met tolerance, exit for loop
            if abs(Y_new-Y) < Y_tol:
                break
        if (plotting == True) and (step == True):
            plt.plot(result.dedz,result.altitude/1e3,'--', label='iteration=%.0f: r=%.2fm and Y=%.3f MPa' %(j,r_new,Y_new/1e6),linewidth=3)
        # if more than 20 runs to complete, exit while loop
        if j > 20:
            break
        
    if plotting == True:
        plt.plot(result.dedz,result.altitude/1e3,'r-', label='Fit to dataset',linewidth=3)
        plt.title('Result: r=%.2fm and Y=%.3f MPa' %(r_new,Y_new/1e6),fontsize=20)
        plt.xlabel('Energy per km of altitude, dEdz [Kt/km]',fontsize=16)
        plt.ylabel('Altitude, z [m]',fontsize=16)
        plt.legend(loc='best',fontsize=16)
        plt.axis([data.dedz.min()-5, data.dedz.max()+5, result.altitude.min()/1e3-5, result.altitude.max()/1e3+5])
        plt.show()
        
    print('Radius: %.2f m & Strength: %.3f Pa' %(r_new,Y_new))
