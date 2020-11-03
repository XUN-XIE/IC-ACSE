#include <iostream>
#include <vector>
#include <omp.h>


int main()
{
	std::cerr << "\nMAX threads:" << omp_get_max_threads();
	/*int case1 = 4;
#pragma omp parallel if(case1>5) //parallel regions
	{
	std::cerr << "\n(1) Hello from thread " << omp_get_thread_num() << "!";
	}

	#pragma omp parallel if(case1>1)
	{
	std::cerr << "\n(2) Hello from thread " << omp_get_thread_num() << "!";
	}*/

	/*omp_set_nested(1);
#pragma omp parallel num_threads(4)
	{
	std::cerr << "\nOutside: " << omp_get_thread_num();
	#pragma omp parallel num_threads(4)
	{
	std::cerr << "\nNested: " << omp_get_thread_num();
	}
	}*/
	/*
	#pragma omp parallel for//share workload
		for (int i = 0; i < 10; i++)
		{
		std::cerr << "\nHello FOR! "<< omp_get_thread_num();
		}*/

	/*std::vector<double> myvector(10, 0);
	//can use iterators
	#pragma omp parallel for//share workload
	for (int i = 0; i < myvector.size(); i++)
	{
	//std::vector<double>::iterator iter = myvector.
	myvector[i] = rand();
	std::cerr << "\t" << myvector[i];
	}*/

	//private - completely private / no notion 
	//firstprivate - private - but it will remember the initial value
	//shared - shared among all threads
	//default - setting the default for all variables that are not mentioned
	//iterator it
	//int x(0); 
	//{int x(0); x++; //iterator it 
	//{
	//	//iterator it
	//}
	//}
	
	/*int b(1), c(1);
	#pragma omp parallel num_threads(48) shared(b) 
	{//all this happens in the virtual stack of the thread - not on the master stack
#pragma omp atomic //protects the data -- one at a time
		b+=10;//adding at the same time to shared variables!
	c++;
	std::cerr << "-" << omp_get_thread_num();
	}
	std::cerr << "\nEnd: (b-shared) : " << b;
	std::cerr << "\t (c-default) : " << c;*/

	/*int i = 0, j = 0;
#pragma omp parallel num_threads(48) default(shared)
	{
		i += 1;
#pragma omp atomic //protects the data -- one at a time
		j += 1;
	}*/

/*#pragma omp parallel
	{
		std::cerr << "\nHello from thread " << omp_get_thread_num() << "!";
	}
std::cerr << "\n\n";

#pragma omp parallel
	{
#pragma omp critical //protects the instruction -- one at a time
		std::cerr << "\nHello from thread " << omp_get_thread_num() << "!";
	}*/

	/*int r = 1;
#pragma omp parallel num_threads(48) reduction(+:r)
	{
	r += 2;
	}
	std::cerr << "\n\n\nThis is R: " << r;
	*/
	
	/*std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";

#pragma omp parallel num_threads(48) default(shared) 
	{
		std::cerr << "\nHello" << omp_get_thread_num();
		std::cerr << "\tBye" << omp_get_thread_num();
		std::cerr << "\tCiao" << omp_get_thread_num();
		std::cerr << "\tAufwiedersehen" << omp_get_thread_num();
	} //implicit barrier
	
	std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";
	std::cerr << "\nMAIN";*/


	std::cerr << "\n\n\n";
#pragma omp parallel sections num_threads(48) default(shared) 
	{
#pragma omp section
		std::cerr << "\nHello" << omp_get_thread_num();
#pragma omp section
		std::cerr << "\tBye" << omp_get_thread_num();
#pragma omp section
		std::cerr << "\tCiao" << omp_get_thread_num();
#pragma omp section
		std::cerr << "\tAufwiedersehen" << omp_get_thread_num();
	}

	/*
	int j = 0;
#pragma omp parallel num_threads(48) default(shared) 
	{
		j += omp_get_thread_num();
		std::cerr << "\tHello";
#pragma omp barrier
			std::cerr << "\nALL STOPPING now" << omp_get_thread_num();
		std::cerr << "\tBye" << omp_get_thread_num();
		std::cerr << "\tCiao" << omp_get_thread_num();
		std::cerr << "\tAufwiedersehen" << omp_get_thread_num();

	}*/

	system("pause");
	return 0;
	
}

/*int main()
{
	std::cerr << "\nMAX threads:" << omp_get_max_threads();
	int num_thread = omp_get_thread_num(); 
	double tstart = omp_get_wtime();
#pragma omp parallel 
	{
		std::cerr << "\nHello from thread " << omp_get_thread_num() << "!";
	}
	double tend = omp_get_wtime();
	std::cerr << "\nTotal time: " << tend - tstart;
	system("pause");
	return 0;
}*/