#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;

void bubble_sort(int *list, int *index)
{
	bool swapped;
	int temp;

	do
	{
		swapped = false;

		for (int i = 0; i < p - 1; i++)
		{
			if (list[i] > list[i + 1])
			{
				temp = list[i];
				list[i] = list[i + 1];
				list[i + 1] = temp;

				temp = index[i];
				index[i] = index[i + 1];
				index[i + 1] = temp;
				swapped = true;
			}
		}

	} while (swapped);
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int rand_priority = rand() % 101;

	int *recv_list = nullptr;
	int *order_list = nullptr;
	int *send_list = nullptr;

	//Only process zero is actually receiving data
	if (id == 0)
	{
		recv_list = new int[p];
		order_list = new int[p];
		send_list = new int[p];
	}

	MPI_Gather(&rand_priority, 1, MPI_INT, recv_list, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (id == 0)
	{
		for (int i = 0; i < p; i++)
			order_list[i] = i;

		bubble_sort(recv_list, order_list);

		for (int i = 0; i < p; i++)
		{
			send_list[order_list[i]] = i;
		}
	}

	int id_order;

	MPI_Scatter(send_list, 1, MPI_INT, &id_order, 1, MPI_INT, 0, MPI_COMM_WORLD);

	cout << "Processor " << id << " is " << id_order << " in the list (" << rand_priority << " priority number)" << endl;

	MPI_Finalize();

	delete[] recv_list;
	delete[] order_list;
	delete[] send_list;
}
