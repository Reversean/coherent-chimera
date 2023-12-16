#include <fstream>
#include "matrices.h"

using namespace std;

int Matrices::getN1() {
  return matrixA.size();
}

int Matrices::getN2() {
  return matrixB.size();
}

int Matrices::getN3() {
  return matrixB[0].size();
}

Matrices Matrices::fromFiles(string *sizesFilePath, string *matrixAFilePath, string *matrixBFilePath) {
  return Matrices(sizesFilePath, matrixAFilePath, matrixBFilePath);
}

Matrices::Matrices(string *sizesPath, string *matrixAPath, string *matrixBPath) {
  int n1, n2, n3;
  ifstream fileSizes(*sizesPath);
  fileSizes >> n1;
  fileSizes >> n2;
  fileSizes >> n3;
  fileSizes.close();

  ifstream matrixAFile(*matrixAPath);
  for (int i = 0; i < n1; ++i) {
    vector<int> temp;
    for (int j = 0; j < n2; ++j) {
      int value;
      matrixAFile >> value;
      temp.push_back(value);
    }
    matrixA.push_back(temp);
  }
  matrixAFile.close();

  ifstream matrixBFile(*matrixBPath);
  for (int i = 0; i < n1; ++i) {
    vector<int> temp;
    for (int j = 0; j < n2; ++j) {
      int value;
      matrixBFile >> value;
      temp.push_back(value);
    }
    matrixB.push_back(temp);
  }
  matrixBFile.close();
}
