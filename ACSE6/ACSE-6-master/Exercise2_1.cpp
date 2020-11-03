#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>


using namespace std;

int id, p;
int tag_num = 1;

bool *receive_list, *send_list;
int num_receive;
int send_pair[2];


void create_communication_list(void)
{
	if (p <= 2)
	{
		if (id==0)
			cout << "This program cannot work with two or fewer processes" << endl;
		MPI_Finalize();
		exit(0);
	}

	while ((send_pair[0] = rand() % p) == id);			//find a first number that is not equal to the current id

	do
	{
		while ((send_pair[1] = rand() % p) == id);		//find a second number that is not equal to the current id
	} while (send_pair[1] == send_pair[0]);				//...and ensure that it is not the same as the first number

	receive_list = new bool[p];
	send_list = new bool[p];

	for (int i = 0; i < p; i++)
		send_list[i] = false;

	for (int i = 0; i < 2; i++)
		send_list[send_pair[i]] = true;

	/*setup the sends and receives for the communication list*/
	MPI_Request* request = new MPI_Request[(p - 1)*2];
	int cnt = 0;


	for (int i = 0; i < p; i++)
	{
		if (i != id)
		{
			MPI_Irecv(&receive_list[i], 1, MPI_BYTE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;
			MPI_Isend(&send_list[i], 1, MPI_BYTE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;
		}
		else
			receive_list[i] = false;
	}

	MPI_Waitall(cnt, request, MPI_STATUS_IGNORE);
	tag_num++;

	num_receive = 0;
	for (int i = 0; i < p; i++)
		if (receive_list[i]) num_receive++;

	cout << "Process " << id << " will be receiving data from " << num_receive << " processes" << endl;
	cout.flush();

	delete[] request;
}

void do_communication_round(int com_round)
{
	int data_to_send[2];
	int *data_to_receive = new int[num_receive];
	int received = 0, sent = 0;
	int cnt = 0;

	MPI_Request* request = new MPI_Request[2+num_receive];

	//Create the data to send and do the communications
	for (int i = 0; i < p; i++)
	{
		if (receive_list[i])
		{
			MPI_Irecv(&data_to_receive[received], 1, MPI_INT, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			received++;
			cnt++;
		}

		if (send_list[i])
		{
			data_to_send[sent] = rand();
			MPI_Isend(&data_to_send[sent], 1, MPI_INT, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			sent++;
			cnt++;
		}
	}
	
	MPI_Waitall(cnt, request, MPI_STATUS_IGNORE);
	tag_num++;


	//Doing data output - This can't be done in the previous loops as the communication is not guaranteed to have completed until MPI_Waitall is called  
	received = 0; 
	sent = 0;
	for (int i = 0; i < p; i++)
	{
		if (send_list[i])
		{
			cout << com_round << ": " << id << " process sent " << data_to_send[sent] << " to process " << i << endl;
			sent++;
		}

		if (receive_list[i])
		{
			cout << com_round << ": " << id << " process received " << data_to_receive[received] << " from process " << i << endl;
			cout.flush();
			received++;
		}
	}


	delete[] data_to_receive;
	delete[] request;
}



int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	create_communication_list();

	for (int i = 0; i < 10; i++)
	{
		do_communication_round(i);
	}

	MPI_Finalize();

	return 0;
}