# Лабораторная работа "Многоязыковое параллельное программирование"

## Техническое задание

Имеются две матрицы размерами AxB, BxC, необходимо их умножить.

Требуется провести оптимизацию кода с использованием:

- Компиляторной оптимизации;
- Векторизации;
- Выравнивания адресов;
- Многопоточности.

Необходимо протестировать и измерить временные характеристики программы на каждом из этапов реализации.

Необходимо собрать используемый код в подключаемую библиотеку и использовать во внешнем приложении.

## Простая реализация

В качестве оболочки для хранения данных матрицы был создан класс `Matrix` с полем типа `MatrixStruct` (пользовательская
структура, требуется для дальнейшей оптимизации). Сами числовые значения хранятся в двумерном массиве `int **`.

В качестве тестовых данных были сгенерированы 2 матрицы размером 2500x2500.

В качестве простой реализации используется математическое решение с поэлементным перемножением и сложением
соответствующих элементов двух матриц:

```c++
Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  auto matrixAData = matrixA->getData();
  auto matrixBData = matrixB->getData();
  auto resultData = result->getData();
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        resultData[i][k] += matrixAData[i][j] * matrixBData[j][k];
      }
    }
  }
  return result;
}
```

Компиляция решения (векторизация временно отключена для исследования прироста производительности):

```shell
g++ -fopenmp -fno-tree-vectorize main.cpp matrix.cpp matrix.h -o simple.out 
```

Запуск программы:

```shell
./simple.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s   |
|----|-----------|
| 1  | 66.252324 |
| 2  | 65.911645 |
| 3  | 65.462211 |
| 4  | 65.479019 |
| 5  | 65.450198 |
| 6  | 65.536043 |
| 7  | 65.481536 |
| 8  | 65.550595 |
| 9  | 65.543694 |
| 10 | 65.565879 |

Average time: 65.62

## Оптимизация компилятором

Для ускорения работы можно применить к приложению оптимизации компилятора.

Уровень оптимизации `-O2` заставляет компилятор использовать агрессивную оптимизацию кода, может существенно увеличить 
производительность.

```shell
g++ -fopenmp -O2 -fno-tree-vectorize main.cpp matrix.cpp matrix.h -o o2-opt.out 
```

```shell
./o2-opt.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 8.937707 |
| 2  | 9.132243 |
| 3  | 9.132336 |
| 4  | 8.916507 |
| 5  | 8.829375 |
| 6  | 9.083905 |
| 7  | 8.918446 |
| 8  | 8.800331 |
| 9  | 8.835325 |
| 10 | 8.801019 |

Average: 8.938719

В результате был получен прирост производительности примерно в 8 раз.

## Оптимизация под архитектуру

Далее можно указать компилятору, чтобы код был скомпилирован под текущую архитектуру (в данном случае была указана 
архитектура Skylake, соответствующая CPU на тестовой машине):

```shell
g++ -fopenmp -O2 -march=skylake -fno-tree-vectorize main.cpp matrix.cpp matrix.h -o arch-opt.out 
```

```shell
./arch-opt.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 8.865882 |
| 2  | 8.949322 |
| 3  | 8.832238 |
| 4  | 9.128938 |
| 5  | 8.758952 |
| 6  | 8.970608 |
| 7  | 9.111470 |
| 8  | 8.931047 |
| 9  | 8.845301 |
| 10 | 8.860294 |

Average: 8.925405

Прироста производительности данная оптимизация не дала.

## Векторизация

Для векторизации было сделано два действия.

Во-первых, была добавлена директива, указывающая компилятору оптимизировать цикл под использование SIMD-операций 

```c++
Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  auto matrixAData = matrixA->getData();
  auto matrixBData = matrixB->getData();
  auto resultData = result->getData();
  int temp;
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      temp = matrixAData[i][j];
#pragma omp simd
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        resultData[i][k] += temp * matrixBData[j][k];
      }
    }
  }
  return result;
}
```

Во-вторых, при сборке флаг `fno-tree-vectorize`, отключающий векторизацию, был заменен на `ftree-vectorize`, явно
указывающий на ее включение:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o vectorize.out
```

