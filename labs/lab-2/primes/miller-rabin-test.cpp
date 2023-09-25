#include <cmath>
#include "miller-rabin-test.h"

using namespace std;

unsigned long Power(unsigned long int x, unsigned long y, unsigned long p) {
  unsigned long res = 1;
  x = x % p;
  while (y > 0) {
    if (y & 1)
      res = (res * x) % p;
    y = y >> 1;
    x = (x * x) % p;
  }
  return res;
}

bool MillerTest(unsigned long d, unsigned long n) {
  unsigned long a = 2 + rand() % (n - 4);

  unsigned long x = Power(a, d, n);

  if (x == 1 || x == n - 1)
    return true;

  while (d != n - 1) {
    x = (x * x) % n;
    d *= 2;
    if (x == 1)
      return false;
    if (x == n - 1)
      return true;
  }
  return false;
}
