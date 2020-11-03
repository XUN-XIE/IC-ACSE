
#include <mpi.h>
#include <iostream>
#include <locale>

using namespace std;

int id, p;

int send_total = 10000;
int send_chunk = 100;
int zero_chunk = 10;

int gcd_calc(int a, int b)
{
	while (b != 0)
	{
		int t = b;
		b = a % b;
		a = t;
	}

	return a;
}

void Zero_Proc(void)
{
	int *pairs = new int[send_total * 2];
	int *gcd = new int[send_total];
	MPI_Request *send_request = new MPI_Request[p - 1];
	MPI_Request *recv_request = new MPI_Request[p - 1];

	for (int i = 0; i < send_total * 2; i++)
		pairs[i] = rand() + 1;

	int num_sent = 0;

	for (int i = 1; i < p; i++)
	{
		MPI_Isend(&pairs[num_sent * 2], send_chunk * 2, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request[i - 1]);
		MPI_Irecv(&gcd[num_sent], send_chunk, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_request[(i - 1)]);
		num_sent += send_chunk;
	}

	int total_handled = 0;

	while (num_sent < send_total)
	{
		for (int i = 1; i < p; i++)
		{
			int flag;
			MPI_Test(&recv_request[(i - 1)], &flag, MPI_STATUS_IGNORE);

			if (flag == 1)
			{
				if (num_sent + send_chunk > send_total)
					send_chunk = send_total - num_sent;

				MPI_Isend(&pairs[num_sent * 2], send_chunk * 2, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request[i - 1]);
				MPI_Irecv(&gcd[num_sent], send_chunk, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_request[(i - 1)]);
				num_sent += send_chunk;

				if (num_sent >= send_total)
					break;
			}
		}

		for (int i = 0; i < zero_chunk; i++)
		{
			if (num_sent >= send_total)
				break;

			gcd[num_sent] = gcd_calc(pairs[num_sent * 2], pairs[num_sent * 2 + 1]);
			num_sent++;
			total_handled++;
		}
	}

	for (int i = 1; i < p; i++)
		MPI_Isend(nullptr, 0, MPI_INT, i, 0, MPI_COMM_WORLD, &send_request[i - 1]);

	MPI_Waitall(p - 1, recv_request, MPI_STATUSES_IGNORE);

	cout << "Zero completed after handling " << total_handled << " pairs" << endl;
	cout.flush();

	delete[] pairs;
	delete[] gcd;
	delete[] send_request;
	delete[] recv_request;
}


void Other_Proc(void)
{
	int *pairs = new int[send_chunk * 2];
	int *gcd = new int[send_chunk];

	int total_handled = 0;

	while (true)
	{
		MPI_Status status;

		MPI_Recv(pairs, send_chunk * 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		int recv_cnt;
		MPI_Get_count(&status, MPI_INT, &recv_cnt);

		if (recv_cnt == 0)
			break;

		total_handled += recv_cnt/2;

		for (int i = 0; i < recv_cnt/2; i++)
			gcd[i] = gcd_calc(pairs[i * 2], pairs[i * 2 + 1]);

		MPI_Send(gcd, recv_cnt/2, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	cout << id << " has finished after processing " << total_handled << " pairs" << endl;
	cout.flush();

	delete[] pairs;
	delete[] gcd;
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 1000);

	if (id == 0)
		Zero_Proc();
	else
		Other_Proc();

	MPI_Finalize();
}