#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  int rows = stoi(argv[1]);
  int columns = stoi(argv[2]);
  char *outputPath = argv[3];

  random_device randomDev;
  mt19937 rng(randomDev());
  uniform_int_distribution<int> dist(0,100);

  ofstream outputFile(outputPath);
  if (!outputFile.is_open()) {
    printf("Can't save file %s", outputPath);
    return 1;
  }

  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      outputFile << dist(rng);
      if (j != columns - 1) {
        outputFile << ' ';
      }
    }
    if (i != rows - 1) {
      outputFile << '\n';
    }
  }
  return 0;
}