```shell
./vectorize.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 3.454872 |
| 2  | 4.112369 |
| 3  | 3.412770 |
| 4  | 4.118747 |
| 5  | 3.426250 |
| 6  | 4.108226 |
| 7  | 3.420965 |
| 8  | 4.114686 |
| 9  | 3.408629 |
| 10 | 4.124824 |

Average: 3.770234

Векторизация дала прирост производительности почти в 3 раза.

## Выравнивание адресов

К структуре `MatrixStructure` можно применить выравнивание адресов. Для этого для полей структуры при помощи
конструкции `alignas` можно указать количество бит для выравнивания хранящихся в ней значений.

Посредством анализа скомпилированной программы при помощи утилиты `pahole` (для анализа программа должна быть собрана с 
флагом `-g`), были подобраны коэффициенты для выравнивания полей:

```c++
struct MatrixStr {
  alignas(8) size_t rowCount;
  alignas(8) size_t columnCount;
  alignas(16) int** data;
};
```

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o alignment.out
```

```shell
./alignment.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 3.645423 |
| 2  | 4.246789 |
| 3  | 3.497473 |
| 4  | 4.185080 |
| 5  | 3.467960 |
| 6  | 4.152933 |
| 7  | 3.506341 |
| 8  | 4.317245 |
| 9  | 3.672501 |
| 10 | 4.209460 |

Average: 3.890120

Выравнивание не дало значительный прирост производительности.

## Многопоточность

Для реализации многопоточного кода воспользуемся библиотекой OpenMP. Для параллелизации можно указать внешний цикл:

```c++
Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  int temp;
#pragma omp parallel for shared(matrixA, matrixB, result) private(temp) default(none)
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      temp = matrixA->getData()[i][j];
#pragma omp simd
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        result->getData()[i][k] += temp * matrixB->getData()[j][k];
      }
    }
  }
  return result;
}
```

В разделяемых данных были указаны все матрицы (2 входные и одна выходная), т.к. эти данные требуется читать или 
записывать из всех потоков, оставшаяся переменная для хранения значения из входной матрицы A указана как локальная.

Т.к. гарантируется, что каждый поток может изменять в результирующем массиве только свой ряд матрицы, то дополнительных 
средств синхронизации не требуется.

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o multi-thread.out
```

```shell
./multi-thread.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 1.452196 |
| 2  | 1.082924 |
| 3  | 0.959780 |
| 4  | 1.033400 |
| 5  | 0.969570 |
| 6  | 1.104743 |
| 7  | 0.974645 |
| 8  | 1.390092 |
| 9  | 1.251709 |
| 10 | 1.231146 |

Average: 1.145021

Распараллеливание дало большой прирост производительности более чем в 3 раза.

## Shared Object

Данную программу можно использовать нативно в качестве динамической библиотеки.

Для создания .so-файла требуется скомпилировать программу с соответствующим флагом:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize -fPIC -shared matrix.cpp -o multiply-matrices.so
```

Для проверки возможностей использования нативной библиотеки в программах на других языках, было решено создать
GUI-приложение на языке программирования Python с использованием фреймворка PyQT6.

Приложение представляет собой набор из следующих элементов:

- строка для ввода пути к файлу с первой матрицей
- строка для ввода пути к файлу со второй матрицей
- строка для ввода пути к файлу результирующей матрицы
- кнопка, запускающая функцию из динамически подключаемой библиотеки, передающая в качестве параметров указанные выше
  значения

Для подключения .so-файла была использована библиотека `ctypes`, позволяющая вызывать функции на языке программирования
С. Однако из-за того, что исходный код библиотеки написан на языке С++, требуется обозначить ABI библиотеки следующим
образом:

```c++
extern "C" {
  void MultiplyMatrices(char *matrixAPath, char *matrixBPath, char *outputMatrixPath) {
    auto matrixA = Matrix::read(matrixAPath);
    auto matrixB = Matrix::read(matrixBPath);
    auto outputMatrix = Matrix::multiply(matrixA, matrixB);
    outputMatrix->write(outputMatrixPath);
  }
}
```

Также требуется обозначить API на стороне python-приложения:

```python
import ctypes
from ctypes import c_char_p

multiply_matrices_lib = ctypes.CDLL('multiply-matrices.so')
multiply_matrices = multiply_matrices_lib.MultiplyMatrices
multiply_matrices.argtypes = [c_char_p, c_char_p, c_char_p]
```

Остальной код на языке python настраивает отображение GUI.

Полученную программу удалось запустить и отладить.

## Выводы

В результате всех применённых оптимизаций удалось существенно увеличить производительность программы.

Наибольшее влияние оказали использование -O2, векторизация и параллелизация.

Также получилось настроить взаимодействие между реализованной на С++ библиотекой и GUI-приложения на Python с
использованием библиотеки PyQT6, однако в силу того, что python-программы гораздо менее производительные, чем программы
на C++, производительность заставляет желать лучшего.
