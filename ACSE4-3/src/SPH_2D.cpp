#include "../includes/SPH_2D.h"
//#include <omp.h>

SPH_main* SPH_particle::main_data;

void SPH_particle::calc_index(void)
{
    for (int i = 0; i < 2; i++)
        list_num[i] = int((x[i] - main_data->min_x[i]) / (2.0 * main_data->h));
}

SPH_main::SPH_main()
{
    SPH_particle::main_data = this;
}

void SPH_main::set_values(double h_factor, double DX, double T_MAX)
{
    inner_min_x[0] = min_x[0] = 0.0;
    inner_min_x[1] = min_x[1] = 0.0;

    inner_max_x[0] = max_x[0] = 20;
    inner_max_x[1] = max_x[1] = 10;

    dx = DX;
    h_fac = h_factor;
    h = dx * h_fac;
    t_max = T_MAX;
    delta_t = 0.1 * h / C0;
    cout << h << endl;
}

void SPH_main::initialise_grid(void)
{
    for (int i = 0; i < 2; i++)
    {
        // enlarge the region to set boundary particles
        min_x[i] -= 3.0 * h;
        max_x[i] += 3.0 * h;

        // calculate the number of grid in each dimention
        max_list[i] = int((max_x[i] - min_x[i]) / (2.0 * h) + 1.0);
    }

    // set dimensional size of grid matrix
    search_grid.resize(max_list[0]);
    for (int i = 0; i < max_list[0]; i++)
        search_grid[i].resize(max_list[1]);
}

void SPH_main::place_points(double* min, double* max)
{
    double x[2] = { min[0], min[1] };
    SPH_particle particle;

    while (x[0] <= max[0])
    {
        x[1] = min[1];
        while (x[1] <= max[1])
        {
            // blank part
            if (x[1] > 5 && x[1] < 10 && x[0] >= inner_min_x[0] && x[0] <= inner_max_x[0])
            {
                x[1] += dx;
                continue;
            }

            // blank part
            if (x[0] > 3 && x[0] < 20 && x[1] > 2 && x[1] < 5)
            {
                x[1] += dx;
                continue;
            }

            for (int i = 0; i < 2; i++)
                particle.x[i] = x[i];

            if (!(x[0] > inner_min_x[0] && x[0] < inner_max_x[0] && x[1] > inner_min_x[1] && x[1] < inner_max_x[1]))
                // boundary particles
                particle.boundary_status = true;
            else
                // fliud particles
                particle.boundary_status = false;

            // calculate the grid index of the particle
            particle.calc_index();

            particle_list.push_back(particle);

            x[1] += dx;
        }
        x[0] += dx;
    }
}

void SPH_main::allocate_to_grid(void)                //needs to be called each time that all the particles have their positions updated
{
    for (int i = 0; i < max_list[0]; i++)
        for (int j = 0; j < max_list[1]; j++)
            search_grid[i][j].clear();

    for (unsigned int cnt = 0; cnt < particle_list.size(); cnt++)
    {
        // compute the index of the particle in its grid, which will be used for stencil finding neighbour algorithm
        particle_list[cnt].grid_index = search_grid[particle_list[cnt].list_num[0]][particle_list[cnt].list_num[1]].size();

        // reset the particles in new grid
        search_grid[particle_list[cnt].list_num[0]][particle_list[cnt].list_num[1]].push_back(&particle_list[cnt]);
    }
}

void SPH_main::update_a_D(SPH_particle* part, SPH_particle* other_part, double dist, bool stencil)
{
    double dW = 0.0;
    double m_j;
    double bracket_num1[2];
    double bracket_num2[2];
    double e_ij[2];
    double first_item[2];
    double second_item[2];

    m_j = dx * dx * rho0;
    dW = calculate_dW(dist);

    // calculate the acceleration
    for (int k = 0; k != 2; k++)
    {
        bracket_num1[k] = part->P / pow(part->rho, 2) + other_part->P / pow(other_part->rho, 2);
        bracket_num2[k] = 1.0 / pow(part->rho, 2) + 1.0 / pow(other_part->rho, 2);

        e_ij[k] = (part->x[k] - other_part->x[k]) / dist;

        first_item[k] = m_j * bracket_num1[k] * dW * e_ij[k];
        second_item[k] = m_j * bracket_num2[k] * dW * (part->v[k] - other_part->v[k]) / dist;

        if (!part->boundary_status)
        {
            part->a[k] += mu * second_item[k] - first_item[k];
            if (stencil)
                other_part->a[k] -= mu * second_item[k] - first_item[k];
        }
    }

    // calculate the density
    double dot_product = ((part->v[0] - other_part->v[0]) * (part->x[0] - other_part->x[0]) + (part->v[1] - other_part->v[1]) * (part->x[1] - other_part->x[1])) / dist;

    part->D += m_j * dW * dot_product;
    if (stencil)
        other_part->D += m_j * dW * dot_product;
}

