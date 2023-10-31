# Лабораторная работа 6

1. Реализовать алгоритм решения задания с использованием функций коллективного обмена в MPI, замерить время выполнения
   программы, если возможно, вычислить эффективность.
2. Реализовать алгоритм решения задания с использованием функций двухточечного обмена в MPI. Отладить программу.
   Замерить время выполнения программы, если возможно, вычислить эффективность.
3. Сравнить эффективность реализаций, построить графики, сделать выводы.

**Вариант 5**

Реализовать рассылку массива значений на n процессов по принципу i-ое значение i-му процессу с помощью двухточечных
обменов. Эффективность реализации сравнить с функцией `MPI_Scatter()`.

## Двухточечные обмены

```c++
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
```

## Коллективный обмен (MPI_Scatter)

```c++
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
```

## Сравнение результатов

|    | point-to-point | scatter  |
|----|----------------|----------|
| 2  | 0.000098       | 0.000108 |
| 4  | 0.020130       | 0.010129 |
| 6  | 0.040069       | 0.020485 |
| 8  | 0.040177       | 0.020048 |
| 10 | 0.060324       | 0.030300 |
| 12 | 0.069989       | 0.029420 |
| 14 | 0.077295       | 0.017709 |
| 16 | 0.096580       | 0.020386 |
| 18 | 0.110926       | 0.036504 |
| 20 | 0.090084       | 0.040498 |
| 22 | 0.125371       | 0.040440 |
| 24 | 0.149996       | 0.030379 |
| 26 | 0.142169       | 0.030536 |
| 28 | 0.146130       | 0.039886 |
| 30 | 0.157899       | 0.030452 |
| 32 | 0.172033       | 0.034028 |
