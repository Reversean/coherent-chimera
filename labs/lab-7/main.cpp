#include <mpi.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "matrices.h"

using namespace std;
using namespace chrono;

string sizesPath = "sizes";
string matrixAPath = "matrixA";
string matrixBPath = "matrixB";

int main(int argc, char *argv[]) {

  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//  auto matrices = Matrices::fromFiles(&sizesPath, &matrixAPath, &matrixBPath);

  int n1, n2, n3;
  ifstream fileSizes(sizesPath);
  fileSizes >> n1;
  fileSizes >> n2;
  fileSizes >> n3;
  fileSizes.close();

  int matrixA[n1][n2];
  int matrixB[n2][n3];

  ifstream matrixAFile(matrixAPath);
  for (int i = 0; i < n1; ++i) {
    for (int j = 0; j < n2; ++j) {
      matrixAFile >> matrixA[i][j];
    }
  }
  matrixAFile.close();

  ifstream matrixBFile(matrixBPath);
  for (int i = 0; i < n1; ++i) {
    for (int j = 0; j < n2; ++j) {
      matrixAFile >> matrixA[i][j];
    }
  }
  matrixBFile.close();

  int result[n1][n3];
  int dim = n2;
  int ProcPartSize = dim / size;
  int ProcPartElem = ProcPartSize * dim;
  int *bufA = new int[ProcPartElem];
  int *bufB = new int[ProcPartElem];
  int *bufResult = new int[ProcPartElem];
  auto ProcPart = dim / size;
  auto part = ProcPart * dim;

  double multTime;
  auto multStart = high_resolution_clock::now();

  MPI_Scatter(&matrixA, part, MPI_INT, bufA, part, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(&matrixB, part, MPI_INT, bufB, part, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 1) {
    printf("1, %d\n", bufA[0]);
    printf("1, %d\n", bufA[20]);
  }
  if (rank == 0) {
    printf("0, %d\n", bufA[0]);
    printf("0, %d\n", bufA[20]);
  }

  int index;
  int temp = 0;
  for (int i = 0; i < ProcPartSize; i++) {
    for (int j = 0; j < ProcPartSize; j++) {
      for (int k = 0; k < dim; k++) {
        temp += bufA[i * dim + k] * bufB[j * dim + k];
      }
      bufResult[i * dim + j + ProcPartSize * rank] = temp;
      temp = 0;
    }
  }

  int NextProc;
  int PrevProc;
  for (int p = 1; p < size; p++) {
    NextProc = rank + 1;
    if (rank == size - 1) {
      NextProc = 0;
    }
    PrevProc = rank - 1;
    if (rank == 0) {
      PrevProc = size - 1;
    }
    MPI_Sendrecv_replace(bufB, part, MPI_INT, NextProc, 0, PrevProc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    temp = 0;
    for (int i = 0; i < ProcPartSize; i++) {
      for (int j = 0; j < ProcPartSize; j++) {
        for (int k = 0; k < dim; k++) {
          temp += bufA[i * dim + k] * bufB[j * dim + k];
        }
        if (rank - p >= 0)
          index = rank - p;
        else index = (size - p + rank);
        bufResult[i * dim + j + index * ProcPartSize] = temp;
        temp = 0;
      }
    }
  }


  MPI_Gather(bufResult, ProcPartElem, MPI_INT, result, ProcPartElem, MPI_INT, 0, MPI_COMM_WORLD);

  auto multEnd = high_resolution_clock::now();
  multTime = duration<double, micro>(multEnd - multStart).count();

  if (rank == 0) {
    cout << "Done!" << endl;
    cout << "Procs number: " << size << endl;
    cout << "Parallel matrix multiplication time: " << multTime << " microseconds" << endl;

    ofstream outputFile("result");
    for (int i = 0; i < n1; i++) {
      for (int j = 0; j < n3; j++) {
        outputFile << result[i][j] << " ";
      }
      outputFile << endl;
    }
    outputFile.close();
  }

  MPI_Finalize();
  return 0;
}