#include <mpi.h>
#include <iostream>
#include <algorithm>

using namespace std;

const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz',. ";
const char key[] = "defghijklmnopqrstuvwxyz',. ABCDEFGHIJKLMNOPQRSTUVWXYZabc";
const char encodedMessage[] = "pdqnlqgclvcghdgbceorrgclvcixhobckhooclvcixoob";

const int messageSizeTag = 0;
const int messagePartTag = 1;

void ManageDecoding(int size) {
  auto workerCount = size - 1;
  printf("Workers count: %d\n", workerCount);
  printf("Alphabet: %s\n", alphabet);
  printf("Key: %s\n", key);
  printf("Encoded message: %s\n", encodedMessage);

  if (workerCount < 1) return;

  auto messageSize = sizeof(encodedMessage);
  auto defaultPartSize = messageSize / workerCount;
  auto remainder = messageSize % workerCount;

  for (int worker = 1; worker < size; worker++) {
    size_t partSize;
    if (worker == workerCount) {
      partSize = defaultPartSize + remainder;
    } else {
      partSize = defaultPartSize;
    }
    MPI_Send(&partSize, 1, MPI_UNSIGNED_LONG, worker, messageSizeTag, MPI_COMM_WORLD);
    MPI_Send(&encodedMessage[defaultPartSize * (worker - 1)], partSize, MPI_CHAR, worker, messagePartTag, MPI_COMM_WORLD);
  }

  MPI_Status status;
  char decodedMessage[messageSize];
  for (int worker = 1; worker < size; worker++) {
    size_t partSize;
    MPI_Recv(&partSize, 1, MPI_UNSIGNED_LONG, worker, messageSizeTag, MPI_COMM_WORLD, &status);
    MPI_Recv(&decodedMessage[defaultPartSize * (worker - 1)], partSize, MPI_CHAR, worker, messagePartTag, MPI_COMM_WORLD, &status);
  }

  printf("Decoded message: %s\n", decodedMessage);
}

void HandleDecoding(int rank) {
  MPI_Status status;

  size_t partSize;
  MPI_Recv(&partSize, 1, MPI_UNSIGNED_LONG, 0, messageSizeTag, MPI_COMM_WORLD, &status);
  printf("Thread %d: Encoded message size: %zu\n", rank, partSize);

  char message[partSize];
  MPI_Recv(message, partSize, MPI_CHAR, 0, messagePartTag, MPI_COMM_WORLD, &status);
  printf("Thread %d: Encoded message part: %s\n", rank, message);

  char decodedMessage[partSize];
  auto alphabetSize = sizeof(alphabet);
  for (auto i = 0; i < partSize; i++) {
    for (auto j = 0; j < alphabetSize; ++j) {
      if (message[i] == key[j]) {
        decodedMessage[i] = alphabet[j];
      }
    }
  }
  printf("Thread %d: Decoded message part: %s\n", rank, decodedMessage);

  MPI_Send(&partSize, 1, MPI_UNSIGNED_LONG, 0, messageSizeTag, MPI_COMM_WORLD);
  MPI_Send(decodedMessage, partSize, MPI_CHAR, 0, messagePartTag, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
  int size, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    ManageDecoding(size);
  } else {
    HandleDecoding(rank);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
