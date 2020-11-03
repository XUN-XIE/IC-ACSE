import armageddon as ar
import argparse
import os

def read_single_param(name, dv):
    """
    Read one single float param from std input.

    Parameters
    ----------

    name : string
        The hint of the input.

    dv : float
        The default value.

    Returns
    -------

    value : float
        The value read from std input.
        If error raised, default value will be returned.

    """
    try:
        value = float(input("%s (default %s): " % (name, dv)))
    except:
        # print("Using default %s" % dv)
        value = dv
    return value

def read_planet_params():
    Cd = read_single_param("Cd", 1.)
    Ch = read_single_param("Ch", 0.1)
    Q = read_single_param("Q", 1e7)
    Cl = read_single_param("Cl", 1e-3)
    alpha = read_single_param("Aalpha", 0.3)
    Rp = read_single_param("Rp", 6371e3)
    g = read_single_param("g", 9.81)
    H = read_single_param("H", 8000)
    rho0 = read_single_param("rho0", 1.2)
    return Cd, Ch, Q, Cl, alpha, Rp, g, H, rho0

def read_reverse_params():
    rho_m = read_single_param("rho_m", 3300)
    theta = read_single_param("theta", 18.3)
    velocity_entry = read_single_param("velocity_entry", 19.2e3)
    datapath = input("data path (default data/ChelyabinskEnergyAltitude.csv): ")
    if not os.path.isfile(datapath):
        print("File not found. Using default.")
        datapath = "data/ChelyabinskEnergyAltitude.csv"
    r_tol = read_single_param("r_tol", 1e-2)
    r_min = read_single_param("r_min", 1e-2)
    r_max = read_single_param("r_max", 4.5e3)
    Y_tol = read_single_param("Y_tol", 1e3)
    Y_min = read_single_param("Y_min", 1e3)
    Y_max = read_single_param("Y_max", 1e12)
    return rho_m, theta, velocity_entry, datapath, r_tol, r_min, r_max, Y_tol, Y_min, Y_max

def read_impact_params():
    """
    Read params for impact params.

    Parameters
    ----------

    None

    Returns
    -------

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

    """
    radius = read_single_param("Radius", 10.)
    velocity = read_single_param("Velocity", 20e3)
    density = read_single_param("Density", 3e3)
    strength = read_single_param("Strength", 1e5)
    angle = read_single_param("Angle in Degree", 45.)
    init_altitude = read_single_param("Altitude", 1e5)
    return radius, velocity, density, strength, angle, init_altitude

# Case of an airburst event
def impact_sample_airburst(earth, plotting):
    """
    Simulate a sample impact of Airbrust.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    Returns
    -------

    None

    """
    result, outcome = earth.impact(radius=10, angle=45, strength=1e5, velocity=20e3, density=3000, plotting=plotting)
    print(result)
    print(outcome)

# Case of airburst & cratering
def impact_sample_a_c(earth, plotting):
    """
    Simulate a sample impact of Airbrust & cratering.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    Returns
    -------

    None

    """
    result, outcome = earth.impact(radius=100, angle=45, strength=1e5, velocity=20e3, density=3000, plotting=plotting)
    print(result)
    print(outcome)

# Case of cratering
def impact_sample_cratering(earth, plotting):
    """
    Simulate a sample impact of Cratering.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    Returns
    -------

    None

    """
    result, outcome = earth.impact(radius=200, angle=45, strength=1e5, velocity=20e3, density=3000, plotting=plotting)
    print(result)
    print(outcome)

# Impact from Input params
def impact_from_input(planet, plotting):
    """
    Simulate a sample impact from input params.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    Returns
    -------

    None

    """
    radius, velocity, density, strength, angle, init_altitude = read_impact_params()
    result, outcome = planet.impact(radius=radius, angle=angle, strength=strength, 
                                   velocity=velocity, density=density, init_altitude=init_altitude, plotting=plotting)
    print(result)
    print(outcome)

# Ensemble Function
def ensemble_func(earth, sample_num=20):
    """
    Trigger the ensemble function.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    sample_num : int
        The numebr of sampled points of ensemble function.

    Returns
    -------

    None

    """
    print(ar.solve_ensemble(earth, visual=True, sample_num=sample_num))

