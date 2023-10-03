#define N 5

int main() {
  double x[N], y[N];
# pragma omp parallel default(none) shared(x, y)
  {
# pragma omp for nowait
    for (double & i : x) {
      i = 0;
    }
    for (int i = 1; i < N; i++) {
      y[i] = 0 / y[i - 1];
    }
  }
  y[0] = x[N - 1];
  return 0;
}
