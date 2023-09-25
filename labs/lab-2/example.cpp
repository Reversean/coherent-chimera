#include <cmath>
#include <cstdio>
#include <omp.h>

using namespace std;

const auto kThreadLimit = 4;

int main(int argc, char *argv[]) {
  auto availableThreads = omp_get_max_threads();
  auto threadsInUse = min(availableThreads, kThreadLimit);

#pragma omp parallel num_threads(threadsInUse)
  {
    auto count = omp_get_thread_num();
    auto ItsMe = omp_get_num_threads();
    printf("Hello, OpenMP! I am %d of %d\n", count, ItsMe);
  }

  return 0;
}
