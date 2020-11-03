#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>

#include <math.h>

//If there are multiple independent loops within a parallel region, you can use the
//nowait clause to avoid the implied barrier at the end of the loop construct
int main()
{
	const int n=1000;
	const int m=100;
	std::vector<float> a(n, 0), b(n, 0), y(m, 0), z(m, 0);
	double total = 0;

	for (int i = 1; i < n; i++) //A
		a[i] = i;
	for (int i = 1; i < m; i++) //A
		z[i] = i*i;

	//Here the nowait clause implies that threads can start on the second loop while other threads are still working on the first.
#pragma omp parallel num_threads(50) shared(total)
	{
#pragma omp for nowait  //try this code with and without the nowait 
		for (int i = 1; i < n; i++) //A
		{
			b[i] = (a[i] + a[i - 1]) / 2.0;
			total += b[i];
//#pragma omp critical 
			std::cerr << "a";//	std::cerr << "\na - " << omp_get_thread_num();
		} //there is an implicit barrier here - if we have the nowait clause, it skips this barrier

#pragma omp for nowait //this nowait here is not doing much - we only really need it if there is another instruction after
		for (int i = 0; i < m; i++) //B
		{
			y[i] = sqrt(z[i]);
			total += y[i];
//#pragma omp critical
			std::cerr << "b"; //std::cerr << "\nb-" << omp_get_thread_num();
		}
	}

	std::cerr << "\ntotal" << total;
	system("pause");
	return 0;
}