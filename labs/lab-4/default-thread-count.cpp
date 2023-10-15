#include <cstdio>
#include <omp.h>
#include <string>
#include <vector>
#include "matrix-rank.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Specify matrix size\n");
    return 1;
  }
  auto matrixSize = stoi(argv[2]);
  if (argc < matrixSize * matrixSize + 1) {
    printf("Specify elements of matrix\n");
    return 1;
  }

  auto threadsInUse = omp_get_max_threads();

  vector<vector<int>> matrix = vector<vector<int>>();

  for (auto i = 0; i < matrixSize; ++i) {
    auto row = vector<int>();
    for (auto j = 1; j <= matrixSize; ++j) {
      auto element = stoi(argv[i * matrixSize + j + 1]);
      row.push_back(element);
    }
    matrix.push_back(row);
  }
  auto rank = CalculateRank(matrix, threadsInUse);
  printf("Matrix rank: %lld", rank);
  return 0;
}
