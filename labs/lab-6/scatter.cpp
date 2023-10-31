#include "mpi.h"

const int tag = 34;

int main(int argc, char *argv[]) {
  int size, rank;
  double start, end;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int arr[size];
  if (rank == 0) {
    srand(time(nullptr) + rank);
    for (auto i = 0; i < size; ++i) arr[i] = rand() % 1025;

    start = MPI_Wtime();
  }

  int value;
  MPI_Scatter(&arr, 1, MPI_INT, &value, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    end = MPI_Wtime();

    double runtime = end - start;
    printf("Total runtime: %lf\n", runtime);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