void SPH_main::update_dynamical_t(SPH_particle* part, SPH_particle* other_part)
{
    // compute dt_cfl
    double v_ij = sqrt(pow(part->v[0] - other_part->v[0], 2) + pow(part->v[1] - other_part->v[1], 2));
    double tmp_cfl = h / v_ij;
    if (tmp_cfl < dt_cfl)
        dt_cfl = tmp_cfl;

    // compute dt_f
    double a_i = sqrt(pow(part->a[0], 2) + pow(part->a[1], 2));
    double tmp_f = sqrt(h / a_i);
    if (tmp_f < dt_f)
        dt_f = tmp_f;

    // compute dt_a
    double tmp_a = h / C0 / sqrt(pow(part->rho / rho0, (gama - 1) / 2.0));
    if (tmp_a < dt_a)
        dt_a = tmp_a;
}

void SPH_main::neighbour_iterate_non_stencil(SPH_particle* part, bool stencil, bool change_delta_t)
{
    SPH_particle* other_part;

    //distance between particles
    double dist;

    //vector from 1st to 2nd particle
    double dn[2];

    for (int i = part->list_num[0] - 1; i <= part->list_num[0] + 1; i++)
        if (i >= 0 && i < max_list[0])
            for (int j = part->list_num[1] - 1; j <= part->list_num[1] + 1; j++)
                if (j >= 0 && j < max_list[1])
                {
                    for (unsigned int cnt = 0; cnt < search_grid[i][j].size(); cnt++)
                    {
                        other_part = search_grid[i][j][cnt];

                        //stops particle interacting with itself
                        if (part != other_part)
                        {
                            //Calculates the distance between potential neighbours
                            for (int n = 0; n < 2; n++)
                                dn[n] = part->x[n] - other_part->x[n];

                            dist = sqrt(dn[0] * dn[0] + dn[1] * dn[1]);

                            //only particle within 2h
                            if (dist < 2. * h)
                            {
                                update_a_D(part, other_part, dist);
                                update_dynamical_t(part, other_part);
                            }
                        }
                    }
                }
    // if it is predictor corrector scheme, it needs to update time stepping
    if (change_delta_t)
        delta_t = min(min(dt_cfl, dt_f), dt_a);
}

