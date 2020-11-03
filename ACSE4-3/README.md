# ACSE-4-SPH

[Smoothed Particle Hydrodynamics](https://en.wikipedia.org/wiki/Smoothed-particle_hydrodynamics) (SPH) is a meshless
method for solving the Navier-Stokes equation, in which fluid properties are stored on Lagrangian fluid particles (i.e. on
particles which move with the fluid flow). The particles interact to generate values across the entire fluid domain through
continuous smoothing kernels. 

As the SPH method is meshless and Lagrangian, it is ideal for solving problems involving fluid flow with interfaces and free 
surfaces. This tool implements the SPH method in C++ to solve wave generation in a lock-release/dam-break problem.

## Preview

### Paraview 

Initial simulation visualisation:

![animation.gif](https://github.com/XUN-XIE/ACSE4-3-SPH/blob/master/animation-1.gif)

Finer simulation visualisation:

![animation.gif](https://github.com/XUN-XIE/ACSE4-3-SPH/blob/master/animation-2.gif)


### Python Post Processing 

Created using [Post Processing](https://github.com/acse-2019/acse-4-sph-lubhair-yoo-ar/blob/master/Post_processing.ipynb)



![animation.gif](https://github.com/XUN-XIE/ACSE4-3-SPH/blob/master/animation.gif)

![animation.gif](https://github.com/XUN-XIE/ACSE4-3-SPH/blob/master/crest.png)


## Installation

Clone the repository from Github by either:
* Using command line:
`git clone https://github.com/acse-2019/acse-4-sph-lubhair-yoo-ar.git`
* Downloading the repository as a .zip

## Usage:
Using Unix command line arguments for the g++ compiler from within the library directory, run:

```g++ -o sph main.cpp```

Then run:

```./sph```

to execute the program.

Meanwhile, if you want to apply OpenMP to accelerate the progamme, you can cancel the comment of my code to realise it and remember to set the OpenMP environment in your C++ IDE with include omp.h file.

## Structure 

SPH_particle class is used for representing the particles in simulator carrying its properties such as position, velocity, acceleration, pressure, density, differentiation of density to time and whether it is a boundary particle or fluid particle.

SPH_main class is used for representing the essential data such as value time stepping, region bound and so on. Meanwhile, it is also used for updating particles.

#### Class methods:

for SPH_main class, there are:

```

    void set_values(double h_factor, double DX, double T_MAX);
    
    void initialise_grid(void);
    
    void place_points(double *min, double *max);
    
    void allocate_to_grid(void);
    
    void update_a_D(SPH_particle * part, SPH_particle * other_part, double dist, bool stencil = false);

    void neighbour_iterate(SPH_particle *part, bool stencil = false, bool change_delta_t = false);
    
    void neighbour_iterate_non_stencil(SPH_particle *part, bool stencil, bool change_delta_t);

    double calculate_W(double r);
    
    double calculate_dW(double r);
    
    void update_dynamical_t(SPH_particle * part, SPH_particle * other_part);
    
    friend double distance(SPH_particle & i, SPH_particle & j);
    
    void smoothing();
    
    void forward_euler(bool smooth = false, bool stencil = false);
    
    void predictor_corrector(bool smoot = false, bool chang_delta_t = false);

```


## Documentation

[SPH Documentation](https://github.com/acse-2019/acse-4-sph-lubhair-yoo-ar/blob/master/html/index.html)

## Testing

The tool includes tests, which you can use to check its operation on your system. With the code compiled, these can be run 
with

Check that not too far from boundry 

velocity check 

density check -> reference


```
python run_tests.py
```

## Built With

* [C++](http://www.cplusplus.com/) - Programming Language
* [Open MPI](https://www.open-mpi.org/) - Message Passing Interface


## Functionality

We realised forward Euler scheme to update the status of particles with fixed initial time stepping setting.

We wrote stencil finding neighbour algorithm, but it cannot work properly. 

We wrote predictor corrector scheme to update the status of particles. It is a second-order accurate scheme with fixed timestep. Dynamic timestep is not functionning for this scheme.

The program is able to output results to files,. Also We implemented crest velocity tracking program in python.


## Boundary method

When the fluid particles are going to hit the wall (boundary particles), we applied the push back method to deal with problem of leak. 

We firstly compute the updated the position of particles, then we check whether it will hit the wall. If the fluid particles hit the wall, we do not update the position for this update, namely we fix the portion of particles this process of update. Meanwhile, we change the corresponding direction of velocity, and add a lost rate of velocity to decrease the velocity. For example, if the particle is going to hit the left boundary, we do not change the position of particle for this process of update. And we only change the horizontal velocity of particle to opposite direction (namely right), and we multiply the original horizontal velocity with a lost rate.


## Parallel programming 

We applied OpenMP to accelerate the programming.

#### Serial

Forward Euler:

Dx = 0.5    ：46.3803 seconds

Dx = 0.2	 :  450.164 seconds

Because the serial code for running condition that dx equals to 0.1 is very slow, so we have not recorded the time


#### OpenMP

Forward Euler :

Dx = 0.5	 : 12.9997 seconds

Dx = 0.2	 : 120.836 seconds

Dx = 0.1 	 : 743.459 seconds


## Arbitrary boundary shape

Because we applied push back boundary method, so It is difficult to add arbitrary boundary shape.

## Authors

* **Vagif Aliyev** - [Github](https://github.com/acse-va719)
* **Ping-Chen (Rebecca) Tsai** - [Github](https://github.com/acse-va719)
* **Ellyess Benmoufok** - [Github](https://github.com/acse-va719)
* **Xun Xie** - [Github](https://github.com/acse-xx619)
* **Lingaona Zhu** - [Github](https://github.com/acse-va719)

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/acse-2019/acse-4-sph-lubhair-yoo-ar/blob/master/LICENSE) file for details

