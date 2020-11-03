#include "CSRMatrix.h"

// Constructor - using an initialisation list here
template <class T>
CSRMatrix<T>::CSRMatrix(int rows, int cols, int nnzs, bool preallocate): Matrix<T>(rows, cols, false), nnzs(nnzs)
{
   // If we don't pass false in the initialisation list base constructor, it would allocate values to be of size
   // rows * cols in our base matrix class
   // So then we need to set it to the real value we had passed in
   this->preallocated = preallocate;

   // If we want to handle memory ourselves
   if (this->preallocated)
   {
      // Must remember to delete this in the destructor
      this->values = new T[this->nnzs];
      this->row_position = new int[this->rows+1];
      this->col_index = new int[this->nnzs];
   }
}

// Constructor - now just setting the value of our T pointer
template <class T>
CSRMatrix<T>::CSRMatrix(int rows, int cols, int nnzs, T *values_ptr, int *row_position, int *col_index): Matrix<T>(rows, cols, values_ptr), nnzs(nnzs), row_position(row_position), col_index(col_index)
{}

// destructor
template <class T>
CSRMatrix<T>::~CSRMatrix()
{
   // Delete the values array
   if (this->preallocated){
      delete[] this->row_position;
      delete[] this->col_index;
   }
   // The super destructor is called after we finish here
   // This will delete this->values if preallocated is true
}

// deep copy constructor
template <class T>
CSRMatrix<T>::CSRMatrix(const CSRMatrix<T> & A) : Matrix<T>(A)
{
    nnzs = A.nnzs;
    row_position = new int [A.rows+1];
    col_index = new int [A.nnzs];
    
    for (int i = 0; i != A.rows+1; i++)
        row_position[i] = A.row_position[i];
    for (int i = 0; i != A.nnzs; i++)
        col_index[i] = A.col_index[i];
}

// copy assignment
template <class T>
CSRMatrix<T> & CSRMatrix<T>::operator=(const CSRMatrix<T> & A)
{
    if( this == &A)
        return *this;
    Matrix<T>::operator=(A);
    delete [] row_position;
    delete [] col_index;
    for (int i = 0; i != A.rows+1; i++)
        row_position[i] = A.row_position[i];
    for (int i = 0; i != A.nnzs; i++)
        col_index[i] = A.col_index[i];
    return *this;
}

// Move constructor
template <class T>
CSRMatrix<T>::CSRMatrix(CSRMatrix<T> && A) : Matrix<T>(A), nnzs(A.nnzs)
{
    for (int i=0; i < A.rows; i++)
           for (int j=0; j < A.cols; j++)
               // Copy across the entries from Matrix A
               this->values[i * this->cols + j] = A.values[i * A.cols + j];
           
    A.rows = 0;
    A.cols = 0;

    A.values = nullptr;
    
    for (int i = 0; i != A.rows+1; i++)
        row_position[i] = A.row_position[i];
    for (int i = 0; i != A.nnzs; i++)
        col_index[i] = A.col_index[i];
    
    A.row_position = nullptr;
    A.col_index = nullptr;
    A.nnzs = 0;
}

// Move assignment
template <class T>
CSRMatrix<T> & CSRMatrix<T>::operator=(CSRMatrix<T> && A)
{
    if (this == &A)
        return *this;
        
    delete [] this->values;

    this->values = A.values;

    this->rows = A.rows;
    this->cols = A.cols;

    A.rows = 0;
    A.cols = 0;

    A.values = nullptr;
    
    row_position = A.row_position;
    col_index = A.col_index;
    nnzs = A.nnzs;
    
    A.row_position = nullptr;
    A.col_index = nullptr;
    A.nnzs = 0;

    return *this;
}














// Explicitly print out the values in values array as if they are a matrix
template <class T>
void CSRMatrix<T>::printMatrix()
{
   std::cout << "Printing matrix" << std::endl;
   std::cout << "Values: ";
   for (int j = 0; j< this->nnzs; j++)
   {
      std::cout << this->values[j] << " ";
   }
   std::cout << std::endl;
   std::cout << "row_position: ";
   for (int j = 0; j< this->rows+1; j++)
   {
      std::cout << this->row_position[j] << " ";
   }
   std::cout << std::endl;
   std::cout << "col_index: ";
   for (int j = 0; j< this->nnzs; j++)
   {
      std::cout << this->col_index[j] << " ";
   }
   std::cout << std::endl;
}

// Do a matrix-vector product
// output = this * input
template<class T>
void CSRMatrix<T>::matVecMult(double *input, double *output)
{
   if (input == nullptr || output == nullptr)
   {
      std::cerr << "Input or output haven't been created" << std::endl;
      return;
   }

   // Set the output to zero
   for (int i = 0; i < this->rows; i++)
   {
      output[i] = 0.0;
   }

//   int val_counter = 0;
   // Loop over each row
   for (int i = 0; i < this->rows; i++)
   {
      // Loop over all the entries in this col
      for (int val_index = this->row_position[i]; val_index < this->row_position[i+1]; val_index++)
      {
         // This is an example of indirect addressing
         // Can make it harder for the compiler to vectorise!
         output[i] += this->values[val_index] * input[this->col_index[val_index]];
      }
   }
}


