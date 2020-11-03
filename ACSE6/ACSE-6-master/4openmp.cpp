#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>

int compute_primes(int amount, long min_value);

int uuumain()
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
				double prime1, prime2;
#pragma omp task shared(total)//TASK assigned to a thread : go!
				{
					prime1 = compute_primes(max_prime, 1000000);
#pragma omp atomic
					total += prime1;//this is lost!
//#pragma omp critical
//					std::cerr << "\n" << prime1;
				}

				//std::cerr << "\nTeam member working on:" << max_prime;
				max_prime++;
//#pragma omp task  //TASK
				{
					//prime2 = compute_primes(max_prime, 1000000);
					//total += prime2;//this is lost! - but sufficient to force the compiler to actually compute the prime number
				}
				//std::ofstream ofs("out.txt", std::ios::app);
				//ofs << "\nTeam member working on:" << max_prime;
				//ofs.close();
			}
		}
	}

	std::cerr << "\ntotal:" << total;
	system("pause");
	return 1;
}
