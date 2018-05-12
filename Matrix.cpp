#include "Matrix.hpp"
#include <ctype.h>
#include <vector>
using std::vector;

Matrix::Matrix() : rows(0), cols(0), array(0), trans(false) {};

Matrix::Matrix(int rows, int columns) 
  : rows(rows), cols(columns), trans(false) {
  array = new double[rows*cols];
  for (int i=0; i<rows*cols; i++) array[i]=0;
}

Matrix::Matrix(const Matrix& M) : rows(M.rows), cols(M.cols), trans(M.trans) {
  array = new double[rows*cols];
  for (int i=0; i<rows*cols; i++)
    array[i] = M.array[i];
}

Matrix::Matrix(Matrix&& M) : rows(M.rows), cols(M.cols), trans(M.trans) {
  array = M.array;
  M.array = nullptr;
}

Matrix::~Matrix() {
  if (array) delete [] array;
  array = 0;
}

Matrix Matrix::operator=(const Matrix& M) {
  if (array) delete [] array;
  rows = M.rows; cols = M.cols; 
  trans = M.trans;
  array = new double[rows*cols];
  for (int i=0;i<rows*cols; i++)
    array[i] = M.array[i];
}

Matrix Matrix::operator=(Matrix&& M) {

  cout << "here too\n";

  if (array) delete [] array;
  rows = M.rows; cols = M.cols; 
  trans = M.trans;
  array = M.array;
  M.array = nullptr;
}

void multiply(const Matrix& A, const Matrix& B, Matrix& C) {
  if (A.getCols()!=B.getRows() || A.getRows()!=C.getRows() || B.getCols()!=C.getCols())
    throw Matrix::MatrixMismatch();

  /*
  auto AT = A.trans ? CblasTrans : CblasNoTrans;
  auto BT = B.trans ? CblasTrans : CblasNoTrans;
  double ALPHA = 1, BETA = 0;
  cblas_dgemm(CblasRowMajor, AT, BT, A.getRows(), B.getCols(), A.getCols(), ALPHA, A.array, 
              A.getACols(), B.array, B.getACols(), BETA, C.array, C.getCols());
  */

  // For now.
  for (int i=0; i<A.getRows(); ++i)
    for (int k=0; k<B.getCols(); ++k) {
      C(i,k) = 0;
      for (int j=0; j<A.getCols(); ++j)
        C(i,k) += A(i,j)*B(j,k);
    }
}

void multiply(const Matrix& A, const float * const B, Matrix& C) {
  // Assumes that the dimensions of B are correct -> A.getCols()
  if (A.getRows()!=C.getRows() || C.getCols()!=1)
    throw Matrix::MatrixMismatch();

  for (int i=0; i<A.getRows(); ++i) {
    C(i,0) = 0;
    for (int j=0; j<A.getCols(); ++j)
      C(i,0) += A(i,j) * B[j];
  }
}

void multiply(const Matrix& A, const Matrix& B, float* C) {
  // Assumes that the dimensions of B are correct
  if (A.getCols()!=B.getRows() || B.getCols()!=1)
    throw Matrix::MatrixMismatch();

  for (int i=0; i<A.getRows(); ++i) {
    C[i] = 0;
    for (int j=0; j<A.getCols(); ++j)
      C[i] += A(i,j) * B(j,0);
  }
}

void multiply(const Matrix& A, const float * const B, float* C) {
  // Assumes that the dimensions of B and C are correct
  for (int i=0; i<A.getRows(); ++i) {
    C[i] = 0;
    for (int j=0; j<A.getCols(); ++j)
      C[i] += A(i,j) * B[j];
  }
}

void multiply(const double m, const Matrix& A, Matrix& B) {
  if (A.getRows()!=B.getRows() || A.getCols()!=B.getCols())
    throw Matrix::MatrixMismatch();
  for (int i=0; i<A.rows*A.cols; i++) 
    B.array[i] = m*A.array[i];
}

void timesEq(Matrix& A, const double m) {
  for (int i=0; i<A.rows*A.cols; i++)
    A.array[i] *= m;
}

void add(const Matrix& A, const Matrix& B, Matrix& C) {
  if (!Matrix::checkDims(A, B) || !Matrix::checkDims(A, C))
    throw Matrix::MatrixMismatch();
  for (int y=0; y<A.getRows(); y++)
    for (int x=0; x<A.getCols(); x++)
      C(y,x) = A(y,x)+B(y,x); // Unsafe version should be faster
}

void plusEq(Matrix& A, const Matrix& B) {
  if (!Matrix::checkDims(A, B))
    throw Matrix::MatrixMismatch();
  for (int y=0; y<A.getRows(); y++)
    for (int x=0; x<A.getCols(); x++)
      A(y,x) += B(y,x);
}

void subtract(const Matrix& A, const Matrix& B, Matrix& C) {
  if (!Matrix::checkDims(A, B) || !Matrix::checkDims(A,C))
    throw Matrix::MatrixMismatch();
  for (int y=0;y<A.getRows(); y++)
    for (int x=0; x<A.getCols(); x++) {
      C(y,x) = A(y,x)-B(y,x); // Unsafe version should be faster
      //C.at(y,x) = A.at(y,x)-B.at(y,x);
    }
}

void minusEq(Matrix& A, const Matrix& B) {
  if (!Matrix::checkDims(A, B))
    throw Matrix::MatrixMismatch();
  for (int y=0; y<A.getRows(); y++)
    for (int x=0; x<A.getCols(); x++)
      A(y,x) -=B(y,x);
}

