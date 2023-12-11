#include "matrix.h"

Matrix::Matrix(vector<vector<int>> data)
    : str{data.size(), data[0].size(), new int *[str.rowCount]} {
  for (int i = 0; i < str.rowCount; i++) {
    str.data[i] = new int[str.columnCount];
  }
  for (int i = 0; i < str.columnCount; i++) {
    for (int j = 0; j < str.rowCount; j++) {
      str.data[j][i] = data[j][i];
    }
  }
}

Matrix::Matrix(size_t rowCount, size_t columnCount)
    : str{rowCount, columnCount, new int *[str.rowCount]} {
  for (int i = 0; i < str.rowCount; i++) {
    str.data[i] = new int[str.columnCount];
    for (int j = 0; j < str.columnCount; j++) {
      str.data[i][j] = 0;
    }
  }
}

size_t Matrix::getColumnCount() const {
  return str.columnCount;
}

size_t Matrix::getRowCount() const {
  return str.rowCount;
}

int **Matrix::getData() const {
  return str.data;
}

void Matrix::write(char *matrixFilePath) {
  ofstream outputFile;
  outputFile.exceptions(ofstream::badbit);
  auto data = getData();
  try {
    outputFile.open(matrixFilePath);
    for (int i = 0; i < getRowCount(); ++i) {
      for (int j = 0; j < getColumnCount(); ++j) {
        outputFile << data[i][j];
        if (j != getColumnCount() - 1) {
          outputFile << ' ';
        }
      }
      if (i != getRowCount() - 1) {
        outputFile << '\n';
      }
    }
  } catch (const ofstream::failure &e) {
    printf("Exception opening/writing file");
  }
  outputFile.close();
}

Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  int temp;
#pragma omp parallel for shared(matrixA, matrixB, result) private(temp) default(none)
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      temp = matrixA->getData()[i][j];
#pragma omp simd
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        result->getData()[i][k] += temp * matrixB->getData()[j][k];
      }
    }
  }
  return result;
}

//Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
//  auto matrixARowCount = matrixA->getRowCount();
//  auto matrixBRowCount = matrixB->getRowCount();
//  auto matrixBColumnCount = matrixB->getColumnCount();
//  auto result = new Matrix(matrixARowCount, matrixBColumnCount);
//  auto matrixAData = matrixA->getData();
//  auto matrixBData = matrixB->getData();
//  auto resultData = result->getData();
//#pragma omp parallel for shared(resultData, matrixAData, matrixBData, matrixARowCount, matrixBRowCount, matrixBColumnCount) default(none)
//  for (int k = 0; k < matrixARowCount; ++k) {
//    for (int j = 0; j < matrixBRowCount; ++j) {
//      for (int i = 0; i < matrixBColumnCount; ++i) {
//        resultData[k][i] += matrixBData[j][i] * matrixAData[k][j];
//      }
//    }
//  }
//  return result;
//}

Matrix *Matrix::read(char *matrixFilePath) {
  ifstream inputFile;
  inputFile.exceptions(ifstream::badbit);
  vector<vector<int>> vectorMatrix;
  try {
    inputFile.open(matrixFilePath);
    string value;
    char ch;
    vector<int> row;
    while (inputFile.get(ch)) {
      if (ch == '\n') {
        row.push_back(stoi(value));
        value.clear();
        vectorMatrix.push_back(row);
        row.clear();
        continue;
      }
      if (ch == ' ') {
        row.push_back(stoi(value));
        value.clear();
        continue;
      }
      value.push_back(ch);
    }
    if (!value.empty()) {
      row.push_back(stoi(value));
    }
    if (!row.empty()) {
      vectorMatrix.push_back(row);
    }
  } catch (const ifstream::failure &e) {
    printf("Exception opening/reading file");
  }
  inputFile.close();
  return new Matrix(vectorMatrix);
}

extern "C" {
  void MultiplyMatrices(char *matrixAPath, char *matrixBPath, char *outputMatrixPath) {
    auto matrixA = Matrix::read(matrixAPath);
    auto matrixB = Matrix::read(matrixBPath);
    auto outputMatrix = Matrix::multiply(matrixA, matrixB);
    outputMatrix->write(outputMatrixPath);
  }
}
