#include <iostream>
#include <chrono>
#include <thread>

bool isPrime(long number)
{
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

void call_from_thread(int inumber)
{
	int id = std::hash < std::thread::id > {}(std::this_thread::get_id());
	srand(id);
	std::cout << "\n" << compute_primes(35000 + rand() % 100 + inumber, 1000000 + rand() % 10);
	//std::cout << "\n" << std::this_thread::get_id();
}

int main()
{
	std::cout << "\nMAIN: " << std::this_thread::get_id();
	//serial***********************************************************
	{std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	for (int i = 0; i < 10; i++)
	{
		call_from_thread(i);
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "\nSeconds:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "\n";
	}
	
	//parallel****************************************************************
	{std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	//std::thread t1(call_from_thread);
	//t1.join();//Join the thread with the main thread

	std::thread t[10]; //static array of threads
	//Launch a group of threads
	for (int i = 0; i < 10; ++i) { //creation
		t[i] = std::thread(call_from_thread, rand()%10);
	}
	for (int i = 0; i < 10; ++i) { //deployment
		t[i].join();
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "\nSeconds (Parallel):" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}
	std::cout << "\nDone!\n";
	system("pause");
}