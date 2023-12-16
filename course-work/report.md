# Лабораторная работа "Многоязыковое параллельное программирование"

Выполнил студент группы 5140901/21501 Лихолетов М.Д.

---

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

В качестве тестовых данных были сгенерированы 2 матрицы размером 2500x2500. Входные и выходные данные хранятся в виде
файлов.

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

Компиляция решения (векторизация явно отключена для исследования прироста производительности):

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

Среднее время выполнения: 65.62

## Оптимизация компилятором

Для ускорения работы можно применить к приложению оптимизации компилятора.

Уровень оптимизации `-O2` заставляет компилятор использовать агрессивную оптимизацию кода, может существенно увеличить
производительность.

```shell
g++ -fopenmp -O2 -fno-tree-vectorize main.cpp matrix.cpp matrix.h -o o2-opt.out 
```

Запуск программы:

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

Среднее время выполнения: 8.938719

В результате был получен прирост производительности примерно в 8 раз.

## Оптимизация под архитектуру

Далее можно указать компилятору, чтобы код был скомпилирован под текущую архитектуру (в данном случае была указана
архитектура Skylake, соответствующая CPU на тестовой машине):

```shell
g++ -fopenmp -O2 -march=skylake -fno-tree-vectorize main.cpp matrix.cpp matrix.h -o arch-opt.out 
```

Запуск программы:

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

Среднее время выполнения: 8.925405

Прироста производительности данная оптимизация не дала.

## Векторизация

Для векторизации было сделано два действия.

Во-первых, была добавлена директива, указывающая компилятору оптимизировать внутренний цикл под использование 
SIMD-операций:

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
указывающий компилятору использовать векторизацию:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o vectorize.out
```

Запуск программы:

```shell
./vectorize.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 4.393521 |
| 2  | 4.689900 |
| 3  | 4.147908 |
| 4  | 4.766818 |
| 5  | 4.197970 |
| 6  | 4.738735 |
| 7  | 4.191885 |
| 8  | 4.728298 |
| 9  | 4.206985 |
| 10 | 4.912950 |

Среднее время выполнения: 4.497497

Векторизация дала прирост производительности в 2 раза.

## Выравнивание адресов

К структуре `MatrixStructure` можно применить выравнивание адресов. Для этого для полей структуры при помощи
конструкции `alignas` можно указать количество бит для выравнивания хранящихся в ней значений.

С учетом архитектуры используемого CPU и анализа скомпилированной программы при помощи утилиты `pahole` (для анализа 
программа должна быть собрана с флагом `-g`), были подобраны коэффициенты для выравнивания полей:

Однако учитывая для того, чтобы выравнивание работало как надо на 64-битной архитектуре 

```c++
struct MatrixStr {
  alignas(32) size_t rowCount;
  alignas(32) size_t columnCount;
  alignas(32) int** data;
};
```

Также была модифицирована директива `#pragma omp simd`: был указан модификатор `aligned`, указывающий компилятору, что
требуется сделать выравнивание массива с входными данными:

```c++
Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  auto matrixAData = matrixA->getData();
  auto matrixBData = matrixB->getData();
  auto resultData = result->getData();
  int temp1, *temp2;
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      temp1 = matrixAData[i][j];
      temp2 = matrixBData[j];
#pragma omp simd aligned(temp2:32)
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        resultData[i][k] += temp1 * temp2[k];
      }
    }
  }
  return result;
}
```

Компиляция решения:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o alignment.out
```

Запуск программы:

```shell
./alignment.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 3.767462 |
| 2  | 3.744853 |
| 3  | 3.714222 |
| 4  | 3.735639 |
| 5  | 3.646847 |
| 6  | 3.671307 |
| 7  | 3.722677 |
| 8  | 3.701179 |
| 9  | 3.708727 |
| 10 | 3.701846 |

Среднее время выполнения: 3.7114759

Выравнивание адресов дало некоторый прирост производительности (меньше секунды).

## Многопоточность

Для реализации многопоточного кода воспользуемся библиотекой OpenMP. Для параллелизации можно указать специальную 
директиву, указывающую выполнять итерации внешнего цикла параллельно:

```c++
Matrix *Matrix::multiply(Matrix *matrixA, Matrix *matrixB) {
  auto result = new Matrix(matrixA->getRowCount(), matrixB->getColumnCount());
  int temp1, *temp2;
#pragma omp parallel for shared(matrixA, matrixB, result) private(temp1, temp2) default(none)
  for (int i = 0; i < matrixA->getRowCount(); ++i) {
    for (int j = 0; j < matrixB->getRowCount(); ++j) {
      temp1 = matrixA->getData()[i][j];
      temp2 = matrixB->getData()[j];
#pragma omp simd aligned(temp2:32)
      for (int k = 0; k < matrixB->getColumnCount(); ++k) {
        result->getData()[i][k] += temp1 * temp2[k];
      }
    }
  }
  return result;
}
```

