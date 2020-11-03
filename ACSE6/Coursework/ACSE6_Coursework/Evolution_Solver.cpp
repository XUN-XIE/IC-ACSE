#include "Evolution_Solver.hpp"

/*
 class method to evolve the matrix applying stripe decomposition.
 The decomposition is realisde by horizontal stripes because C++ stores
 its arrray in row style.
 */
void stripe::striped_solver(int loop_num, int myid, int numprocs, bool outfile, bool outtime)
{
    // call cut_matrix function to split the rows of original matrix to nearly equal number of rows in each processor (namely all of them are equal except last processor)
    vector<int> group_list = cut_matrix(numprocs);

    // initialize array
    int small_row = group_list[myid];
    stripe arr(small_row, cols - 2, periodic, true);   // remember the buffer
    
    // inital output file if you want to output
    if (outfile)
    {
        // define receive amount and dispersion arrays for gather all small matrix to master processor
        int * rec_count = new int [numprocs];
        int * disp = new int [numprocs];
        for (int i = 0; i != numprocs; i++)
        {
            rec_count[i] = group_list[i] * arr.cols;
            if (i != 0)
                disp[i] = i * rec_count[i - 1];
        }
        
        // gather all data to process 0 to print or save to file for outputing
        MPI_Gatherv(&arr(1,0), (arr.rows-2) * arr.cols, MPI_INT, &(*this)(1,0), rec_count, disp, MPI_INT, 0, MPI_COMM_WORLD);
        
        delete [] rec_count;
        delete [] disp;
        
        file_out(0, 0, numprocs, loop_num);
    }
    
    // calculate neighbor processors for each processor for sending and receiving information
    int top, down;

    if ( myid > 0 )
        top = myid - 1;
    else
    {
        if(this->periodic)
            top = numprocs - 1;
        else
            top = MPI_PROC_NULL;
    }
  
    if ( myid < numprocs - 1 )
        down = myid + 1;
    else
        {
            if(this->periodic)
                down = 0;
            else
                down = MPI_PROC_NULL;
        }

    // set the beginning and ending rows of calculate
    int begin_row = 1;
    int end_row = small_row;

    // define request
    MPI_Request * request = new MPI_Request[4];

    
    double start_time, end_time;
    double sum_time = 0;
    // start loop for updating
    for ( int n = 0; n != loop_num; n++ )
    {
        start_time = MPI_Wtime();
        // define buffers for sending information
        int * buf1 = new int [cols];
        int * buf2 = new int [cols];
        for (int i = 0; i != arr.cols; i++)
        {
            buf1[i] = arr(begin_row,i);
            buf2[i] = arr(end_row,i);
        }
        
        MPI_Isend(buf1, arr.cols, MPI_INT, top, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(buf2, arr.cols, MPI_INT, down, 1, MPI_COMM_WORLD, &request[1]);

        MPI_Irecv(&arr(0, 0), arr.cols, MPI_INT, top, 1, MPI_COMM_WORLD, &request[2]);
        MPI_Irecv(&arr(arr.rows-1, 0), arr.cols, MPI_INT, down, 0, MPI_COMM_WORLD, &request[3]);

        MPI_Waitall(4, request, MPI_STATUS_IGNORE);
        
        // only after information is sent, the function can be called. Or the result will be error.
        arr.basic_evolution();
        
        // ensure all processors are synchronization to update for next loop
        MPI_Barrier(MPI_COMM_WORLD);
        end_time = MPI_Wtime();
        
        double time_cost = end_time - start_time;
        

        // printing out time consuming if you want to output
        if (outtime)
        {
            double * tmp_time = new double;
            MPI_Reduce(&time_cost, tmp_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            sum_time += *tmp_time;
            
            if (n == loop_num - 1 && myid == 0)
            {
                if(periodic)
                    printf("stripe ghost time consuming %f\n", sum_time / numprocs);
                else
                    printf("stripe non-periodic time consuming %f\n", sum_time / numprocs);
            }
        }
        
        // if it is ghost condition, we also need to update the boundary values at boundary processors
        if(periodic)
        {
            for (int i = 0; i != arr.rows; i++)
            {
                arr(i,0) = arr(i, arr.cols-2);
                arr(i,arr.cols-1) = arr(i,1);
            }
            
            if(myid == 0)
            {
                arr(0,0) = arr(0,arr.cols-2);
                arr(0,arr.cols-1) = arr(0,1);
            }
            
            if(myid == numprocs - 1)
            {
                arr(arr.rows-1,0) = arr(arr.rows-1,arr.cols-2);
                arr(arr.rows-1,arr.cols-1) = arr(arr.rows-1,1);
            }
        }
        
        // ensure all processors are synchronization to update for next loop
        MPI_Barrier(MPI_COMM_WORLD);
        if (outfile && myid == 0)
        {
            // define receive amount and dispersion arrays for gather all small matrix to master processor
            int * rec_count = new int [numprocs];
            int * disp = new int [numprocs];
            for (int i = 0; i != numprocs; i++)
            {
                rec_count[i] = group_list[i] * arr.cols;
                if (i != 0)
                    disp[i] = i * rec_count[i - 1];
            }
            
            // gather all data to process 0 to print or save to file for outputing
            MPI_Gatherv(&arr(1,0), (arr.rows-2) * arr.cols, MPI_INT, &(*this)(1,0), rec_count, disp, MPI_INT, 0, MPI_COMM_WORLD);
            
            delete [] rec_count;
            delete [] disp;
            
            MPI_Barrier(MPI_COMM_WORLD);
            
            file_out(n + 1, time_cost, numprocs);
        }
    
        delete [] buf1;
        delete [] buf2;
    }
    delete [] request;
    

}

/*
 split the rows of original matrix to nearly equal number of rows in each processor (namely all of them are equal except last processor)
 */
vector<int> stripe::cut_matrix(int numprocs)
{
    vector<int> res;
    int size_to_split = this->rows - 2;
    all_except_last = size_to_split / numprocs;
    last_num = size_to_split - (numprocs - 1) * (size_to_split / numprocs);
    
    for ( int i = 0; i != numprocs - 1; i++ )
        res.push_back(all_except_last);
    res.push_back(last_num);
    
    return res;
}

void stripe::file_out(int num, double running_time, int numprocs, int loopnum)
{
    ofstream fout;
    string filename;
    if (periodic)
        filename = "stripe_ghost" + to_string(num) + ".txt";
    else
        filename = "stripe_non_periodic" + to_string(num) + ".txt";
    
    fout.open(filename);
    
    fout << rows-2 << '\n';
    fout << cols-2 << '\n';
    fout << running_time << '\n';
    fout << numprocs << '\n';
    fout << loopnum << '\n';
    
    for (int i = 1; i != rows - 1; i++)
    {
        for (int j = 1; j != cols - 1; j++)
        {
            fout << (*this)(i,j) << ' ';
        }
        if ( i < rows - 2)
            fout << '\n';
    }
}


// ----------------------------------Grid solver-------------------------------------

/*
 class method to evolve the matrix applying grid decomposition.
 if the cores is a prime number, it will call the stripe decomposition method to update the matrix.
 in other condition, it will cut matrix into different size matrix.
 */
void grid::grid_solver(int loopnum, int myid, int numprocs, bool outfile, bool outtime)
{
    // call find_dimensions to split processors into m * n form
    int grid_rows, grid_cols;
    find_dimensions(numprocs, myid, grid_rows, grid_cols);

    // if the numprocs is a prime number, then call stripe method!
    if (grid_rows == 1)
    {
        stripe new_M(rows-2, cols-2, periodic, false);
        new_M.striped_solver(loopnum, myid, numprocs, outfile, outtime);
        return;
    }
    
    // call cut_matrix_rows function to split the rows of original matrix to nearly equal number of rows in each processor (namely all of them are equal except last processor in rows)
    vector<int> row_size = cut_matrix_row(grid_rows);
    // call cut_matrix_cols function to split the rows of original matrix to nearly equal number of cols in each processor (namely all of them are equal except last processor in columns)
    vector<int> col_size = cut_matrix_col(grid_cols);
    
    // set values for small matrix in different processors
    int arr_rows = myid / grid_cols;
    int arr_cols = myid % grid_cols;
    grid arr(row_size[arr_rows], col_size[arr_cols], periodic);

//--------------------------for test solver--------------------------
//     for test input, 9 processors and 30 * 30
//    for (int i = 0; i != arr.rows; i++)
//        for (int j = 0; j != arr.cols; j++)
//            arr(i,j) = 0;
//    if(myid == 0 || myid == 8)
//        arr(2,2) = arr(2,3) = arr(2,4) = arr(3,4) = arr(4,3) = 1;
//    if(myid == 1)
//        arr(3,3) = arr(3,4) = arr(3,5) = 1;
//    if(myid == 3)
//        arr (3,3) = arr(4,3) = arr(5,3) = 1;
//    if( myid == 4)
//        arr(1,1) = arr(1,2) = arr(2,1) = arr(2,3) = arr(3,2) = 1;
//    if( myid == 6 || myid == 2)
//        arr(2,2) = arr(2,3) = arr(3,2) = arr(3,3) = 1;
    
    // ouotput file if you want
    if (outfile)
    {
        gather_all_data(myid, numprocs, grid_cols, row_size, col_size, arr);
        MPI_Barrier(MPI_COMM_WORLD);
        if (myid == 0)
        file_out(0, 0, numprocs, loopnum);
    }
    
    // calculate neighbor processors for each processor for sending and receiving information
    int left, top, right, down;
    
    // general condition (core grids)
    left = myid - 1;
    right = myid + 1;
    top = myid - grid_cols;
    down = myid + grid_cols;
    
    if(arr_rows == 0)
    {
        if(periodic)
            top = myid + (grid_rows - 1) * grid_cols;
        else
            top = MPI_PROC_NULL;
    }
    if(arr_rows == grid_rows - 1)
    {
        if(periodic)
            down = myid - (grid_rows - 1) * grid_cols;
        else
            down = MPI_PROC_NULL;
    }
    if(arr_cols == 0)
    {
        if(periodic)
            left = myid + grid_cols - 1;
        else
            left = MPI_PROC_NULL;
    }
    if(arr_cols == grid_cols - 1)
    {
        if(periodic)
            right = myid - (grid_cols - 1);
        else
            right = MPI_PROC_NULL;
    }
        
    // calculate for corner neiboring processors for each processor for sending and receiving information usd to update four corner values for each small matrix
    int topleft, topright, downleft, downright;
    // general condition (core grids)
    topleft = myid - grid_cols - 1;
    topright = myid - grid_cols + 1;
    downleft = myid + grid_cols - 1;
    downright = myid + grid_cols + 1;
    
    // boundary grids excluding corner grids
    if ( myid != 0 && myid != grid_cols - 1 && myid != numprocs - grid_cols && myid != numprocs - 1)
    {
        if(arr_rows == 0)
        {
            if(periodic)
            {
                topleft = top - 1;
                topright = top + 1;
            }
            else
            {
                topleft = MPI_PROC_NULL;
                topright = MPI_PROC_NULL;
            }
        }
        if(arr_rows == grid_rows - 1)
        {
            if(periodic)
            {
                downleft = down - 1;
                downright = down + 1;
            }
            else
            {
                downleft = MPI_PROC_NULL;
                downright = MPI_PROC_NULL;
            }
        }
        if(arr_cols == 0)
        {
            if(periodic)
            {
                topleft = left - grid_cols;
                downleft = left + grid_cols;
            }
            else
            {
                topleft = MPI_PROC_NULL;
                downleft = MPI_PROC_NULL;
            }
        }
        if(arr_cols == grid_cols - 1)
        {
            if(periodic)
            {
                topright = right - grid_cols;
                downright = right + grid_cols;
            }
            else
            {
                topright = MPI_PROC_NULL;
                downright = MPI_PROC_NULL;
            }
        }
    }
    else
    {
        if (myid == 0)
        {
            
            if(periodic)
            {
                topleft = numprocs - 1;
                topright = top + 1;
                downleft = left + grid_cols;
            }
            else
            {
                topleft = MPI_PROC_NULL;
                topright = MPI_PROC_NULL;
                downleft = MPI_PROC_NULL;
            }
        }
        if (myid == grid_cols - 1)
        {
            
            if(periodic)
            {
                topleft = top - 1;
                topright = numprocs - grid_cols;
                downright = right + grid_cols;
            }
            else
            {
                topleft = MPI_PROC_NULL;
                topright = MPI_PROC_NULL;
                downright = MPI_PROC_NULL;
            }
        }
        if (myid == numprocs - grid_cols)
        {
            
            if(periodic)
            {
                topleft = left - grid_cols;
                downleft = grid_cols - 1;
                downright = down + 1;
            }
            else
            {
                topleft = MPI_PROC_NULL;
                downright = MPI_PROC_NULL;
                downleft = MPI_PROC_NULL;
            }
        }
        if (myid == numprocs - 1)
        {
            
            if(periodic)
            {
                topright = right - grid_cols;
                downright = 0;
                downleft = down - 1;
            }
            else
            {
                downright = MPI_PROC_NULL;
                topright = MPI_PROC_NULL;
                downleft = MPI_PROC_NULL;
            }
        }
    }
    
    // process special condition when number of processor is 1
    if (numprocs == 1)
    {
        if(periodic)
        {
        left = 0 ;
        right = 0;
        top = 0;
        down = 0;
        topleft = 0;
        topright = 0;
        downleft = 0;
        downright = 0;
        }
    }
    
    // define request
    MPI_Request * request = new MPI_Request[8];
    MPI_Request * request_corner = new MPI_Request[8];

    double start_time, end_time;
    double sum_time = 0;
    // start loop for updating
    for ( int n = 0; n != loopnum; n++ )
    {
        start_time = MPI_Wtime();
        // define send buffers for sending information
        int * buf_left = new int [arr.rows];
        int * buf_top = new int [arr.cols];
        int * buf_right = new int [arr.rows];
        int * buf_down = new int [arr.cols];
        
        for (int i = 0; i != arr.cols; i++)
        {
            buf_top[i] = arr(1,i);
            buf_down[i] = arr(arr.rows-2,i);
        }
        for (int i = 0; i != arr.rows; i++)
        {
            buf_left[i] = arr(i, 1);
            buf_right[i] = arr(i, arr.cols-2);
        }
        
        // define receive buffers for receiving information
        int * buf_rec_left = new int [arr.rows];
        int * buf_rec_right = new int [arr.rows];
        
        // send information to left, top, right and down neighboring processors
        MPI_Isend(buf_left, arr.rows, MPI_INT, left, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(buf_top, arr.cols, MPI_INT, top, 1, MPI_COMM_WORLD, &request[1]);
        MPI_Isend(buf_right, arr.rows, MPI_INT, right, 2, MPI_COMM_WORLD, &request[2]);
        MPI_Isend(buf_down, arr.cols, MPI_INT, down, 3, MPI_COMM_WORLD, &request[3]);
        
        // receive from left
        MPI_Irecv(buf_rec_left, arr.rows, MPI_INT, left, 2, MPI_COMM_WORLD, &request[4]);
        // receive from top
        MPI_Irecv(&arr(0,0), arr.cols, MPI_INT, top, 3, MPI_COMM_WORLD, &request[5]);
        // receive from right
        MPI_Irecv(buf_rec_right, arr.rows, MPI_INT, right, 0, MPI_COMM_WORLD, &request[6]);
        // receive from down
        MPI_Irecv(&arr(arr.rows-1,0), arr.cols, MPI_INT, down, 1, MPI_COMM_WORLD, &request[7]);

        MPI_Waitall(8, request, MPI_STATUS_IGNORE);
        
        // we need to change the boundary after waitall!!!!
        for (int i = 0; i != arr.rows; i++)
        {
            if(left == MPI_PROC_NULL)
                arr(i,0) = 0;
            else
                arr(i,0) = buf_rec_left[i];
            
            if(right == MPI_PROC_NULL)
                arr(i,arr.cols-1) = 0;
            else
                arr(i,arr.cols-1) = buf_rec_right[i];
        }
        
        // send four corner points to ohters
        MPI_Isend(&arr(1,1), 1, MPI_INT, topleft, 5, MPI_COMM_WORLD, &request_corner[0]);
        MPI_Isend(&arr(1,arr.cols-2), 1, MPI_INT, topright, 6, MPI_COMM_WORLD, &request_corner[1]);
        MPI_Isend(&arr(arr.rows-2,1), 1, MPI_INT, downleft, 7, MPI_COMM_WORLD, &request_corner[2]);
        MPI_Isend(&arr(arr.rows-2,arr.cols-2), 1, MPI_INT, downright, 8, MPI_COMM_WORLD, &request_corner[3]);
        
        // receive from topleft
        MPI_Irecv(&arr(0,0), 1, MPI_INT, topleft, 8, MPI_COMM_WORLD, &request_corner[4]);
        // receive from topright
        MPI_Irecv(&arr(0,arr.cols-1), 1, MPI_INT, topright, 7, MPI_COMM_WORLD, &request_corner[5]);
        // receive from downleft
        MPI_Irecv(&arr(arr.rows-1,0), 1, MPI_INT, downleft, 6, MPI_COMM_WORLD, &request_corner[6]);
        // receive from downright
        MPI_Irecv(&arr(arr.rows-1,arr.cols-1), 1, MPI_INT, downright, 5, MPI_COMM_WORLD, &request_corner[7]);

        MPI_Waitall(8, request_corner, MPI_STATUS_IGNORE);

        // only after information is sent, the function can be called. Or the result will be error.
        arr.basic_evolution();

        // ensure all processors are synchronization to update for next loop
        MPI_Barrier(MPI_COMM_WORLD);
        end_time = MPI_Wtime();
        
        double time_cost = end_time - start_time;
        
        // printing out time consuming if you want
        if (outtime)
        {
            double * tmp_time = new double;
            MPI_Reduce(&time_cost, tmp_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            sum_time += *tmp_time;
            
            if (n == loopnum - 1 && myid == 0)
            {
                if(periodic)
                    printf("Grid ghost time consuming %f\n", sum_time / numprocs);
                else
                    printf("Grid non-periodic time consuming %f\n", sum_time / numprocs);
            }
        }

        // output file if you want
        if (outfile)
        {
            gather_all_data(myid, numprocs, grid_cols, row_size, col_size, arr);
            MPI_Barrier(MPI_COMM_WORLD);
            if (myid == 0)
                file_out(n + 1, time_cost, numprocs);
        }
        
        delete [] buf_left;
        delete [] buf_top;
        delete [] buf_right;
        delete [] buf_down;
        delete [] buf_rec_left;
        delete [] buf_rec_right;
    }
    delete [] request;
    delete [] request_corner;
}
    
/*
 Divide processes into a grid of size m x n (ensure that m and n are integers that are as close to one another as possible)
 */
void find_dimensions(int numprocs, int myid, int &rows, int &columns)
{
    int min_gap =numprocs;
    int top = sqrt(numprocs) + 1;
    for (int i = 1; i <= top; i++)
    {
        if (numprocs % i == 0)
        {
            int gap = abs(numprocs / i - i);

            if (gap < min_gap)
            {
                min_gap = gap;
                rows = i;
                columns = numprocs / i;
            }
        }
    }
}

// split the rows of original matrix to nearly equal number of rows in each processor (namely all of them are equal except last processor in rows)
vector<int> grid::cut_matrix_row(int numprocs)
{
    vector<int> res;
    int size_to_split = this->rows - 2;
    row_all_except_last = size_to_split / numprocs;
    row_last_num = size_to_split - (numprocs - 1) * (size_to_split / numprocs);
    
    for ( int i = 0; i != numprocs - 1; i++ )
        res.push_back(row_all_except_last);
    res.push_back(row_last_num);
    
    return res;
}

// split the rows of original matrix to nearly equal number of cols in each processor (namely all of them are equal except last processor in columns)
vector<int> grid::cut_matrix_col(int numprocs)
{
    vector<int> res;
    int size_to_split = this->cols - 2;
    col_all_except_last = size_to_split / numprocs;
    col_last_num = size_to_split - (numprocs - 1) * (size_to_split / numprocs);
    
    for ( int i = 0; i != numprocs - 1; i++ )
        res.push_back(col_all_except_last);
    res.push_back(col_last_num);
    
    return res;
}


void grid::gather_all_data(int myid, int numprocs, int grid_cols, vector<int> & row_size, vector<int> & col_size, grid & arr)
{
    // define receive amount and dispersion arrays for gather all small matrix to master processor
    int * rec_count = new int [numprocs];
    int * disp = new int [numprocs];
    
    for (int i = 0; i != numprocs; i++)
    {
        rec_count[i] = (row_size[ i / grid_cols ] + 2) * (col_size[ i % grid_cols ] + 2);
        disp[0] = 0;
        
        if (i != 0)
            disp[i] = disp[i-1] + rec_count[i-1];
    }
    
    int sum = 0;
    
    for (int i = 0; i != numprocs; i++)
        sum += rec_count[i];
    
    int * recv_all = new int [sum];
    
    // gather all data to process 0 to print or save to file for outputing
    MPI_Gatherv(&arr(0,0), rec_count[myid], MPI_INT, recv_all, rec_count, disp, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (myid == 0)
    {
        // define disperse amount to set values for each temporary matrix
        int position;
        // find the corresponding position of original matrix in different small matrix in processor so that we can set the values in orignal matrix
        int row_pos_start;
        int col_pos_start;
        
        // start loop to gather values in small matrix in all processors to orignal matrix
        for (int i = 0; i != numprocs; i++)
        {
            Matrix tmp_matrix(row_size[i / grid_cols], col_size[i % grid_cols]);
            position = disp[i];
            
            // store values from different processor into temporary matrix
            for (int j = 0; j != rec_count[i]; j++)
                tmp_matrix.values[j] = recv_all[position + j];
            
            row_pos_start = (i / grid_cols) * row_size[0] + 1;
            col_pos_start = (i % grid_cols) * col_size[0] + 1;
            
            // store values in temporary matrix to original matrix
            for (int j = 0; j != tmp_matrix.rows-2; j++)
                for (int k = 0; k != tmp_matrix.cols-2; k++)
                    (*this)( row_pos_start + j, col_pos_start + k ) = tmp_matrix( 1 + j, 1 + k );
        }
    }
    delete [] rec_count;
    delete [] disp;
    delete [] recv_all;
}

void grid::file_out(int num, double running_time, int numprocs, int loopnum)
{
    ofstream fout;
    string filename;
    if (periodic)
        filename = "grid_ghost" + to_string(num) + ".txt";
    else
        filename = "grid_non_periodic" + to_string(num) + ".txt";
    
    fout.open(filename);
    fout << rows-2 << '\n';
    fout << cols-2 << '\n';
    fout << running_time << '\n';
    fout << numprocs << '\n';
    fout << loopnum << '\n';
    
    for (int i = 1; i != rows - 1; i++)
    {
        for (int j = 1; j != cols - 1; j++)
        {
            fout << (*this)(i,j) << ' ';
        }
        if ( i < rows - 2)
            fout << '\n';
    }
}