//// Do matrix matrix multiplication
//// output = this * mat_right
//template <class T>
//void CSRMatrix<T>::matMatMult(CSRMatrix<T>& mat_right, CSRMatrix<T>& output)
//{
//
//   // Check our dimensions match
//   if (this->cols != mat_right.rows)
//   {
//      std::cerr << "Input dimensions for matrices don't match" << std::endl;
//      return;
//   }
//
//   // Check if our output matrix has had space allocated to it
//   if (output.values != nullptr)
//   {
//      // Check our dimensions match
//      if (this->rows != output.rows || mat_right.cols != output.cols)
//      {
//         std::cerr << "Input dimensions for matrices don't match" << std::endl;
//         return;
//      }
//   }
//   // The output hasn't been preallocated, so we are going to do that
//   else
//   {
//      std::cerr << "OUTPUT HASN'T BEEN ALLOCATED" << std::endl;
//
//   }
//
//    T temp_value;
//
//    vector<T> output_CSR_values;
//    vector<T> output_CSR_row_position = {0};
//    vector<T> output_CSR_col_index;
//
//    // 计算每次循环中每一行的数值个数
//    int count_rows_num = 0;
//
//
//
//   // HOW DO WE SET THE SPARSITY OF OUR OUTPUT MATRIX HERE??
//    for (int i = 0; i < this->rows; i++)
//    {
////        count_rows_num = 0;
//        for ( int j = 0; j < this->cols; j++)
//        {
//            // 左矩阵第 i 行有数值的个数
//            int left_i_row_num = row_position[i+1] - row_position[i];
//
//            // 对应的左矩阵 i 行的 列为
//            for (int k = row_position[i]; k < row_position[i+1]; k++)
//                col_index[k];
//
//
//            // 得到右矩阵出现第 j 列的数值 对应的前面有多少个values！
//            vector<int> count_vec;
//            int count;
//            for (count = 0; count != mat_right.nnzs; count++)
//                if (mat_right.col_index[count] == j)
//                    count_vec.push_back(count);
//            // 得到右矩阵出现 j 列数值对应的行！
//
//
//            vector<int> rows_index;
//            int rows_ind;
//            int k;
//
//            // 有问题，rows_inde可能重复，无法解决
//            for (rows_ind = 0, k = 0; (rows_ind <= this->rows) && (k < (int) count_vec.size()) ;)
//            {
//                if (mat_right.row_position[rows_ind] >= count_vec[k])
//                {
//                    if (rows_ind == mat_right.rows)
//                    {
//                        rows_index.push_back(rows_ind - 1);
//                        k++;
//                    }
//                    else
//                    {
//                        rows_index.push_back(rows_ind);
//                        k++;
//                    }
//                }
//                else
//                    rows_ind++;
//            }
//            // 得到右矩阵第 j 列 出现数值在 rows_index 行(计算机内部索引方法）
//            if (i == 0 && j == 2)
//            {cout << "-----";
//                    cout << rows_index << endl;
//                cout << "------";
//            }
////            cout << "i is :" << i << endl << "j is : " << j << endl;
//            temp_value = 0;
//            // Loop over all the entries in this col
//            for (int val_index = this->row_position[i]; val_index < this->row_position[i+1]; val_index++)
//            {
//              // This is an example of indirect addressing
//              // Can make it harder for the compiler to vectorise!
//
//                // 左矩阵出现值对应的列，用来查找右矩阵对应列是否有值！
//                int num_to_find = col_index[val_index];
//
//
//                if ( find(rows_index.begin(), rows_index.end(), num_to_find) != rows_index.end() )
//                {
//                    // 寻找和该次左矩阵数值对应的右矩阵的那一行
//                    for (int q = 0; q != rows_index.size(); q++)
//                    {
//                        // 只能查找到第一次出现的，所以不能general
//                        if (col_index[val_index] == rows_index[q])
//                        {
//                            temp_value += this->values[val_index] * mat_right.values[count_vec[q]];
//
//                        }
//                    }
//                }
//            }
//
//
//            if (temp_value != 0)
//            {
//                output_CSR_values.push_back(temp_value);
//                count_rows_num++;
//                output_CSR_col_index.push_back(j);
//            }
//        }
//
//        output_CSR_row_position.push_back(count_rows_num);
//    }
////    delete [] output.values;
////    delete [] output.row_position;
////    delete [] output.col_index;
//    output.nnzs = output_CSR_values.size();
//    output.values = new double [output_CSR_values.size()];
//    for (int i = 0; i != output_CSR_values.size(); i++)
//        output.values[i] = output_CSR_values[i];
//    output.row_position = new int [output_CSR_row_position.size()];
//    for (int i = 0; i != output_CSR_values.size(); i++)
//        output.row_position[i] = output_CSR_row_position[i];
//    output.col_index = new int [output_CSR_col_index.size()];
//    for (int i = 0; i != output_CSR_values.size(); i++)
//        output.col_index[i] = output_CSR_col_index[i];
//}
//


//template <class U>
//CSRMatrix<U> operator~(const CSRMatrix<U> & A)
//{
//    CSRMatrix<U> B(A.cols, A.rows, A.nnzs, false);
//
//    // 查找 A 的每一列，然后转换为B的对应行！
//
//    // 找 A 的列
//
//    // 计数确定数值所在的rows_index
//    int count;
//
//    // 储存row_index的vector
//    vector<int> row_index;
//
//    vector<U>
//    // 遍历 A 的 所有列
//    for (int col_index_A = 0; col_index_A != A.cols; col_index_A++)
//    {
//        count = 0;
//        // 遍历 A.col_index 寻找对应的col    同时    也代表A.values的位置
//        for (int i = 0; i != A.nnzs; i++)
//        {
//            if (col_index_A == i)
//            {
//                // 遍历 A.row_position 寻找该数值的行数
//                for (int row_ind = 0; row_ind != A.rows; row_ind++)
//                    if (i >= row_ind)
//                    {
//                        // 该数值对应的行！
//                        row_index.push_back(row_ind);
//                    }
//            }
//            count++;
//        }
//    }
//}