void hadamard(const Matrix& A, const Matrix& B, Matrix& C) {
  if (!Matrix::checkDims(A,B)) throw Matrix::MatrixMismatch();
  if (A.array==0 || B.array==0 || C.array==0) throw Matrix::Unallocated(); // We could also have an impropper qref
  // Will only work if none are transposed
  for (int i=0; i<A.rows*A.cols; i++) C.array[i] = A.array[i]*B.array[i];
}

void apply(const Matrix& A, Function F, Matrix& C) {
  if (A.getCols()!=C.getCols() || A.getRows()!=C.getRows())
    throw Matrix::MatrixMismatch();
  
  for (int i=0; i<A.rows*A.cols; i++)
    C.array[i] = F(A.array[i]);
}

void Matrix::apply(Function F) {
  for (int i=0; i<rows*cols; ++i) array[i] = F(array[i]);
}

double& Matrix::at(int row, int col) {
  if (trans) { // Swap
    int temp = col;
    col = row;
    row = temp;
  }
  if (row<0 || row>=rows || col<0 || col>=cols) 
    throw MatrixOutOfBounds(); // The indentation here doesn't work
  return array[cols*row + col];
}

double Matrix::at(int row, int col) const {
  if (trans) { // Swap
    int temp = col;
    col = row;
    row = temp;
  }
  if (row<0 || row>=rows || col<0 || col>=cols)
    throw MatrixOutOfBounds(); // The indentation here doesn't work
  return array[cols*row + col];
}

double& Matrix::operator()(int row, int col) {
  if (trans) { // Swap
    int temp = col;
    col = row;
    row = temp;
  }
  return array[cols*row + col];
}

double Matrix::operator()(int row, int col) const {
  if (trans) { // Swap
    int temp = col;
    col = row;
    row = temp;
  }
  return array[cols*row + col];
}

double& Matrix::operator() (int row) {
  return operator()(row,0);
}

double Matrix::operator() (int row) const {
  return operator()(row,0);
}

double& Matrix::access_NT(int row, int col) {
  return array[cols*row + col];
}

double& Matrix::access_T(int row, int col) {
  return array[cols*col + row];
}

double Matrix::operator[](int index) {
  if (index<0 || index>=rows*cols)
    throw MatrixOutOfBounds();
  return array[index];
}

double Matrix::norm() const {
  double N = 0;
  for (int i=0; i<rows*cols; i++) N += array[i]*array[i];
  return sqrt(N);
}

double Matrix::max() const {
  double M = -1e9;
  for (int i=0; i<rows*cols; i++) 
    if (M < array[i]) M = array[i];
  return M;
}

double Matrix::min() const {
  double M = 1e9;
  for (int i=0; i<rows*cols; i++)
    if (M > array[i]) M = array[i];
  return M;
}

void Matrix::resize(int rows, int cols) {
  if (array) delete [] array; 
  this->rows = rows; this->cols = cols;
  array = new double[rows*cols];
}

void Matrix::reshape(int rows, int cols) {
  if (rows*cols != this->rows*this->cols)
    throw BadReshape();
  this->rows = rows; this->cols = cols;
}

void Matrix::randomUniform(double max) {
  for (int i=0; i<rows*cols; i++) array[i] = max*(2*drand48()-1);
}

void Matrix::randomNormal(double sigma, double mu) {
  for (int i=0; i<rows*cols; i++) array[i] = sigma*randNormal()+mu;
}

void Matrix::zero() {
  for (int i=0; i<rows*cols; i++) array[i] = 0;
}

void Matrix::qrel() {
  array = nullptr;
}

void Matrix::qref(Matrix& M) {
  // Assumes that matrix M is of the same dimensions as this matrix
  if (array) delete [] array;
  array = M.array;
}

std::ostream& operator<<(std::ostream& out, const Matrix& M) {
  int width = M.getCols(), height = M.getRows();
  out << "{";
  for (int y=0; y<height; y++) {
    out << "{";
    for (int x=0; x<width; x++) {
      out << M.at(y,x);
      if (x!=width-1) out << ",";
    }
    out << "}";
    if (y!=height-1) out << ",";
  }
  out << "}";
  return out;
}

std::istream& operator>>(std::istream& in, Matrix& M) {
  vector<vector<double>> array;
  char c;
  double num;
  bool closed = true, open = false;
  int rows = 0, cols = 0;
  in.get(c);
  while (!in.eof()) {
    // Get rid of whitespaces
    while (c==' ' || c=='\n' || c=='\r') in.get(c);
    // Check chars
    if (c=='{') {
      closed = false;
      if (open) {
	rows++;
	array.push_back(vector<double>());
      }
      open = true;
    }
    if (c=='}') {
      if (closed) break;
      else if (array.at(rows-1).size() > cols) cols = array.at(rows-1).size();
      closed = true;
    }
    if (isdigit(c)) {
      in.putback(c);
      in >> num;
      array.at(rows-1).push_back(num);
    }
    in.get(c);
  }
  
  M.resize(rows, cols);
  for (int i=0; i<rows; i++) {
    int j;
    for (j=0; j<array.at(i).size(); j++) {
      M.at(i,j) = array.at(i).at(j);
    }
    for (; j<cols; j++) { // Hopefully this won't happen, but just in case
      M.at(i,j) = array.at(i).at(j);
    }
  }
  return in;
}

inline bool Matrix::checkDims(const Matrix& A, const Matrix& B) {
  return A.rows==B.rows && A.cols==B.cols;
}
