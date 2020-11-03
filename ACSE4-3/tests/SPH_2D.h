/* ***************************************************************************
*  This file is part of SPH_2D.                                              *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License version 3 as         *
*  published by the Free Software Foundation.                                *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with OST. If not, see <http://www.gnu.org/licenses/>.               *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*  @file     SPH_2D.h                                                        *
*  @brief    SPH_particle class and SPH_main class                           *
*  Details.                                                                  *
*                                                                            *
*  @author   LINAONA ZHU, PING-CHEN TSAI, XUN XIE                            *
*  @version  1.0.0.1                                                         *
*  @date     2020/02/27                                                      *
*  @license  GNU General Public License (GPL)                                *
*                                                                            *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*----------------------------------------------------------------------------*
*  Change History :                                                          *
*  <Date>     | <Version> | <Author>       | <Description>                   *
*----------------------------------------------------------------------------*
*  2020/02/27 | 1.0.0.1   | LINAONA ZHU    | Create file                     *
*             |           | PING-CHEN TSAI |                                 *
*             |           | XUN XIE       |                                  *
*----------------------------------------------------------------------------*
*                                                                            *
*****************************************************************************/

#ifndef SPH_2D_hpp
#define SPH_2D_hpp
#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <string>


#define mu 0.001
#define G - 9.81
#define PI 3.1415926
#define gama 7
#define C0 20
#define rho0 1000
#define velocity_lost_rate 0.5

using namespace std;

class SPH_main;

/*
* @brief
* representation of particle
*
* @detail
* represents the properties of particles, such as position,
* velocity, acceleration, pressure, density, differentiation of
* density to time and whether it is a boundary particle or fluid particle.
*/
class SPH_particle
{
public:
    // position and velocity
    double x[2], v[2];
    
    // density and pressure
    double rho = 1000, P;
    
    // acceleration
    double a[2];
    
    // differentiation of density to time
    double D;
    
    // link to SPH_main class so that it can be used in calc_index
    static SPH_main *main_data;
    
    // store previous position for predictor corrector scheme
    double prev_x[2];
    
    // store previous velocity for predictor corrector scheme
    double prev_v[2];
    
    // store previous densiity for predictor corrector scheme
    double prev_rho;
    
    //index in neighbour finding array
    int list_num[2];
    
    // whether the particle is a boundary or fluid particle
    bool boundary_status = false;
    
    // get the index of particle in his grid, which is used for stencil neighbour particles finding algorithm
    unsigned int grid_index = 0;
    
    // calculate the grid index of particle
    void calc_index(void);
    
    // update pressure for particle
    void calculate_P()
    { P = rho0 * C0 * C0 / gama * ( pow( (rho / rho0) , gama) - 1 ); }
};

/*
* @brief
* representation of essential information for update particles and implementation of update
*
* @detail
*
*/
class SPH_main
{
public:
    //smoothing length, computed by h = h_fac * dx
    double h;
    
    // factor to determine the magnitude of h, which is functioned on dx
    double h_fac;
    
    //particle initial spacing
    double dx;
    
    // the time stepping
    double delta_t;
    
    // the upper limit of time you want to simulate
    double t_max;
    
    // three time calculated for update time stepping for predictor corrector algorithm
    double dt_cfl = 10, dt_f = 10, dt_a = 10;
    
    // dimensions of simulation region
    double min_x[2], max_x[2];
    
    // simulation region of fluid particles
    double inner_max_x[2], inner_min_x[2];
    
    // the upper limit number of grid index in two dimension
    int max_list[2];
    
    // list of all the particles
    vector<SPH_particle> particle_list;
    
    // Outer 2 are the grid, inner vector is the list of pointers in each cell
    vector<vector<vector<SPH_particle*> > > search_grid;
    
public:
    SPH_main();

    /*
    * @brief set basic parameters for particles and environment
    * @param[in] h_factor       factor to compute h
    * @param[in] DX             value of dx
    * @param[in] T_MAX          upper limit of time you want to simulate
    */
    void set_values(double h_factor, double DX, double T_MAX);
    
    
    /*
    * @brief set the size of region and initialise the search_grid to construct the grids
    */
    void initialise_grid(void);
    
    
    /*
    * @brief set all the partilces in the region including fliud partilces and voundary particles
    * @param[in] min            the array of lower bound of region for two dimension
    * @param[in] max            the array of upper bound of region for two dimension
    */
    void place_points(double *min, double *max);
    
    
    /*
    * @brief allocates all the points to the search grid (assumes that index has been appropriately updated)
    */
    void allocate_to_grid(void);
    
    
    /*
    * @brief compute the acceleration and density from neighbouring particle for target particle
    * @param[in] part                   target particle
    * @param[in] other_part             neighbour particle
    * @param[in] dist                   distance between two particles
    * @param[in] stencil                whether it is a stencil finding neighbour algorithm to update the acceleration and density of neighbour particle
    */
    void update_a_D(SPH_particle * part, SPH_particle * other_part, double dist, bool stencil = false);
    
    
    /*
    * @brief iterates over all particles within 2h of part
    * @param[in] part                   target particle
    * @param[in] stencil                whether it is a stencil finding neighbour algorithm
    * @param[in] change_delta_t         whether it is predictor corrector algorithm which need to update the dt
    */
    void neighbour_iterate(SPH_particle *part, bool stencil = false, bool change_delta_t = false);
    
    
    /*
    * @brief iterates over all particles within 2h of part for non stencil algorithm
    * @param[in] part                   target particle
    * @param[in] stencil                whether it is a stencil finding neighbour algorithm
    * @param[in] change_delta_t         whether it is predictor corrector algorithm which need to update the dt
    */
    void neighbour_iterate_non_stencil(SPH_particle *part, bool stencil, bool change_delta_t);

    
    /*
    * @brief W in cubic spline
    * @param[in] r                   distance between target particle and neibour particle
    *
    * @return value of W for later computation
    */
    double calculate_W(double r);
    
    
    /*
    * @brief differentiation of W in cubic spline
    * @param[in] r                   distance between target particle and neibour particle
    *
    * @return value of dW for later computation
    */
    double calculate_dW(double r);
    
    
    /*
    * @brief compute the updated time stepping
    * @param[in] part                   target particle
    * @param[in] other_part             neighbour particle
    */
    void update_dynamical_t(SPH_particle * part, SPH_particle * other_part);
    
    
    /*
    * @brief compute distance of two particles
    * @param[in] i             first particle
    * @param[in] j             second particle
    *
    * return distance of two particles
    */
    friend double distance(SPH_particle & i, SPH_particle & j);
    
    
    /*
    * @brief reset the density for every particle by computing neibour particles and itself
    */
    void smoothing();
    
    
    /*
    * @brief forward euler scheme which apply push back scheme to deal with the fliud particles which are going to leak
    * @param[in] smooth              whether it needs to smooth density for this update
    * @param[in] stencil             whether it applies stencil finding neighbour algorithm
    */
    void forward_euler(bool smooth = false, bool stencil = false);
    
    
    /*
    * @brief predictor corrector scheme which apply push back scheme to deal with the fliud particles which are going to leak
    * @param[in] smooth              whether it needs to smooth density for this update
    * @param[in] change_delta_t      it is predictor corrector algorithm which need to update the dt
    */
    void predictor_corrector(bool smooth = false, bool change_delta_t = false);
};
#endif /* SPH_2D_hpp */
