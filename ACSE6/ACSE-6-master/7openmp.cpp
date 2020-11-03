#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>

int compute_primes(int amount, long min_value);

int fffmain()
{
	std::cerr << "\nMAX threads:" << omp_get_max_threads();
	
	double total = 0;

#pragma omp parallel
	{
#pragma omp single
		{
			int max_prime(1);
			while (max_prime < 100)//LOOP CREATING TASKS
			{
				double prime1(0), prime2(0);
#pragma omp task shared(total, prime1) depend(out:prime1) //TASK assigned to a thread : go!
				{
					prime1 = compute_primes(max_prime, 1000000);
					total += prime1;
				}
#pragma omp task shared(total, prime1) depend(in:prime1) //TASK assigned to a thread : go!
				{
					prime2 = compute_primes(max_prime, 1000000);
					total += double(prime1)*1000;					
				}
				max_prime++;
			}
		}
	}

	std::cerr << "\ntotal:" << total;

	system("pause");
	return 1;
}