# Marion Reverse
def reverse(planet, rho_m, theta, velocity_entry, datapath,
            r_tol, r_min, r_max, Y_tol, Y_min, Y_max):
    """
    Trigger the reverse function to get radius and strength.

    Parameters
    ----------

    earth : Planet
        The planet object of armageddon.

    max_step : int
        The maximum number of iteration.

    Returns
    -------

    None

    """
    ar.analyse_data(planet, rho_m, theta, velocity_entry, datapath=datapath, r_tol=r_tol,
                    r_min=r_min, r_max=r_max, Y_tol=Y_tol, Y_min=Y_min, Y_max=Y_max, 
                    plotting=True)
    # this takes my computer 2min43 to run
    # can make it faster by setting better edge r and Y values (for presentation)

parser = argparse.ArgumentParser()
parser.add_argument("-p", "--planet", help="choose a planet",
                    type=str, choices=["Earth", "Mars", "Custom"], default="Earth")
parser.add_argument("-r", "--rho_atmos", help="choose a type of atmosphere",
                    type=str, choices=["exponential", "mars", "tabular", "constant"], default="exponential")
parser.add_argument("-e", "--event", help="choose an event",
                    type=str, choices=["Impact", "Ensemble", "Reverse"])
parser.add_argument("-s", "--sample", help="using examples of impact",
                    type=str, choices=["Airburst", "Airburst and Cratering", "Cratering"])
parser.add_argument("-n", "--num", help="for ensemble: number of samples; for reverse: max step", 
                    type=int, default=20)
parser.add_argument("--silent", help="not showing any graphs for event impact", action="store_true")
args = parser.parse_args()

plotting = not args.silent
if args.planet == "Mars":
    planet = ar.Planet(atmos_func='mars',
                       Cd=1, Ch=0.1, Q=1e7, Cl=1e-3, alpha=0.3, 
                       Rp=33895e2,g=3.71, H=11100., rho0=0.02)
    if args.rho_atmos != "mars":
        print("Input arg rho_atmos for Mars Ignored")
        print("Using planet %s; atmosphere density %s" % ("Mars", "mars"))
elif args.planet == "Custom":
    Cd, Ch, Q, Cl, alpha, Rp, g, H, rho0 = read_planet_params()
    if args.rho_atmos == "tabular":
        atmos_filename = input("Absolute atmos_filename (default data/AltitudeDensityTable.csv): ")
        if not os.path.isfile(atmos_filename):
            print("File not found. Using default.")
            atmos_filename = "data/AltitudeDensityTable.csv"
        planet = ar.Planet(atmos_func='tabular', atmos_filename=atmos_filename,
                            Cd=Cd, Ch=Ch, Q=Q, Cl=Cl, alpha=alpha, Rp=Rp,
                            g=g, H=H, rho0=rho0)
        print("Using planet %s; atmosphere density %s" % ("Custom", "tabular"))
    else:
        planet = ar.Planet(atmos_func=args.rho_atmos, Cd=Cd, Ch=Ch, Q=Q, 
                           Cl=Cl, alpha=alpha, Rp=Rp, g=g, H=H, rho0=rho0)
        print("Using planet %s; atmosphere density %s" % ("Custom", args.rho_atmos))
else:
    if args.rho_atmos == "tabular":
        atmos_filename = input("Absolute atmos_filename (default data/AltitudeDensityTable.csv): ")
        if not os.path.isfile(atmos_filename):
            print("File not found. Using default.")
            atmos_filename = "data/AltitudeDensityTable.csv"
        planet = ar.Planet(atmos_func=args.rho_atmos, atmos_filename=atmos_filename)
    else:
        planet = ar.Planet(atmos_func=args.rho_atmos)
    print("Using planet %s; atmosphere density %s" % ("Earth", args.rho_atmos))

if args.event == "Impact":
    if args.sample == "Airburst":
        impact_sample_airburst(planet, plotting=plotting)
    elif args.sample == "Airburst and Cratering":
        impact_sample_a_c(planet, plotting=plotting)
    elif args.sample == "Cratering":
        impact_sample_cratering(planet, plotting=plotting)
    else:
        impact_from_input(planet, plotting=plotting)
elif args.event == "Ensemble":
    if args.num > 0:
        ensemble_func(planet, sample_num=args.num)
    else:
        ensemble_func(planet)
elif args.event == "Reverse":
    rho_m, theta, velocity_entry, datapath, r_tol, r_min, r_max, Y_tol, Y_min, Y_max = read_reverse_params()
    reverse(planet, rho_m=rho_m, theta=theta, velocity_entry=velocity_entry,
                    datapath=datapath, r_tol=r_tol, r_min=r_min, r_max=r_max, 
                    Y_tol=Y_tol, Y_min=Y_min, Y_max=Y_max)