#include <iostream>
#include <vector>
#include <omp.h>
#include <fstream>



int sssmain()
{
	const int N(100);

	double fibo_values[N];
	fibo_values[0] = 0;
	fibo_values[1] = 1;

#pragma omp parallel
	{
		#pragma omp single
		{
			//task group
			for (int i = 2; i<N; i++)
			{
#pragma omp task depend(out:fibo_values[i]) depend(in:fibo_values[i-1],fibo_values[i-2])
				fibo_values[i] = fibo_values[i - 1] + fibo_values[i - 2];
			}
		}
	}
	
	for (auto f : fibo_values)
		std::cerr << "\nFibo: " << f;

	system("pause");
	return 0;

}