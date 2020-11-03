#include <iostream>
#include <chrono>
#include <omp.h>
#include <math.h>

bool isPrime(long number)
{
    //for (int a = 2; a < number; a++)
    //for (int a = 2; a < number / 2; a++)
    for (long a = 2; a <= sqrt(number); a++)
    {
        if (number % a == 0)
        {
            return false;
        }
    }
    return true;
}

int compute_primes(int amount, long min_value)
{
    int total_primes(0);
    //for (int currentNum = 2; ; currentNum++)///currentNum < maximum_value
    for (long currentNum = min_value;; currentNum++)
    {
        if (isPrime(currentNum))
        {
            //std::cout << endl << currentNum << " ";
            total_primes++;
            if (total_primes == amount) return total_primes;
        }
    }
    //return total_primes;
    return 0;
}

/*
int main()
{
    std::cout << "\nOMP: \n";
    {
        double start_time = omp_get_wtime();//start time

//        //Share the workload
//        #pragma omp parallel for
//                for (int i = 0; i < 100; ++i)
//                {
//                    double prime0 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime0 << "-1-"<<  omp_get_thread_num();
//
//                    double prime1 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime1 << "-2-" << omp_get_thread_num();
//
//                    double prime2 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime2 << "-3-" << omp_get_thread_num();
//
//                    double prime3 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime3 << "-4-" << omp_get_thread_num();
//                }

//        #pragma omp parallel num_threads(100)
//                {
//                    //in parallel!
//                    double prime0 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime0 << "-1-" << omp_get_thread_num();
//
//                    double prime1 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime1 << "-2-" << omp_get_thread_num();
//
//                    //stop!
//                    #pragma omp barrier
//                    std::cout << "-B";
//
//                    //in parallel!
//                    double prime2 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime2 << "-3-" << omp_get_thread_num();
//
//                    double prime3 = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
//                    #pragma omp critical
//                    std::cout << std::endl << prime3 << "-4-" << omp_get_thread_num();
//                }
        
        double end_time = omp_get_wtime();//end time
        std::cout << "\nSeconds: " << end_time - start_time; //total time
    }
}
//    system("pause");
//    return 0;
//}
*/

/*
int main()
{
//    {
//        double start_time = omp_get_wtime(); //start time - elapsed wall clock time in seconds
//
//        //compute primes
//        for (int i = 0; i < 10; ++i)
//        {
////            std::cout << "loop time : " << i << std::endl;
//            std::cout << std::endl << compute_primes(10000 + rand(), 1000000 + rand() % 10);
//        }
//
//        double end_time = omp_get_wtime(); //end time
//        std::cout << "\nSeconds: " << end_time - start_time;//total time
//    }
    
    std::cout << "\nOMP: \n";
    {
        double start_time = omp_get_wtime();//start time

    //Share the workload
#pragma omp parallel for
        for (int i = 0; i < 100; ++i)
        {
            double prime = compute_primes(10000 + omp_get_thread_num() * 2, 1000000 + +omp_get_thread_num() * 2);
#pragma omp critical
            std::cout << std::endl << prime;
        }
        
        double end_time = omp_get_wtime();//end time
        std::cout << "\nSeconds: " << end_time-start_time; //total time
    }
    
    system("pause");
    return 0;
}
*/
