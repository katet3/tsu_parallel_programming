/*
    Задача 2:
    Передать случайное число из диапазона [0,9] по кругу начиная с процесса с номером
    ноль. При этом каждый процесс должен добавлять к получаемому числу свой номер
    и отправлять дальше. Задачу решить с использованием функции двухточечного
    обмена. В конце работы программы процесс с номер ноль выдает на экран исходное
    случайное число и полученное им число. Все остальные процессы выдают на экран
    свой номер и переданное им значение.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int number, initial_number;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Инициализация генератора случайных чисел (только процесс 0)
    if (rank == 0) {
        srand(time(NULL));
        initial_number = rand() % 10;  // Сохраняем исходное число
        number = initial_number;       // Рабочая переменная для передачи
    }

    // Передача по кругу
    if (rank == 0) {
        // Процесс 0 отправляет число следующему процессу
        MPI_Send(&number, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
        
        // Затем получает число от последнего процесса
        MPI_Recv(&number, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
        
        // Вывод исходного и итогового числа
        printf("Process %d: initial number = %d, final number = %d\n", 
               rank, initial_number, number);
    } else {
        // Все остальные процессы получают число, добавляют свой rank и отправляют дальше
        MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process %d: received %d\n", rank, number);
        
        number += rank;
        MPI_Send(&number, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}