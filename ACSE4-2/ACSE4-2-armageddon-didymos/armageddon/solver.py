import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def mars(z):
    """
    Atmosphere density calculation on Mars.

    Parameters
    ----------

    z : float
        The altitude.

    Returns
    -------

    Result : float
        The atmosphere density at input altitude on Mars.

    """
    p = 0.699 * np.exp(-9e-5 * z)
    if z >= 7e3:
        T = 249.7 - 2.22e-3 * z
    else:
        T = 242.1 - 9.98e-4 * z
    return p / 1.921e-1 / T


class Planet():
    """
    The class called Planet is initialised with constants appropriate
    for the given target planet, including the atmospheric density profile
    and other constants
    """

    def __init__(self, atmos_func='exponential', atmos_filename='data/AltitudeDensityTable.csv',
                 Cd=1., Ch=0.1, Q=1e7, Cl=1e-3, alpha=0.3, Rp=6371e3,
                 g=9.81, H=8000., rho0=1.2):
        """
        Set up the initial parameters and constants for the target planet

        Parameters
        ----------

        atmos_func : string, optional
            Function which computes atmospheric density, rho, at altitude, z.
            Default is the exponential function ``rho = rho0 exp(-z/H)``.
            Options are ``exponential``, ``tabular``, ``constant`` and ``mars``

        atmos_filename : string, optional
            If ``atmos_func`` = ``'tabular'``, then set the filename of the table
            to be read in here.

        Cd : float, optional
            The drag coefficient

        Ch : float, optional
            The heat transfer coefficient

        Q : float, optional
            The heat of ablation (J/kg)

        Cl : float, optional
            Lift coefficient

        alpha : float, optional
            Dispersion coefficient

        Rp : float, optional
            Planet radius (m)

        rho0 : float, optional
            Air density at zero altitude (kg/m^3)

        g : float, optional
            Surface gravity (m/s^2)

        H : float, optional
            Atmospheric scale height (m)

        Returns
        -------

        None
        """

        # Input constants
        self.Cd = Cd
        self.Ch = Ch
        self.Q = Q
        self.Cl = Cl
        self.alpha = alpha
        self.Rp = Rp
        self.g = g
        self.H = H
        self.rho0 = rho0
        self.tabular_dict = {}

        if atmos_func == 'exponential':
            self.rhoa = lambda z: self.rho0 * np.exp(-z / self.H)
        elif atmos_func == 'tabular':
            DF = pd.read_csv(atmos_filename, sep=' ', skiprows=6, names=['Altitude', 'Atmospheric_density', 'H'])
            self.z_zero_rho = float(DF.iloc[0].Atmospheric_density)
            self.z_max = float(DF.iloc[-1].Altitude)
            self.z_max_rho = float(DF.iloc[-1].Atmospheric_density)
            for i in range(len(DF)):
                self.tabular_dict[int(DF.iloc[i].Altitude)] = (float(DF.iloc[i].Atmospheric_density), float(DF.iloc[i].H))
            self.rhoa = lambda z: self.z_zero_rho if int(z) <= 0 else self.tabular_dict[int(int(z/10)*10)][0] * np.exp((int(int(z/10)*10)-z)/self.tabular_dict[int(int(z/10)*10)][1]) if int(z) < self.z_max else self.z_max_rho
        elif atmos_func == 'mars':
            self.rhoa = mars
        elif atmos_func == 'constant':
            self.rhoa = lambda x: rho0
        else:
            raise NotImplementedError

    def impact(self, radius, velocity, density, strength, angle,
               init_altitude=100e3, dt=0.05, radians=False, plotting=False):
        """
        Solve the system of differential equations for a given impact event.
        Also calculates the kinetic energy lost per unit altitude and
        analyses the result to determine the outcome of the impact.

        Parameters
        ----------

        radius : float
            The radius of the asteroid in meters

        velocity : float
            The entery speed of the asteroid in meters/second

        density : float
            The density of the asteroid in kg/m^3

        strength : float
            The strength of the asteroid (i.e., the ram pressure above which
            fragmentation and spreading occurs) in N/m^2 (Pa)

        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        init_altitude : float, optional
            Initial altitude in m

        dt : float, optional
            The output timestep, in s

        radians : logical, optional
            Whether angles should be given in degrees or radians. Default=False
            Angles returned in the DataFrame will have the same units as the
            input

        plotting : logical, optional
            Allows the user to choose whether to plot the results graphically or not.
            Default is set to False (no plotting).

        Returns
        -------

        Result : DataFrame
            A pandas DataFrame containing the solution to the system.
            Includes the following columns:
            ``velocity``, ``mass``, ``angle``, ``altitude``,
            ``distance``, ``radius``, ``time``, ``dedz``

        outcome : Dict
            dictionary with details of airburst and/or cratering event.
            For an airburst, this will contain the following keys:
            ``burst_peak_dedz``, ``burst_altitude``, ``burst_total_ke_lost``.

            For a cratering event, this will contain the following keys:
            ``impact_time``, ``impact_mass``, ``impact_speed``.

            All events should also contain an entry with the key ``outcome``,
            which should contain one of the following strings:
            ``Airburst``, ``Cratering`` or ``Airburst and cratering``
        """
        result = self.solve_atmospheric_entry(radius, velocity, density, strength, angle,
                                init_altitude=init_altitude, dt=dt, radians=radians)
        result = self.calculate_energy(result)
        outcome = self.analyse_outcome(result, strength=strength, plotting=plotting)
        return result, outcome

    def solve_atmospheric_entry(
            self, radius, velocity, density, strength, angle,
            init_altitude=100e3, dt=0.05, radians=False, analytical=False):
        """
        Solve the system of differential equations for a given impact scenario

        Parameters
        ----------

        radius : float
            The radius of the asteroid in meters

        velocity : float
            The entery speed of the asteroid in meters/second

        density : float
            The density of the asteroid in kg/m^3

        strength : float
            The strength of the asteroid (i.e., the ram pressure above which
            fragmentation and spreading occurs) in N/m^2 (Pa)

        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        init_altitude : float, optional
            Initial altitude in m

        dt : float, optional
            The output timestep, in s

        radians : logical, optional
            Whether angles should be given in degrees or radians. Default=False
            Angles returned in the DataFrame will have the same units as the
            input

        Returns
        -------
        Result : DataFrame
            A pandas DataFrame containing the solution to the system.
            Includes the following columns:
            ``velocity``, ``mass``, ``angle``, ``altitude``,
            ``distance``, ``radius``, ``time``
        """

        # Enter your code here to solve the differential equations

        if radians == False:
            angle = self.deg_to_rad(angle)
        
        u0 = np.array([velocity, density*4/3*np.pi*radius**3, angle, init_altitude, 0, radius])
        if not analytical:
            uall, tall = self.RK4(self.f_msd, u0, 0, 1e5, dt, strength, density)
        else:
            uall, tall = self.RK4(self.f_msd_analytical, u0, 0, 5e3, dt, strength, density)

        return pd.DataFrame({'velocity': uall[:, 0],
                             'mass': uall[:, 1],
                             'angle': uall[:, 2]*180/np.pi,
                             'altitude': uall[:, 3],
                             'distance': uall[:, 4],
                             'radius': uall[:, 5],
                             'time': tall}, index=range(uall.shape[0]))

    def calculate_energy(self, result, method='central'):
        """
        Function to calculate the kinetic energy lost per unit altitude in
        kilotons TNT per km, for a given solution.

        Parameters
        ----------

        result : DataFrame
            A pandas DataFrame with columns for the velocity, mass, angle,
            altitude, horizontal distance and radius as a function of time
            
        method : string
            Method to be used for numerical differenciation. Choice in:
            'forward', 'backward', 'central', 'undertermined_coeff' and
            'analytical'.
            By default, analytical calculations are used.

        Returns
        -------

        Result : DataFrame
            Returns the DataFrame with additional column ``dedz`` which is the
            kinetic energy lost per unit altitude
        """

        # remove repeat altitudes -> not physical
        if result.altitude.nunique() != len(result.altitude):
            result = result.drop_duplicates(subset='altitude', keep="first")
            result = result.reset_index()

        result.insert(len(result.columns), 'dedz', np.array(np.nan))
        result.insert(len(result.columns), 'ke', np.array(np.nan))

        # compute kinetic energy at each timestep
        result.ke = 1/2 * result.mass * result.velocity**2

        assert method in ['forward','backward','central','undetermined_coeff','analytical']
        
        if method == 'forward':
            # use forward euler scheme
            result.dedz = result.ke.diff(-1)/result.altitude.diff(-1)
        if method == 'backward':
            # use backward euler scheme
            result.dedz = result.ke.diff()/result.altitude.diff()
        elif method == 'central':
            # central difference scheme 
            result.dedz = result.ke.diff(2)/result.altitude.diff(2)
            result.dedz = result.dedz.shift(-1)
        elif method == 'undetermined_coeff':
            # use method of undetermined coefficients
            result.insert(len(result.columns), 'dz_0', np.array(np.nan))
            result.insert(len(result.columns), 'dz_2', np.array(np.nan))
            result.dz_0 = result.altitude.diff()
            result.dz_2 = result.altitude.diff(-1)
            result.dedz = -(result.dz_2/result.dz_0*(1/(result.dz_0-result.dz_2)) * result.ke.diff() \
                        - (result.dz_2**2+result.dz_0**2)/(result.dz_0*result.dz_2)*(1/(result.dz_0-result.dz_2)) * result.ke \
                        + result.dz_0/result.dz_2*(1/(result.dz_0-result.dz_2)) * result.ke.diff(-1))
            del result['dz_0']
            del result['dz_2']
        elif method == 'analytical':
            # use analytical method derived 
            result.dedz = - (1/2*result.velocity**2) / (- result.velocity * np.sin(result.angle)) \
                        * (- self.Ch*self.rhoa(result.altitude)*np.pi*result.radius**2*result.velocity**3/(2*self.Q)) \
                        - (result.mass * result.velocity) / (- result.velocity * np.sin(result.angle)) \
                        * (- self.Cd*self.rhoa(result.altitude)*np.pi*result.radius**2*result.velocity**2/(2*result.mass) + self.g * np.sin(result.angle)) 
        
        if method != 'analytical':               
            # add Euler values at end points
            result.dedz[0] = result.ke.diff(-1)[0]/result.altitude.diff(-1)[0]
            result.dedz.iloc[-1:] = result.ke.diff(1).tail(1)/result.altitude.diff(1).tail(1)

        # unit change from J/km to kt(TNT)/km
        result.dedz = result.dedz/(4.184e9)

        del result['ke']

        return result

    def analyse_outcome(self, result, strength=5e3, plotting=False):
            """
            Inspect a prefound solution to calculate the impact and airburst stats

            Parameters
            ----------

            result : DataFrame
                pandas DataFrame with velocity, mass, angle, altitude, horizontal
                distance, radius and dedz as a function of time

            strength : float
                Specified by user when calling the impact function.
                Here it is used in the calcuations of the break-up altitude

            plotting : logical, optional
                Allows the user to choose whether to graphically display the results.
                Default is set to False (no plotting).

            Returns
            -------

            outcome : Dict
                dictionary with details of airburst and/or cratering event.
                For an airburst, this will contain the following keys:
                ``burst_peak_dedz``, ``burst_altitude``, ``burst_total_ke_lost``.

                For a cratering event, this will contain the following keys:
                ``impact_time``, ``impact_mass``, ``impact_speed``.

                All events should also contain an entry with the key ``outcome``,
                which should contain one of the following strings:
                ``Airburst``, ``Cratering`` or ``Airburst and cratering``
            """
            #setting up dictionary called 'outcome'
            outcome = {}

            # finding the burst peak dedz (maximum energy)
            # filtering by positive mass values only first
            burst_peak_dedz = result[result.mass >= 0].loc[:, "dedz"].max()

            # raising an error for any negative dedz values
            if burst_peak_dedz < 0:
                raise ValueError #burst_peak_dedz must be positive.
            
            # finding the burst altitude corresponding to the peak dedz
            # this is done by finding which row contains the burst peak dedz in the dataframe and selecting the corresponding altitude
            burst_altitude = float(result.loc[result.dedz == burst_peak_dedz]['altitude'].values)
            # raises type error when this returns an empty frame

            # want to find the break-up altitude for plotting.
            # This begins when density_atmosphere * velocity**2 = strength.
            breakup_df = result.copy()
            breakup_df["Atmospheric_density"] = breakup_df.altitude.apply(lambda x : self.rhoa(x))
            breakup_df["Calculation"] = breakup_df.Atmospheric_density * (breakup_df.velocity)**2

            # takes the first instance where the calculation equals or exceeds the strength
            if breakup_df[breakup_df.Calculation >= strength].size != 0:
                break_up_altitude_data = breakup_df[breakup_df.Calculation >= strength].iloc[0]
                break_up_alt = float(break_up_altitude_data.altitude)

            else: #for the cases where there is no break-up, the break_up_alt is returned as Nan.
                break_up_alt = np.NaN
            
            #print(breakup_df)
            breakup_df["diff_in_vel"] = np.abs(breakup_df["velocity"] - breakup_df["velocity"].shift(1))
            max_change_vel = breakup_df.diff_in_vel.max()
            max_change_vel_alt = float(breakup_df.loc[breakup_df.diff_in_vel == max_change_vel]['altitude'].values)

            # finding the burst total kinetic energy lost up until the burst point.
            if burst_altitude > 5000.:
                #finding the index of the burst event in the dataframe
                index_peak_dedz = result[result.dedz == burst_peak_dedz].index.values.astype(int)
                
                # kinetic energy is 0.5*m*v**2 (in joules).
                max_ke = 0.5 * float(result.mass[index_peak_dedz] * (result.velocity[index_peak_dedz])**2)
                initial_ke = 0.5*float(result.mass[0] * (result.velocity[0])**2)

                # Total kinetic energy lost is maximum kinetic minus the initial kinetic energy.
                #units are converted to kt.
                burst_total_ke_lost = np.abs(initial_ke - max_ke)/(4.184*10**12)

                # the dictionary should have 4 key-value pairs: burst_peak_dedz, burst_altitude, burst_total_ke_lost and outcome
                outcome = { 'burst_peak_dedz' : burst_peak_dedz, 'burst_altitude' : burst_altitude, 'burst_total_ke_lost' : burst_total_ke_lost, 'outcome': 'Airburst'}
                

                # code for plotting the results:
                def plotting_radius_velocity_atmos_density(breakup_df):
                    fig, axs = plt.subplots(1, 3, figsize=(11, 5), sharey=True)
                    
                    axs[0].plot(breakup_df.radius.tolist(), breakup_df.altitude.tolist())
                    color = 'tab:red'
                    axs[0].set_xlabel('Radius (m)', color=color)
                    axs[0].set_ylabel('Altitude (m)')
                    axs[0].plot(breakup_df.radius.tolist(), breakup_df.altitude.tolist(), color=color)
                    left, right = axs[0].get_xlim()
                    axs[0].plot((float(break_up_altitude_data.radius)), (break_up_alt), 'ko')
                    axs[0].set_title('Radius over Altitude')
                    axs[0].text(float((break_up_altitude_data.radius)+3), break_up_alt, 'Asteroid begins to flatten\n and horizontal\nradius increases', color='black')
    
                    max_radius = float(result[result.mass >= 0].loc[:, "radius"].max())
                    max_radius_alt = result.loc[(result['radius']-max_radius).abs().argsort()[:1]].altitude
                    left, right = axs[1].get_xlim()
                    axs[0].plot((max_radius), (max_radius_alt), 'ko')
                    axs[0].text(left+(max_radius/10), max_radius_alt, 'Asteroid reaches max radius', color='black')
                    axs[0].plot((result.radius[0]), (result.altitude[0]), 'ko')
                    axs[0].text(left+(1.3*result.radius[0]), result.altitude[0], 'Asteroid begins as a sphere')
                    axs[0].tick_params(axis='y')

                    color = 'tab:blue'
                    axs[1].set_xlabel('Velocity (ms$^{-1}$)', color=color)  # we already handled the x-label with ax1
                    axs[1].plot(breakup_df.velocity.tolist(), breakup_df.altitude.tolist(), color=color)
                    left, right = axs[1].get_xlim()
                    axs[1].set_title('Velocity over Altitude')
                    zero_vel = result.loc[(result['velocity']-0).abs().argsort()[:1]].velocity
                    zero_vel_alt = result.loc[(result['velocity']-0).abs().argsort()[:1]].altitude
                    axs[1].plot((zero_vel), (zero_vel_alt), 'ko')
                    axs[1].text(zero_vel, zero_vel_alt, 'Asteroid velocity reaches zero')
                    axs[1].plot((result.velocity[0]), (result.altitude[0]), 'ko')
                    axs[1].text((right - result.velocity[0]), result.altitude[0], 'Initial velocity = %s' %(result.velocity[0]))
                    axs[1].plot(([left, right]), ([max_change_vel_alt, max_change_vel_alt]), 'k--')
                    axs[1].text(left, (max_change_vel_alt + max_change_vel_alt/4), 'Altitude of max $\Delta$ velocity')
                    axs[1].tick_params(axis='y')

                    color = 'tab:green'
                    axs[2].set_xlabel('Atmospheric Density (kgm$^{-3}$)', color=color)
                    axs[2].plot(breakup_df.Atmospheric_density.tolist(), breakup_df.altitude.tolist(), color=color)
                    axs[2].tick_params(axis='y')
                    axs[2].set_title('Atmospheric Density Model')

                    plt.show()

                if plotting == True:
                    plotting_radius_velocity_atmos_density(breakup_df)
                    
                    
                def plotting_ke_over_altitude(result):
                    plt.plot(result.dedz.tolist(), result.altitude.tolist(), 'k')
                    plt.xlabel('Energy per unit height (Kt km$^{-1}$)')
                    plt.ylabel('Altitude (m)')
                    plt.text(burst_peak_dedz-10, burst_altitude-7000, 'Peak dedz', fontsize=10, color='red')
                    plt.ylim(top=100000)
                    left, right = plt.xlim()
                    bottom, top = plt.ylim()
                    plt.plot([left, right], [burst_altitude, burst_altitude], 'b--')
                    plt.text((right/3), burst_altitude+1000, 'Burst altitude', fontsize=10)
                    plt.title('Asteroid kinetic energy loss per km as a function of altitude')
                    plt.plot([left, right], [break_up_alt, break_up_alt], 'g--')
                    plt.text((right/3), break_up_alt+1000, 'Break-up altitude', fontsize=10)
                    plt.text((right/1.5), (top-top/10), 'Outcome: Airburst', bbox=dict(facecolor='none', edgecolor='black', boxstyle='round,pad=1'))
                    plt.plot(burst_peak_dedz, burst_altitude, 'ro')
                    plt.show()
                
                if plotting == True:
                    plotting_ke_over_altitude(result)

            if burst_altitude <= 0.: # if the burst altitude is less than or equal to 0 km, it is a cratering only event.
            
                # The impact time, mass and speed are given when the altitude of the asteroid is equal to 0 km.     
                # e.g. the asteroid is hitting the planet's surface.      

                # The result dataframe is sorted to find the row which contains an altitude of zero 
                # or the row which contains an altitude that is closest to zero.
                df_sort = result.loc[(result['altitude']-0.).abs().argsort()[:1]]
                # The altitude value is converted into a float for compatibility.
                altitude_close_zero = float(df_sort.altitude)

                # finding the impact time
                impact_time = float(result[result.altitude == altitude_close_zero]['time'].values)
        
                # finding the impact mass
                impact_mass = float(result[result.altitude == altitude_close_zero]['mass'].values)
        
                # finding the impact speed
                impact_speed = float(result[result.altitude == altitude_close_zero]['velocity'].values)

                # the dictionary should have 4 key-value pairs: impact_time, impact_mass, impact_speed and outcome
                outcome = { 'impact_time' : impact_time, 'impact_mass' : impact_mass, 'impact_speed' : impact_speed, 'outcome': 'Cratering'}

                # code for plotting the results:
                def plotting_time_mass_velocity(breakup_df):
                    fig, axs = plt.subplots(1, 3, figsize=(11, 5), sharey=True)
                    
                    color = 'tab:red'
                    axs[0].set_xlabel('Time (s)', color=color)
                    axs[0].set_ylabel('Altitude (m)')
                    axs[0].plot(breakup_df.time.tolist(), breakup_df.altitude.tolist(), color=color)
                    left, right = axs[0].get_xlim()
                    bottom, top = axs[0].get_ylim()
                    axs[0].set_title('Altitude over Time')
                    axs[0].plot((impact_time), (altitude_close_zero), 'ko')
                    axs[0].text((left+0.25*impact_time), (altitude_close_zero-bottom/4), 'Impact time : %.3f' %(impact_time))
                    axs[0].grid(True)

                    color = 'tab:blue'
                    axs[1].set_xlabel('Mass (kg)', color=color)  # we already handled the x-label with ax1
                    axs[1].plot(breakup_df.mass.tolist(), breakup_df.altitude.tolist(), color=color)
                    axs[1].set_title('Altitude over Mass')
                    axs[1].plot((impact_mass), (altitude_close_zero), 'ko')
                    axs[1].plot((result.mass[0]), (result.altitude[0]), 'ko')
                    axs[1].text((left+0.2*result.mass[0]), (result.altitude[0]-bottom/8), 'Initial mass : ~ %d tera-grams' %((result.mass[0]/1000000000)))
                    axs[1].text((left+0.25*impact_mass), (altitude_close_zero/3), 'Impact mass :\n ~ %d tera-grams' %(impact_mass/1000000000))
                    axs[1].grid(True)

                    color = 'tab:green'
                    axs[2].set_xlabel('Velocity (ms$^{-1}$)', color=color)
                    axs[2].plot(breakup_df.velocity.tolist(), breakup_df.altitude.tolist(), color=color)
                    axs[2].set_title('Altitude over Velocity')
                    axs[2].plot((impact_speed), (altitude_close_zero), 'ko')
                    axs[2].text((left+0.4*impact_speed), (altitude_close_zero/4), 'Impact speed : %d' %(impact_speed))
                    axs[2].grid(True)
                    plt.show()

                if plotting == True:
                    plotting_time_mass_velocity(breakup_df)

                def plotting_ke_over_altitude(result):
                    plt.plot(result.dedz.tolist(), result.altitude.tolist(), 'k')
                    plt.xlabel('Energy per unit height (Kt km$^{-1}$)')
                    plt.ylabel('Altitude (m)')
                    plt.text(burst_peak_dedz-burst_peak_dedz/10, burst_altitude-burst_altitude/5, 'Peak dedz', fontsize=10, color='red')
                    plt.ylim(top=100000)
                    left, right = plt.xlim()
                    bottom, top = plt.ylim()
                    plt.plot([left, right], [burst_altitude, burst_altitude], 'b--')
                    plt.text((right/3), burst_altitude+1000, 'Burst altitude', fontsize=10)
                    plt.title('Asteroid kinetic energy loss per km as a function of altitude')
                    plt.plot([left, right], [break_up_alt, break_up_alt], 'g--')
                    plt.text((right/3), break_up_alt+1000, 'Break-up altitude', fontsize=10)
                    plt.text((right-right/3), (top-top/6), 'Outcome: Cratering', bbox=dict(facecolor='none', edgecolor='black', boxstyle='round,pad=1'))
                    plt.plot(burst_peak_dedz, burst_altitude, 'ro')
                    plt.title('Asteroid kinetic energy loss per km as a function of altitude')
                    plt.show()

                if plotting == True:
                    plotting_ke_over_altitude(result)


            if burst_altitude <= 5000. and burst_altitude > 0.: # if the burst altitude is greater than 0 km and less than or equal to 5 km
                                                             # then the event is both an airburst and cratering.
        
                # These are the same steps as in the cratering only event (see above).
                df_sort = result.loc[(result['altitude']-0.).abs().argsort()[:1]]
                altitude_close_zero = float(df_sort.altitude)
                impact_time = float(result[result.altitude == altitude_close_zero]['time'].values)
                impact_mass = float(result[result.altitude == altitude_close_zero]['mass'].values)
                impact_speed = float(result[result.altitude == altitude_close_zero]['velocity'].values)
            
                # These are the same steps as in the airbust only event (see above).
                index_peak_dedz = result[result.dedz == burst_peak_dedz].index.values.astype(int)
                initial_ke = 0.5*float(result.mass[0] * (result.velocity[0])**2)
                max_ke = 0.5 * float(result.mass[index_peak_dedz] * (result.velocity[index_peak_dedz])**2)
                burst_total_ke_lost = (initial_ke - max_ke)/(4.184*10**12)
                
                # the dictionary should have 7 key-value pairs:
                outcome = { 'burst_peak_dedz' : burst_peak_dedz, 'burst_altitude' : burst_altitude, 'burst_total_ke_lost' : burst_total_ke_lost, 'impact_time' : impact_time, 'impact_mass' : impact_mass, 'impact_speed' : impact_speed, 'outcome': 'Airburst and cratering'}
            
                # code for plotting the results:
                def plotting_ke_over_altitude(result):
                    plt.plot(result.dedz.tolist(), result.altitude.tolist(), 'k')
                    plt.xlabel('Energy per unit height (Kt km$^{-1}$)')
                    plt.ylabel('Altitude (m)')
                    plt.text(burst_peak_dedz-burst_peak_dedz/10, burst_altitude-9*burst_altitude/3, 'Peak dedz', fontsize=10, color='red')
                    plt.ylim(top=100000)
                    left, right = plt.xlim()
                    bottom, top = plt.ylim()
                    plt.plot([left, right], [burst_altitude, burst_altitude], 'b--')
                    plt.text((right/3), burst_altitude+1000, 'Burst altitude', fontsize=10)
                    plt.title('Asteroid kinetic energy loss per km as a function of altitude')
                    if break_up_alt != np.NaN:
                        plt.plot([left, right], [break_up_alt, break_up_alt], 'g--')
                        plt.text((right/3), break_up_alt+1000, 'Break-up altitude', fontsize=10)
                    plt.text((right-right/2), (top-top/6), 'Outcome: Airburst & Cratering', bbox=dict(facecolor='none', edgecolor='black', boxstyle='round,pad=1'))
                    plt.plot(burst_peak_dedz, burst_altitude, 'ro')
                    plt.title('Asteroid kinetic energy loss per km as a function of altitude')
                    plt.show()
                
                if plotting == True:
                    plotting_ke_over_altitude(result)

            # the outcome dictionary is returned when the analyse_outcome function is called.
            return outcome
    
    def f_msd(self, t, u, strength, density):

        """ 
        Define a coupled set of (total six) ordinary differential equations  

           f[0]:dv/dt 
           f[1]:dm/dt 
           f[2]:d(theta)/dt
           f[3]:dz/dt
           f[4]:dx/dt
           f[5]:dr/dt


        
        parameters
        ----------

        velocity : float
            The entery speed of the asteroid in meters/second
        
        mass : float
            Initial mass 
            
        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        altitude : float
            Initial z-position (m)

        distance : float
            Initial x-position (m)
        
        radius : float
            The radius of the asteroid in meters

        strength : float
            The strength of the asteroid (i.e., the ram pressure above which
            fragmentation and spreading occurs) in N/m^2 (Pa)
        
        density : float
            The density of the asteroid in kg/m^3

        Define a list of u
        u = [velocity, mass, angle, altitude, distance, radius]



        Returns
        -------

        Result : list of function
        
            f[0, 1, 2, 3, 4, 5]
            f[0]:dv/dt 
            f[1]:dm/dt 
            f[2]:d(theta)/dt
            f[3]:dz/dt
            f[4]:dx/dt
            f[5]:dr/dt
        """
        f = np.zeros_like(u)
        rhoa = self.rhoa(u[3])
        A = np.pi*u[5]**2
        f[0] = -(self.Cd*rhoa*A*(u[0])**2)/(2*(u[1])) + self.g*np.sin(u[2])
        f[1] = -(self.Ch*rhoa*A*(u[0])**3)/(2*self.Q)
        f[2] = self.g*np.cos(u[2])/(u[0]) - self.Cl*rhoa*A*(u[0])/(2*(u[1])) - u[0]*np.cos(u[2])/(self.Rp+u[3])
        f[3] = -u[0]*np.sin(u[2])
        f[4] = u[0]*np.cos(u[2])/(1 + u[3]/self.Rp)
        f[5] = 0 
        if rhoa*u[0]**2 >= strength:
            f[5] = (7/2*self.alpha*rhoa/density)**(1/2)*(u[0])
        return f

    def f_msd_analytical(self, t, u, strength, density):
    
        """ 
        Define a coupled set of (total six) ordinary differential equations  

           f[0]:dv/dt 
           f[1]:dm/dt 
           f[2]:d(theta)/dt
           f[3]:dz/dt
           f[4]:dx/dt
           f[5]:dr/dt


        
        parameters
        ----------

        velocity : float
            The entery speed of the asteroid in meters/second
        
        mass : float
            Initial mass 
            
        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        altitude : float
            Initial z-position (m)

        distance : float
            Initial x-position (m)
        
        radius : float
            The radius of the asteroid in meters

        strength : float
            The strength of the asteroid (i.e., the ram pressure above which
            fragmentation and spreading occurs) in N/m^2 (Pa)
        
        density : float
            The density of the asteroid in kg/m^3

        Define a list of u
        u = [velocity, mass, angle, altitude, distance, radius]



        Returns
        -------

        Result : list of function
        
            f[0, 1, 2, 3, 4, 5]
            f[0]:dv/dt 
            f[1]:dm/dt 
            f[2]:d(theta)/dt
            f[3]:dz/dt
            f[4]:dx/dt
            f[5]:dr/dt
        """
        f = np.zeros_like(u)
        rhoa = self.rhoa(u[3])
        A = np.pi*u[5]**2
        f[0] = -(self.Cd*rhoa*A*(u[0])**2)/(2*(u[1])) + self.g*np.sin(u[2])
        f[1] = 0
        f[2] = self.g*np.cos(u[2])/(u[0]) - self.Cl*rhoa*A*(u[0])/(2*(u[1])) - u[0]*np.cos(u[2])/(self.Rp+u[3])
        f[3] = -u[0]*np.sin(u[2])
        f[4] = u[0]*np.cos(u[2])/(1 + u[3]/self.Rp)
        f[5] = 0 
        return f

    def RK4(self, f, u0, t0, t_max, dt, strength, density):
        
        """
        Solve the a coupled set of (total six) ordinary differential equations.
        Implement RK4 algorithm



        Parameters
        ----------

        velocity : float
            The entery speed of the asteroid in meters/second
        
        mass : float
            Initial mass 
            
        angle : float
            The initial trajectory angle of the asteroid to the horizontal
            By default, input is in degrees. If 'radians' is set to True, the
            input should be in radians

        altitude : float
            Initial z-position (m)

        distance : float
            Initial x-position (m)
        
        radius : float
            The radius of the asteroid in meters

        t_max : float
            The maximum of setting time

        dt : float
          The timestep

        strength : float
            The strength of the asteroid (i.e., the ram pressure above which
            fragmentation and spreading occurs) in N/m^2 (Pa)

        density : float
            The density of the asteroid in kg/m^3


    
        Returns
        -------

        Result : array
            u = [velocity, mass, angle, altitude, distance, radius]
            t = [time]   
        """
        
        u = np.array(u0)
        t = np.array(t0)
        u_all = [u0]
        t_all = [t0]
        flag = False                 
        while t < t_max:
            k1 = dt*f(t, u, strength, density)
            k2 = dt*f(t + 0.5*dt, u + 0.5*k1, strength, density)
            k3 = dt*f(t + 0.5*dt, u + 0.5*k2, strength, density)
            k4 = dt*f(t + dt, u + k3, strength, density)
            u = u + (1./6.)*(k1 + 2*k2 + 2*k3 + k4)
            u_all.append(u)
            t = t + dt
            if not flag and (u[3]<=0 or u[1]<=0):
                t_max = t + 1
                flag = True
            t_all.append(t)
        return np.array(u_all), np.array(t_all)
    
    def deg_to_rad(self, deg):
        """
        Convert degree to radians.


        Parameters
        ----------

        deg : float
            The degree of the input angle.

        Returns
        -------

        Result : float
            The radians of the input angle.

        """
        return deg * np.pi / 180


    def dmove_odeint(self, Point, t, sets):
        C_D, g, C_H, Q, C_L, R_p, alpha, rho_m, rho_0, H, Y = (self.Cd, self.g, self.Ch, self.Q, self.Cl, self.Rp, 
                                self.alpha, sets[1], self.rho0, self.H, sets[0])
        v, m, theta, z, x, r = Point
        A = np.pi*(r**2)
        rho_a = self.rhoa(z)
        return np.array([(-C_D*rho_a*A*(v**2))/(2*m) + g*np.sin(theta),
                        (-C_H*rho_a*A*(v**3))/(2*Q),
                        (g*np.cos(theta))/(v) - (C_L*rho_a*A*v)/(2*m) - (v*np.cos(theta))/(R_p + z),
                        -v*np.sin(theta),
                        (v*np.cos(theta))/(1 + z/R_p),
                        v*((7/2*alpha*rho_a/rho_m)**(1/2)) if rho_a * (v**2) >= Y else 0])

    def v_z(self, test_list, z, radians=False):
        # v(z) Calculating velocity from altitude.
        # area
        A = np.pi*test_list[3]**2 
        # angle/radian transformation
        if not radians:
            radian = test_list[2]*np.pi/180.
        else:
            radian = test_list[2]  
        #return 
        #      v(z) = v0 * e^(-(Cd * rho0 * A * H * e^(-z/H)) / ( 2 * m * sin(theta)))
        return test_list[0]*np.exp(-self.H*self.Cd*self.rho0*np.exp(-z/self.H)*A/(2*test_list[1]*np.sin(radian)))

    def dv_dz(self, test_list, z):
        # dv/dz Calculating derivative of velocity from altitude
        # area
        A = np.pi*test_list[3]**2
        return self.v_z(test_list, z, radians=True)*self.Cd*self.rho0*np.exp(-z/self.H)*A/(2*test_list[1]*np.sin(test_list[2]))

    def dE_dz(self, test_list, z, radians=False):
        # dE/dz Calculating derivative of energy from altitude - chain rule
        # area
        A = np.pi*test_list[3]**2
        # angle/radian transformation
        if not radians:
            test_list[2] = test_list[2]*np.pi/180.
        # return 
        #        dE/dz = (dv/dz * v(z) * m)/4.184e9
        return self.dv_dz(test_list, z)*self.v_z(test_list, z, radians=True)*test_list[1]/4.184e9