// iterates over all particles within 2h of part - can be made more efficient using a stencil and realising that all interactions are symmetric
void SPH_main::neighbour_iterate(SPH_particle* part, bool stencil, bool change_delta_t)
{
    SPH_particle* other_part;

    //distance between particles
    double dist;

    //vector from 1st to 2nd particle
    double dn[2];

    dt_cfl = 10, dt_f = 10, dt_a = 10;

    if (stencil == false)
    {
        neighbour_iterate_non_stencil(part, stencil, change_delta_t);
    }

    else if (stencil == true)
    {
        // get the grid index of target particle
        int i = part->list_num[0];
        int j = part->list_num[1];

        // set two array to loop for the grid that the taget particle need to find
        int i_list[5]{ i,  i - 1,     i,  i + 1,  i + 1 };
        int j_list[5]{ j,  j + 1, j + 1,  j + 1,      j };

        //        omp_set_num_threads(5);
        //#pragma omp parallel for
        for (int cn = 0; cn < 5; cn++)
        {
            if (i_list[cn] >= 0 && i_list[cn] < max_list[0] && j_list[cn] >= 0 && j_list[cn] < max_list[1])
                // Set the boundary of the index of neighbour_grid
            {
                if (cn != 0)
                {
                    //        #pragma omp parallel for
                    for (int m = 0; m < search_grid[i_list[cn]][j_list[cn]].size(); m++)
                    {
                        other_part = search_grid[i_list[cn]][j_list[cn]][m];

                        // calculates the distance between potential neighbours
                        for (int n = 0; n < 2; n++)
                            dn[n] = part->x[n] - other_part->x[n];

                        dist = sqrt(dn[0] * dn[0] + dn[1] * dn[1]);

                        // only particle within 2h
                        if (dist < 2. * h)
                        {
                            update_a_D(part, other_part, dist, stencil);
                            update_dynamical_t(part, other_part);
                        }
                    }
                }
                else if (cn == 0)
                {
                    //        #pragma omp parallel for
                    for (int m = part->grid_index - 1; m >= 0; m--)
                    {
                        other_part = search_grid[i][j][m];

                        // calculates the distance between potential neighbours
                        for (int n = 0; n < 2; n++)
                            dn[n] = part->x[n] - other_part->x[n];

                        dist = sqrt(dn[0] * dn[0] + dn[1] * dn[1]);

                        // only particle within 2h
                        if (dist < 2. * h)
                        {
                            update_a_D(part, other_part, dist, stencil);
                            update_dynamical_t(part, other_part);
                        }
                    }
                }
            }
        }
    }
}


//    -------------------------------------------------------------------------------
double SPH_main::calculate_W(double r)
{
    double q = r / h;
    double tmp;

    if (q >= 0 && q <= 1)
        tmp = 1 - 1.5 * q * q + 0.75 * q * q * q;
    else if (q > 1 && q <= 2)
        tmp = 0.25 * pow((2 - q), 3);
    else
        return 0;

    return (10 / 7.0 / PI / h / h) * tmp;
}

double SPH_main::calculate_dW(double r)
{
    double q = r / h;
    double tmp;

    if (q >= 0 && q <= 1)
        tmp = -3 * q + 2.25 * q * q;
    else if (q > 1 && q <= 2)
        tmp = -0.75 * pow((2 - q), 2);
    else
        return 0;

    return (10 / 7.0 / PI / h / h / h) * tmp;
}

double distance(SPH_particle& i, SPH_particle& j)
{
    double x_2 = pow((i.x[0] - j.x[0]), 2);
    double y_2 = pow((i.x[1] - j.x[1]), 2);

    return sqrt(x_2 + y_2);
}

void SPH_main::smoothing()
{
    SPH_particle* other_part;
    double dn[2];
    double dist;

    double W;
    // loop all neighbour grid and its own grid to find neighbouring particles
//#pragma omp parallel for private(W, dist, dn, other_part)
    for (int ii = 0; ii != particle_list.size(); ii++)
    {
        double numerator_sum = 0;
        double denominator_sum = 0;
        for (int i = particle_list[ii].list_num[0] - 1; i <= particle_list[ii].list_num[0] + 1; i++)
            if (i >= 0 && i < max_list[0])
                for (int j = particle_list[ii].list_num[1] - 1; j <= particle_list[ii].list_num[1] + 1; j++)
                    if (j >= 0 && j < max_list[1])
                    {
                        for (unsigned int cnt = 0; cnt < search_grid[i][j].size(); cnt++)
                        {
                            other_part = search_grid[i][j][cnt];

                            // calculates the distance between potential neighbours
                            for (int n = 0; n < 2; n++)
                                dn[n] = particle_list[ii].x[n] - other_part->x[n];

                            dist = sqrt(dn[0] * dn[0] + dn[1] * dn[1]);

                            // only particle within 2h
                            if (dist < 2. * h)
                            {
                                W = calculate_W(dist);
                                numerator_sum += W;
                                denominator_sum += W / other_part->rho;
                            }
                        }
                    }
        particle_list[ii].rho = numerator_sum / denominator_sum;
    }
}

