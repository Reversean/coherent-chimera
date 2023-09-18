# Лабораторная работа 1

## Часть 1. Выбор подходящего уровня оптимизации

На основе примера, демонстрирующего различные уровни оптимизации, написать первый сценарий, выполняющий следующие
действия в цикле:

- Компиляцию вашего приложения, не интерактивно обрабатывающего данные, на компилируемом языке с ключами оптимизации:
    - `-O0`
    - `-Os`
    - `-O1`
    - `-O2`
    - `-O3`
    - `-O2 -march=native`
    - `-O3 -march=native`
    - `-O2 -march=native -funroll-loops`
    - `-O3 -march=native -funroll-loops`
- Вычисление времени выполнения программы (`time`). Приложение без оптимизации должно работать по меньшей мере 20 с.
- Вычисление занимаемого исполняемым файлом дискового пространства (в байтах) (`du`).

Сценарий должен принимать только имя исходного файла программы.

Вывод сценария должен содержать следующую информацию:

- Текущие опции оптимизации.
- Время затраченное программой на выполнение.
- Занимаемое программой дисковое пространство.

В качестве программы для компиляции был использован генератор бинарных деревьев для их
[сумматора](https://github.com/Reversean/binary-tree-sum).

Скрипт для сбора статистики компиляций приложения с разными уровнями оптимизации:

```shell
#!/bin/bash

SRC_DIR=binary-tree-sum/simple
EXECUTABLE_NAME=optimization-levels.out

options=(
    "-O0" "-Os" "-O1" "-O2" "-O3" "-O2 -march=native" "-O3 -march=native" \
    "-O2 -march=native -funroll-loops" "-O3 -march=native -funroll-loops"
)

for i in ${!options[*]}
do
    echo "Options: ${options[$i]}"
    g++ ${options[$i]} -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
    time ./$EXECUTABLE_NAME
    executableSize=$(du -b "./$EXECUTABLE_NAME")
    echo "Executable size: $executableSize bytes"
    echo "======================================"
    echo "======================================"
    rm $EXECUTABLE_NAME
done
```

Результаты работы скрипта:

| Опции                              | Время (real), с | Время (user), с | Время (sys), с | Размер, байт |
|------------------------------------|-----------------|-----------------|----------------|--------------|
| `-O0`                              | 27,674          | 23,047          | 4,579          | 44280        |
| `-Os`                              | 16,665          | 11,475          | 4,948          | 17824        |
| `-O1`                              | 15,975          | 10,688          | 5,036          | 17488        |
| `-O2`                              | 15,246          | 10,253          | 4,861          | 17456        |
| `-O3`                              | 11,147          | 6,280           | 4,817          | 17456        |
| `-O2 -march=native`                | 15,393          | 10,333          | 4,966          | 17456        |
| `-O3 -march=native`                | 11,206          | 6,434           | 4,710          | 17456        |
| `-O2 -march=native -funroll-loops` | 14,586          | 9,883           | 4,656          | 17456        |
| `-O3 -march=native -funroll-loops` | 10,940          | 6,157           | 4,760          | 17456        |

Самый оптимальный размер получился при использовании уровней оптимизации `-O2` и `-O3` (17456 байт).

Наиболее высокое быстродействие было достигнуто при использовании оптимизации `-O3 -march=native -funroll-loops` 
(суммарно 10,940 с).

## Часть 2. Выбор системного метода оптимизации

Скрипт для сбора статистики компиляций приложения с разными системными оптимизациями:

```shell
#!/bin/bash

SRC_DIR=binary-tree-sum/simple
EXECUTABLE_NAME=system-optimization.out
GUIDED_EXECUTABLE_NAME=guided-system-optimization.out

compileFlags="-O3 -march=native -funroll-loops"

echo "Options: $compileFlags -fipa-pta -flto=auto"
g++ $compileFlags -fipa-pta -flto=auto -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $EXECUTABLE_NAME

echo "Options: $compileFlags -fprofile-generate/-fprofile-use"
g++ $compileFlags -fprofile-generate -o $GUIDED_EXECUTABLE_NAME $SRC_DIR/*.cpp
./$GUIDED_EXECUTABLE_NAME
g++ $compileFlags -fprofile-use -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $GUIDED_EXECUTABLE_NAME
rm ./*.gcda
rm $EXECUTABLE_NAME

echo "Options: $compileFlags -fipa-pta -flto=auto -fprofile-generate/-fprofile-use"
g++ $compileFlags -fipa-pta -flto=auto -fprofile-generate -o $GUIDED_EXECUTABLE_NAME $SRC_DIR/*.cpp
./$GUIDED_EXECUTABLE_NAME
g++ $compileFlags -fipa-pta -flto=auto -fprofile-use -o $EXECUTABLE_NAME $SRC_DIR/*.cpp
time ./$EXECUTABLE_NAME
echo
executableSize=$(du -b "./$EXECUTABLE_NAME")
echo "Executable size: $executableSize bytes"
echo "======================================"
echo "======================================"
rm $GUIDED_EXECUTABLE_NAME
rm ./*.gcda
rm $EXECUTABLE_NAME
```

Результаты работы скрипта:

| Опции                                                                                    | Время (real), с | Время (user), с | Время (sys), с | Размер, байт |
|------------------------------------------------------------------------------------------|-----------------|-----------------|----------------|--------------|
| `-O3 -march=native -funroll-loops -fipa-pta -flto=auto`                                  | 11,378          | 6,180           | 5,108          | 16944        |
| `-O3 -march=native -funroll-loops -fprofile-generate/-fprofile-use`                      | 10,873          | 6,097           | 4,756          | 17456        |
| `-O3 -march=native -funroll-loops -fipa-pta -flto=auto -fprofile-generate/-fprofile-use` | 10,839          | 5,923           | 4,890          | 16944        |


При использовании межпроцедурной оптимизаций и оптимизацией времени компоновки удалось достичь наименьшего размера 
исполняемого файла (16944 байт).

При этом при помощи оптимизации ведомой профилем удалось немного увеличить быстродействие.

В итоге наилучший результат по размеру и быстродействию удалось используя все оптимизации (`-O3 -march=native -funroll-loops -fipa-pta -flto=auto -fprofile-generate/-fprofile-use`)
