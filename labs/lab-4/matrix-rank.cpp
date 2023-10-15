#include <valarray>
#include <string>
#include "matrix-rank.h"

using namespace std;

vector<vector<size_t>> GenerateCombinations(size_t matrixSize, size_t order) {
  string bitmask(order, 1);
  bitmask.resize(matrixSize, 0);
  auto result = vector<vector<size_t>>();
  do {
    auto combination = vector<size_t>();
    for (int i = 0; i < matrixSize; ++i) {
      if (bitmask[i]) combination.push_back(i);
    }
    result.push_back(combination);
  } while (prev_permutation(bitmask.begin(), bitmask.end()));
  return result;
}

int CalculateDeterminant(vector<vector<int>> matrix) {
  int num1, num2, index;
  auto total = 1;
  auto determinant = 1;
  size_t size = matrix.size();

  int temp[size + 1];

  for (size_t i = 0; i < size; i++) {
    index = i;

    while (index < size && matrix[index][i] == 0)
      index++;

    if (index == size)
      continue;

    if (index != i) {
      for (size_t j = 0; j < size; j++) {
        swap(matrix[index][j], matrix[i][j]);
      }
      determinant = determinant * pow(-1, index - i);
    }

    for (size_t j = 0; j < size; j++)
      temp[j] = matrix[i][j];

    for (size_t j = i + 1; j < size; j++) {
      num1 = temp[i];
      num2 = matrix[j][i];

      for (size_t k = 0; k < size; k++)
        matrix[j][k] = (num1 * matrix[j][k]) - (num2 * temp[k]);

      total = total * num1;
    }
  }

  for (size_t i = 0; i < size; i++)
    determinant = determinant * matrix[i][i];

  return determinant / total;
}

unsigned long long CalculateRank(vector<vector<int>> matrix, int threadCount) {
  auto matrixSize = matrix.size();
  if (matrixSize == 1) return 1;

  size_t rank = 0;
  for (size_t order = 2; order <= matrixSize; ++order) {
    auto combinations = GenerateCombinations(matrixSize, order);
# pragma omp parallel for default(none) num_threads(threadCount) shared(matrix, matrixSize, combinations, rank, order, threadCount)
    for (const auto &rows : combinations) {
# pragma omp parallel for default(none) num_threads(threadCount) shared(matrix, matrixSize, combinations, rank, order, rows)
      for (const auto &columns : combinations) {
        auto minor = vector<vector<int>>();
        for (size_t row : rows) {
          auto minorRow = vector<int>();
          for (size_t column : columns) {
            minorRow.push_back(matrix[row][column]);
          }
          minor.push_back(minorRow);
        }
        auto determinant = CalculateDeterminant(minor);
        if (determinant != 0)
          rank = order;
      }
    }
    if (rank < order)
      break;
  }
  return rank;
}