#include "mpi.h"

const int tag = 34;

int main(int argc, char *argv[]) {
  int size, rank;
  double start, end;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int value;
  if (rank > 0) {
    MPI_Recv(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Finalize();
    return EXIT_SUCCESS;
  }

  int arr[size];
  srand(time(nullptr) + rank);
  for (auto i = 0; i < size; ++i) arr[i] = rand() % 1025;

  start = MPI_Wtime();

  for (auto i = 0; i < size; ++i) {
    MPI_Send(&arr[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD);
  }

  MPI_Recv(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  end = MPI_Wtime();

  double runtime = end - start;
  printf("Total runtime: %lf\n", runtime);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