void SPH_main::forward_euler(bool smooth, bool stencil)
{
    allocate_to_grid();

    // generally it needs to smooth the density every ten to twenty updates
    if (smooth)
    {
        smoothing();
    }

    //#pragma omp parallel for
    for (int i = 0; i != particle_list.size(); i++)
    {
        // it needs to reset acceleration and density to zero at beginning
        particle_list[i].a[0] = 0;
        if (particle_list[i].boundary_status)
            particle_list[i].a[1] = 0;
        else
            particle_list[i].a[1] = G;
        particle_list[i].D = 0;

        neighbour_iterate(&(particle_list[i]), stencil);
    }

    //#pragma omp parallel for
    for (int i = 0; i != particle_list.size(); i++)
    {
        if (particle_list[i].boundary_status == true)
        {
            particle_list[i].v[0] = 0;
            particle_list[i].v[1] = 0;
            particle_list[i].boundary_status = true;
        }
        else
        {
            bool wall[2] = { false, false };
            for (int k = 0; k != 2; k++)
            {
                particle_list[i].x[k] = particle_list[i].x[k] + delta_t * particle_list[i].v[k];
                if (particle_list[i].x[k] < inner_min_x[k] || particle_list[i].x[k] > inner_max_x[k])
                {
                    particle_list[i].x[k] = particle_list[i].x[k] - delta_t * particle_list[i].v[k];
                    particle_list[i].v[k] = -velocity_lost_rate * particle_list[i].v[k];
                    wall[k] = true;
                }

                if (!wall[k])
                    particle_list[i].v[k] = particle_list[i].v[k] + delta_t * particle_list[i].a[k];
            }
        }
        particle_list[i].rho = particle_list[i].rho + delta_t * particle_list[i].D;
        particle_list[i].calculate_P();
        particle_list[i].calc_index();
    }
}


