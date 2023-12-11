#ifndef DECODER_COURSE_WORK_MATRIX_H_
#define DECODER_COURSE_WORK_MATRIX_H_

#include <cstdio>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

//struct MatrixStr {
//  size_t rowCount;
//  size_t columnCount;
//  int** data;
//};

struct MatrixStr {
  alignas(32) size_t rowCount;
  alignas(32) size_t columnCount;
  alignas(32) int** data;
};

class Matrix {
 private:
  MatrixStr str;
 public:
  Matrix(vector<vector<int>> data);
  Matrix(size_t rowCount, size_t columnCount);
  size_t getColumnCount() const;
  size_t getRowCount() const;
  int** getData() const;
  void write(char *matrixFilePath);
  static Matrix *multiply(Matrix *matrixA, Matrix *matrixB);
  static Matrix *read(char *matrixFilePath);
};

#endif //DECODER_COURSE_WORK_MATRIX_H_
