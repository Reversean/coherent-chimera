#include <omp.h>
#include <cstdio>
#include <vector>
#include <numeric>
#include <string>
#include "miller-rabin-test.h"

using namespace std;

const short kCheckCount = 4;

inline bool IsPrime(unsigned long n) {
  if (n <= 1 || n == 4) return false;
  if (n <= 3) return true;

  unsigned long int d = n - 1;
  while (d % 2 == 0)
    d /= 2;
  for (int i = 0; i < kCheckCount; i++) {
    if (!MillerTest(d, n))
      return false;
  }
  return true;
}

vector<unsigned long> FindPrimes(int argc, char **argv) {
  auto threadLimit = stoi(argv[1]);
  auto availableThreads = omp_get_max_threads();
  auto threadsInUse = min(availableThreads, threadLimit);

  auto result = vector<unsigned long>();

#pragma omp parallel for num_threads(threadsInUse) default(none) shared(argc, argv, result)
  for (int i = 2; i < argc; i++) {
    auto number = stoul(argv[i]);
    if (IsPrime(number)) {
#pragma omp critical
      result.push_back(number);
    }
  }

  return result;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Specify thread number\n");
    return 1;
  }
  if (argc == 2) {
    printf("Specify numbers to test\n");
    return 1;
  }

  auto primes = FindPrimes(argc, argv);

  for (auto n : primes) {
    printf("%lu\n", n);
  }

  return 0;
}