// predictor corrector scheme which is second-order scheme
void SPH_main::predictor_corrector(bool smooth, bool change_delta_t)
{
    if (smooth)
    {
        smoothing();
    }

    allocate_to_grid();
    // Update and search neighbour
    for (int i = 0; i != particle_list.size(); i++)
    {

        particle_list[i].a[0] = 0;
        if (particle_list[i].boundary_status)
            particle_list[i].a[1] = 0;
        else
            particle_list[i].a[1] = G;

        particle_list[i].D = 0;
        neighbour_iterate(&(particle_list[i]), smooth, change_delta_t);
    }

    // Loop two times. The first loop is hal-f step, the second loop is full-step
    for (int step = 0; step < 2; step++)
    {


        if (step == 0) // Run half-step
        {
            for (int i = 0; i != particle_list.size(); i++)
            {
                // if the particle is boundary, velocity is 0
                if (particle_list[i].boundary_status == true)
                {
                    for (int k = 0; k != 2; k++)
                    {
                        // set current boundary velocity to 0.
                        particle_list[i].v[k] = 0;

                        // particle_list[i].boundary_status = true;
                         // sst previous boundary velocity to 0.
                        particle_list[i].prev_v[k] = 0;
                    }
                    particle_list[i].prev_rho = particle_list[i].rho;
                }

                // if the particle is not boundary, update postition first, and check if the position is our of grid's boundary.
                else
                {
                    // Store previous particle information
                    for (int k = 0; k != 2; k++)
                    {
                        particle_list[i].prev_x[k] = particle_list[i].x[k];
                        particle_list[i].prev_v[k] = particle_list[i].v[k];
                    }
                    particle_list[i].prev_rho = particle_list[i].rho;

                    // Assume all the particals are not close to the wall
                    bool vertical_wall = false, horizontal_wall = false;

                    // Update position in x-direction
                    particle_list[i].x[0] = particle_list[i].x[0] + 0.5 * delta_t * particle_list[i].v[0];

                    // if the x-direction position is inside wall, or out of boundary
                    if (particle_list[i].x[0] < inner_min_x[0] || particle_list[i].x[0] > inner_max_x[0])
                    {
                        // return back to previous x-direction possition, reverse the velocity direction to bounce the particle back
                        particle_list[i].x[0] = particle_list[i].x[0] - 0.5 * delta_t * particle_list[i].v[0];
                        particle_list[i].v[0] = -velocity_lost_rate * particle_list[i].prev_v[0];
                        horizontal_wall = true; // the particle is close to the horizontal wall
                    }

                    // Update position in y-direction
                    particle_list[i].x[1] = particle_list[i].x[1] + 0.5 * delta_t * particle_list[i].v[1];
                    if (particle_list[i].x[1] < inner_min_x[1] || particle_list[i].x[1] > inner_max_x[1])
                    {
                        // return back to previous y-direction possition, reverse the velocity direction to bounce the particle back
                        particle_list[i].x[1] = particle_list[i].x[1] - 0.5 * delta_t * particle_list[i].v[1];
                        particle_list[i].v[1] = -velocity_lost_rate * particle_list[i].prev_v[1];
                        vertical_wall = true; // the particle is close to the vertical wall
                    }

                    // Update all particles' velocities that are not close to the wall
                    if (!vertical_wall)
                        particle_list[i].v[1] = particle_list[i].v[1] + 0.5 * delta_t * particle_list[i].a[1];
                    if (!horizontal_wall)
                        particle_list[i].v[0] = particle_list[i].v[0] + 0.5 * delta_t * particle_list[i].a[0];
                }

                // Update density, pressure and particle index using previous time-step result
                particle_list[i].rho = particle_list[i].rho + 0.5 * delta_t * particle_list[i].D;
                particle_list[i].calculate_P();
                particle_list[i].calc_index();
            }
        }
        // Run full-step using hal-step results
        if (step == 1)
        {
            for (int i = 0; i != particle_list.size(); i++)
            {
                // if the particle is boundary, velocity is 0
                if (particle_list[i].boundary_status == true)
                {
                    // set current boundary velocity to 0.
                    particle_list[i].v[0] = 0;
                    particle_list[i].v[1] = 0;
                    particle_list[i].boundary_status = true;
                }

                // if the particle is not boundary, update postition first, and check if the position is our of grid's boundary.
                else
                {
                    // Assume all the particals are not close to the wall
                    bool vertical_wall = false, horizontal_wall = false;

                    // Update position in x-direction based on half-step
                    double temp_half_x0 = particle_list[i].prev_x[0] + 0.5 * delta_t * particle_list[i].v[0];
                    particle_list[i].x[0] = 2 * temp_half_x0 - particle_list[i].prev_x[0];

                    // if the x-direction position is inside wall, or out of boundary
                    if (particle_list[i].x[0] < inner_min_x[0] || particle_list[i].x[0] > inner_max_x[0])
                    {
                        // return back to previous x-direction possition, reverse the velocity direction to bounce the particle back
                        particle_list[i].x[0] = particle_list[i].prev_x[0];
                        particle_list[i].v[0] = -velocity_lost_rate * particle_list[i].prev_v[0];
                        horizontal_wall = true; // the particle is close to the horizontal wall
                    }

                    // Update position in y-direction based on half-step
                    double temp_half_x1 = particle_list[i].prev_x[1] + 0.5 * delta_t * particle_list[i].v[1];
                    particle_list[i].x[1] = 2 * temp_half_x1 - particle_list[i].prev_x[1];
                    if (particle_list[i].x[1] < inner_min_x[1] || particle_list[i].x[1] > inner_max_x[1])
                    {
                        // return back to previous y-direction possition, reverse the velocity direction to bounce the particle back
                        particle_list[i].x[1] = particle_list[i].prev_x[1];
                        particle_list[i].v[1] = -velocity_lost_rate * particle_list[i].prev_v[1];
                        vertical_wall = true; // the particle is close to the vertical wall
                    }

                    // Update all particles' velocities that are not close to the wall
                    if (!vertical_wall)
                    {
                        double temp_half_v1 = particle_list[i].prev_v[1] + 0.5 * delta_t * particle_list[i].a[1];
                        particle_list[i].v[1] = 2 * temp_half_v1 - particle_list[i].prev_v[1];
                    }
                    if (!horizontal_wall)
                    {
                        double temp_half_v0 = particle_list[i].prev_v[0] + 0.5 * delta_t * particle_list[i].a[0];
                        particle_list[i].v[0] = 2 * temp_half_v0 - particle_list[i].prev_v[0];
                    }

                }
                double temp_half_rho = particle_list[i].prev_rho + 0.5 * delta_t * particle_list[i].D;
                particle_list[i].rho = 2 * temp_half_rho - particle_list[i].prev_rho;
                particle_list[i].calculate_P();
                particle_list[i].calc_index();
            }
        }
    }
}
