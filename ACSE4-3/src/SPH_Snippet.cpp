#include "../includes/SPH_2D.h"
#include "../includes/file_writer.h"
#include <ctime>

SPH_main domain;

int main(void)
{
    double h_factor = 1.3;
    double DX;

    cout << "Input the initial distance between particles (namely dx) : ";
    cin >> DX;
    if (cin.fail())
    {
        cerr << "you input a wrong format of dx! Over!" << endl;
        exit(0);
    }

    double T_MAX;
    cout << "Input the total time you want to simulate : ";
    cin >> T_MAX;
    if (cin.fail())
    {
        cerr << "you input a wrong format of total time! Over!" << endl;
        exit(0);
    }

    bool scheme;
    cout << "Select your time stepping scheme to simulate, \"0\" : Forward Euler, and \"1\" : predictor corrector:\n";
    cin >> scheme;
    if (cin.fail())
    {
        cerr << "you input a wrong format of scheme number! Over!" << endl;
        exit(0);
    }


    //Set simulation parameters
    domain.set_values(h_factor, DX, T_MAX);

    //initialise simulation grid
    domain.initialise_grid();

    //places initial points - will need to be modified to include boundary points and the specifics of where the fluid is in the domain
    domain.place_points(domain.min_x, domain.max_x);

    write_file("original.vtp", &domain.particle_list);

    //needs to be called for each time step
    domain.allocate_to_grid();

    bool smooth;
    double time = 0;
    int cnt = 0;

    clock_t start, end;
    start = clock();
    while (time < domain.t_max)
    {
        smooth = false;
        domain.allocate_to_grid();
        if (cnt % 20 == 0)
            smooth = true;

        if (scheme)
            domain.predictor_corrector(smooth);
        else
            domain.forward_euler(smooth);

        if ( cnt % 50 == 0 )
        {
            cout << "iteration " << cnt << endl;
            string name = "example_" + to_string( (int) ( cnt / 50 ) ) + ".vtp";
            cout << name << endl;
            cout << "time is : " << time << endl;
            const char* cstr = name.c_str();
            write_file(cstr, &domain.particle_list);
        }

        cnt++;
        time += domain.delta_t;
    }
    cout << "final iteration is " << cnt << endl;
    end = clock();
    cout << "all time consuming is : " << (end - start) / (double)CLOCKS_PER_SEC << " seconds" << endl;

    return 0;
}
