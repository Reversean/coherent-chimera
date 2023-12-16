#include <vector>
#include <string>

using namespace std;

class Matrices {
 public:
  vector<vector<int>> matrixA;
  vector<vector<int>> matrixB;
  int getN1();
  int getN2();
  int getN3();
  static Matrices fromFiles(string *sizesFilePath, string *matrixAFilePath, string *matrixBFilePath);
 private:
  Matrices(string *sizesPath, string *matrixAPath, string *matrixBPath);
};
