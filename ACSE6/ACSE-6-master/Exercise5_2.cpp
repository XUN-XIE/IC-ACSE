#include <mpi.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

//Note that this is the solution to 5_1b (a is much simpler than this and very similar to the first workshop exersise)

using namespace std;

int id, p, tag_num = 1;
vector<double> sort_list;
vector<double> pivot_list[3];
int list_length;

double Random(void)
{
	return (double)rand() / RAND_MAX;
}

//probably cheaper to actually pass on the range, but in this code I re-determine it for each list
void find_range(double *list, int n, double &min_val, double &max_val)
{
	min_val = list[0];
	max_val = list[1];
	for (int i = 1; i < n; i++)
	{
		if (min_val > list[i]) min_val = list[i];
		if (max_val < list[i]) max_val = list[i];
	}
}

int serial_partition(double *sort_list, int low, int high)
{
	double pivot = sort_list[high]; 
	int i = low;

	for (int j = low; j <= high - 1; j++)
	{ 
		if (sort_list[j] <= pivot)
		{
			double temp = sort_list[i];
			sort_list[i] = sort_list[j];
			sort_list[j] = temp;
			i++;  
		}
	}
	double temp = sort_list[i];
	sort_list[i] = sort_list[high];
	sort_list[high] = temp;
	return i;
}

void serial_quicksort(double *sort_list, int low, int high)
{
	if (low < high)
	{
		int pi = serial_partition(sort_list, low, high);

		serial_quicksort(sort_list, low, pi - 1);
		serial_quicksort(sort_list, pi + 1, high);
	}
}

//We could make pivot lists in place, but it is a bit more complex more than with one pivot 
//(where you can just start with the top and bottom of the list as I do in the serial quicksort code)  
//and so I am taking the slightly less efficient option of creating new lists.
//This does not change the computational order, but does require additional memory allocation.
void make_pivot_lists(double *pivots, int n, double *sort_list)
{
	for (int i = 0; i < n; i++)
	{
		if (sort_list[i] < pivots[0])
			pivot_list[0].push_back(sort_list[i]);
		else if (sort_list[i] < pivots[1])
			pivot_list[1].push_back(sort_list[i]);
		else
			pivot_list[2].push_back(sort_list[i]);
	}
}

//note that this can very easily be done recursively
void distribution(void)
{
	int first = -1, second = -1, num_pivots=0;
	MPI_Request requests[6];
	int send_minmax_first[2], send_minmax_second[2];
	int p_minmax[2];
	int request_cnt = 0;
	int in_id;

	if (id == 0)
	{
		p_minmax[0] = 1;
		p_minmax[1] = p - 1;
		in_id = -1;
	}
	else
	{
		//No point in non-blocking as only one list to receive and can't calculate without receiving it 
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, tag_num, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_DOUBLE, &list_length);
		in_id = status.MPI_SOURCE;
		sort_list.resize(list_length);
		MPI_Recv(&sort_list[0], list_length, MPI_DOUBLE, MPI_ANY_SOURCE, tag_num, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&p_minmax, 2, MPI_INT, MPI_ANY_SOURCE, tag_num + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	//a min greater than the max indicates nothing to distribute
	int num_to_distribute = p_minmax[1] - p_minmax[0] + 1;
	if (num_to_distribute < 0) num_to_distribute = 0;

	if (p_minmax[0] <= p_minmax[1])
	{
		first = p_minmax[0];
		num_pivots++;
	}
	if (p_minmax[0] + 1 <= p_minmax[1])
	{
		second = p_minmax[0]+1;
		num_pivots++;
	}

	//receive, send on and calculate
	
	double pivot[2], min_val, max_val;

	if (num_pivots != 0)
	{
		double proportion_kept = 1.0 / (num_to_distribute + 1);
		find_range(&sort_list[0], list_length, min_val, max_val);
			
		pivot[0] = min_val + proportion_kept * (max_val - min_val);
		if (num_pivots == 2)
			pivot[1] = pivot[0] + (max_val - pivot[0]) / 2.0;
		else pivot[1] = max_val * 2.0;	//ensures all definitely in first pivot list
		make_pivot_lists(pivot, list_length, &sort_list[0]);
	}

	//proportion of processors to assign weighted by size of list
	int proc_first_num = num_to_distribute;
	if (second != -1)
	{
		proc_first_num = (int)round((double)pivot_list[1].size() / (pivot_list[1].size() + pivot_list[2].size())*num_to_distribute);
		if (proc_first_num == 0) proc_first_num = 1;
	}

	//setup both the sends and the receives - use send process number as tag on on receive to ensure correct one is received
	if (first != -1)
	{
		send_minmax_first[0] = p_minmax[0] + num_pivots;
		send_minmax_first[1] = p_minmax[0] + num_pivots + proc_first_num - 2;
		MPI_Isend(&pivot_list[1][0], pivot_list[1].size(), MPI_DOUBLE, first, tag_num, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
		MPI_Isend(send_minmax_first, 2, MPI_INT, first, tag_num+1, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
		int recv_pos = pivot_list[0].size();
		MPI_Irecv(&sort_list[recv_pos], pivot_list[1].size(), MPI_DOUBLE, first, tag_num + 2 + first, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
	}
	if (second != -1)
	{
		send_minmax_second[0] = p_minmax[0] + num_pivots + proc_first_num - 1;
		send_minmax_second[1] = p_minmax[1];
		MPI_Isend(&pivot_list[2][0], pivot_list[2].size(), MPI_DOUBLE, second, tag_num, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
		MPI_Isend(send_minmax_second, 2, MPI_INT, second, tag_num + 1, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
		int recv_pos = pivot_list[0].size() + pivot_list[1].size();
		MPI_Irecv(&sort_list[recv_pos], pivot_list[2].size(), MPI_DOUBLE, second, tag_num + 2 + second, MPI_COMM_WORLD, &requests[request_cnt]);
		request_cnt++;
	}

	if (num_pivots != 0)
	{
		serial_quicksort(&pivot_list[0][0], 0, pivot_list[0].size()-1);
		for (int i = 0; i < pivot_list[0].size(); i++)
			sort_list[i] = pivot_list[0][i];
	}
	else serial_quicksort(&sort_list[0], 0, sort_list.size()-1);

	MPI_Waitall(request_cnt, requests, MPI_STATUSES_IGNORE);

	//send back
	if (id != 0)
	{
		MPI_Send(&sort_list[0], sort_list.size(), MPI_DOUBLE, in_id, tag_num + 2 + id, MPI_COMM_WORLD);
	}
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 1000);

	//Assume that list is generated on process zero and so only it knows the size
	if (id == 0)
	{
		list_length = 10000;
		sort_list.resize(list_length);
		for (int i = 0; i < list_length; i++)
			sort_list[i] = Random()*1000.0;
	}

	distribution();

	//Check for out of order elements
	if (id == 0)
	{
		bool check = false;
		for (int i = 1; i < sort_list.size(); i++)
			if (sort_list[i - 1] > sort_list[i])
			{
				check = true;
				break;
			}

		if (check)
			cout << "There are out of order elements" << endl;
		else cout << "All elements are in order" << endl;
	}
	MPI_Finalize();
}