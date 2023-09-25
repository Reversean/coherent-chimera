# Лабораторная работа 2

1. Создать и откомпилировать простое приложение в среде OpenMP (пример № 1). Доработать его следующим образом:
    - перед запуском параллельных нитей приложение должно определять максимально возможное количество нитей, которое
      одновременно может работать в системе;
    - если максимальное количество нитей меньше четырех, то параллельная секция должна выполняться с максимальным
      количеством нитей, иначе установить количество нитей для параллельной секции равным 4;
    - синхронизировать вывод потоков, что бы доступ к окну консоли был исключающим.
2. Разработать алгоритм решения задания.
3. Реализовать алгоритм в последовательной программе.
4. Распараллелить программу при помощи директив OpenMP, откомпилировать и отладить в среде OpenMP.
5. Составить схему потоков.

### Вариант 5

Дана последовательность натуральных чисел $C = \{ a_0 \dots a_{n–1} \}$. Создать OpenMP-приложение для поиска всех
$a_i$, являющихся простыми числами. Количество потоков является входным параметром программы, потоки проводят вычисления
независимо друг от друга, количество символов в строке может быть не кратно количеству потоков.

## OpenMP приложение

```c++
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
```

## Алгоритм решения задания

В качестве алгоритма был использован
алгоритм [Миллера-Рабина](https://ru.wikipedia.org/wiki/%D0%A2%D0%B5%D1%81%D1%82_%D0%9C%D0%B8%D0%BB%D0%BB%D0%B5%D1%80%D0%B0_%E2%80%94_%D0%A0%D0%B0%D0%B1%D0%B8%D0%BD%D0%B0).

Алгоритм основывается на комбинации малой теоремы Ферма и факте, что для простого числа $p$ не существует других корней 
уравнения $x^2 \equiv 1 \pmod p$, кроме 1 и -1. Тест Миллера-Рабина перебирает несколько значений $a$ и проверяет 
выполнение следующих условий.

Пусть $p$ — простое число и $p-1=2^sd$, тогда для любого $a$ справедливо хотя бы одно из условий:

- $a^{d}\equiv \pm1{\pmod {p}}$
- Существует целое число $r < s$ такое, что $a^{2^{r}d}\equiv -1{\pmod {p}}$

По теореме Ферма $a^{p-1}\equiv1\pmod p$, а т.к. $p-1=2^sd$ из свойства о корнях уравнения $x^2 \equiv 1 \pmod p$ 
следует, что если мы найдем такое $a$, для которого одно из условий не выполняется, значит $p$ — составное число. Если 
одно из условий выполняется, число $a$ называют свидетелем простоты числа $n$ по Миллеру, а само число $n$ — вероятно 
простым.

Чем больше свидетелей простоты найдено, тем выше вероятность того, что $n$ — простое. Согласно теореме Рабина 
вероятность того, что случайно выбранное число $a$ окажется свидетелем простоты составного числа составляет 
приблизительно $1/4$.

Следовательно, если проверить $k$ случайных чисел $a$, то вероятность принять составное число за простое 
$\approx(1/4)^k$.

Сложность работы алгоритма $O(k\log^3p)$, где k — количество проверок.

## Реализация алгоритма в последовательной программе

```c++
#include <cmath>

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
```

## Распараллелить программу при помощи директив OpenMP

Т.к. требуется найти простые числа из массива, можно распараллелить перебор исходного массива. Для этого можно 
использовать директиву `#pragma omp parallel for`.

```c++
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
```

Директивой `#pragma omp critical` здесь обозначена операция, являющейся критической секцией, т.к. здесь происходит
доступ к вектору со всеми найденными простыми числами. Т.к. изменять его могут иметь несколько потоков одновременно,
требуется обеспечить синхронность доступа к разделяемому ресурсу.