В разделяемых данных были указаны все матрицы (2 входные и одна выходная), т.к. эти данные требуется читать или
записывать из всех потоков, остальные временные переменные указаны как приватные.

Т.к. гарантируется, что каждый поток может изменять в результирующем массиве только свой ряд матрицы, то дополнительных
средств синхронизации не требуется.

Компиляция решения:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize main.cpp matrix.cpp matrix.h -o multi-thread.out
```

Запуск программы:

```shell
./multi-thread.out matrix-a.txt matrix-b.txt matrix-out.txt
```

| #  | Time, s  |
|----|----------|
| 1  | 1.019202 |
| 2  | 1.095794 |
| 3  | 1.157425 |
| 4  | 1.071780 |
| 5  | 1.040998 |
| 6  | 1.147622 |
| 7  | 1.272614 |
| 8  | 1.105807 |
| 9  | 1.010876 |
| 10 | 1.116143 |

Среднее время выполнения: 1.103826

Распараллеливание дало большой прирост производительности более чем в 3 раза.

## Shared Object

Данную программу можно использовать нативно в качестве динамической библиотеки.

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

Для создания .so-файла требуется скомпилировать программу с соответствующим флагом:

```shell
g++ -fopenmp -O2 -march=skylake -ftree-vectorize -fPIC -shared matrix.cpp -o multiply-matrices.so
```

Также требуется обозначить ABI на стороне python-приложения:

```python
import ctypes
from ctypes import c_char_p

multiply_matrices_lib = ctypes.CDLL('multiply-matrices.so')
multiply_matrices = multiply_matrices_lib.MultiplyMatrices
multiply_matrices.argtypes = [c_char_p, c_char_p, c_char_p]
```

Остальной код на языке python настраивает отображение GUI:

```python
from PyQt6.QtWidgets import (QApplication, QDialog, QGridLayout, QLineEdit, QPushButton)

class WidgetGallery(QDialog):

    def __init__(self, parent=None):
        super(WidgetGallery, self).__init__(parent)

        self.multiply_matrices_lib = ctypes.CDLL('../multiply-matrices.so')
        self.multiply_matrices = self.multiply_matrices_lib.MultiplyMatrices
        self.multiply_matrices.argtypes = [c_char_p, c_char_p, c_char_p]

        self.matrix_a_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/matrix-a.txt')
        self.matrix_a_path_line.setObjectName('Matrix A Path')
        self.matrix_a_path_line.setPlaceholderText('Matrix A')

        self.matrix_b_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/matrix-b.txt')
        self.matrix_b_path_line.setObjectName('Matrix B Path')
        self.matrix_b_path_line.setPlaceholderText('Matrix B')

        self.output_matrix_path_line = QLineEdit('/home/Reversean/code/coherent-chimera/course-work/test.txt')
        self.output_matrix_path_line.setObjectName('Output Matrix Path')
        self.output_matrix_path_line.setPlaceholderText('Output Matrix')

        multiply_button = QPushButton('Multiply Matrices')
        multiply_button.clicked.connect(self.multiply_button_click)

        layout = QGridLayout()
        layout.addWidget(self.matrix_a_path_line, 0, 0, 1, 2)
        layout.addWidget(self.matrix_b_path_line, 1, 0, 1, 2)
        layout.addWidget(self.output_matrix_path_line, 2, 0, 1, 2)
        layout.addWidget(multiply_button, 3, 0, 1, 2)
        layout.setRowStretch(4, 1)
        self.setLayout(layout)
        self.setWindowTitle("Matrices Multiplication")

    def multiply_button_click(self):
        matrix_a_path = self.matrix_a_path_line.text()
        matrix_b_path = self.matrix_b_path_line.text()
        output_matrix_path = self.output_matrix_path_line.text()
        print(matrix_a_path)
        print(matrix_b_path)
        print(output_matrix_path)
        self.multiply_matrices(
            matrix_a_path.encode('utf-8'),
            matrix_b_path.encode('utf-8'),
            output_matrix_path.encode('utf-8')
        )


if __name__ == '__main__':
    import sys

    app = QApplication(sys.argv)
    gallery = WidgetGallery()
    gallery.show()
    sys.exit(app.exec())
```

Полученную программу удалось запустить и отладить.

## Выводы

В результате всех применённых оптимизаций удалось существенно увеличить производительность программы.

Ощутимое влияние оказали использование второго уровня оптимизации компилятором, векторизация, выравнивание адресов и 
параллелизация.

Также получилось настроить взаимодействие между реализованной на С++ библиотекой и GUI-приложения на Python с
использованием библиотек ctypes PyQT6, однако в силу того, что python-программы гораздо менее производительные, чем 
программы на C++, производительность заставляет желать лучшего.
