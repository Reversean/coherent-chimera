#include <omp.h>
#include "matrix.h"

using namespace std;

double MultiplyMatrices(char *matrixAPath, char *matrixBPath, char *outputMatrixPath) {
  auto matrixA = Matrix::read(matrixAPath);
  auto matrixB = Matrix::read(matrixBPath);
  auto startTime = omp_get_wtime();
  auto outputMatrix = Matrix::multiply(matrixA, matrixB);
  auto endTime = omp_get_wtime();
  auto runtime = endTime - startTime;

  outputMatrix->write(outputMatrixPath);
  return runtime;
}

void TestMultiplyMatrices(char *matrixAPath, char *matrixBPath, char *outputMatrixPath) {
  double totalRuntime;
  for (int i = 0; i < 10; ++i) {
    auto runtime= MultiplyMatrices(matrixAPath, matrixBPath, outputMatrixPath);
    printf("| %d | %lf |\n", i + 1, runtime);
    totalRuntime += runtime;
  }
  printf("\nAverage: %lf\n", totalRuntime / 10);
}

int main(int argc, char *argv[]) {
  char *matrixAPath = argv[1];
  char *matrixBPath = argv[2];
  char *outputMatrixPath = argv[3];
  TestMultiplyMatrices(matrixAPath, matrixBPath, outputMatrixPath);
//  auto runtime= MultiplyMatrices(matrixAPath, matrixBPath, outputMatrixPath);
//  printf("|    | %lf |\n", runtime);
  return 0;
}
