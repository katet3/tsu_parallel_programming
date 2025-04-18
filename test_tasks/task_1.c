/*
    Задача 1:
    Нулевой процесс передает на все остальные процессы совой номер, используя
    функции двухточечного обмена. Каждый процесс принимает сообщение от процесса
    с номером ноль и выдает на экран свой номер и принятое сообщение.
*/

/*
    Message Passing Interface
    Нулевой процесс передает на все остальные процессы свой номер, используя
    функции двухточечного обмена. Каждый процесс принимает сообщение от процесса
    с номером ноль и выдает на экран свой номер и принятое сообщение

*/

/*
    MPI_Init(&argc, &argv) — инициализирует MPI-окружение.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank) — получает ранг (номер) текущего процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size) — получает общее количество процессов.
    MPI_Recv — функция приёма сообщения:
        &received_data — буфер для данных,
        1 — количество элементов,
        MPI_INT — тип данных (int),
        0 — ранг отправителя (ждём только от процесса 0),
        0 — тег сообщения (должен совпадать с тегом в MPI_Send),
        MPI_COMM_WORLD — коммуникатор,
        &status — структура с информацией о полученном сообщении.
*/



#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int received_data;
    MPI_Status status;

    // Инициализация MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Процесс 0 отправляет свой номер всем остальным процессам
        for (int i = 1; i < size; i++) {
            MPI_Send(&rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        // Остальные процессы принимают сообщение от процесса 0
        MPI_Recv(&received_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Process %d received message: %d\n", rank, received_data);
    }

    // Завершение работы с MPI
    MPI_Finalize();
    return 0;
}