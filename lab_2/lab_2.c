#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/*
MPI_Gather - собирает данные со всех процессов в целевом процессе
Параметры:
    sendbuf   - адрес отправного буфера (у всех процессов)
    sendcount - число элементов в отправном буфере
    sendtype  - тип данных отправляемых элементов
    recvbuf   - адрес приемного буфера (только у root-процесса)
    recvcount - число элементов от каждого процесса
    recvtype  - тип данных принимаемых элементов
    root      - ранг процесса-получателя
    comm      - коммуникатор

MPI_Abort - аварийно завершает все процессы в указанном коммуникаторе
Параметры:
    comm      - коммуникатор для завершения
    errorcode - код ошибки для возврата в вызывающую среду
Используется для экстренного завершения при критических ошибках
*/

#define SIZE_ARRAY 5

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int current_rank;
    int total_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

    if (total_processes < 3) {
        if (current_rank == 0) {
            fprintf(stderr, "Program requires minimum 3 processes.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int local_data[SIZE_ARRAY];
    for (int i = 0; i < SIZE_ARRAY; ++i) {
        local_data[i] = current_rank * 10 + i;
    }

    int* ptr_gathered_data = NULL;
    if (current_rank == 2) {
        ptr_gathered_data = (int*)malloc(total_processes * SIZE_ARRAY * sizeof(int));
        if (!ptr_gathered_data) {
            fprintf(stderr, "Memory allocation failed on root process.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
    }

    MPI_Gather(local_data, SIZE_ARRAY, MPI_INT,
              ptr_gathered_data, SIZE_ARRAY, MPI_INT,
              2, MPI_COMM_WORLD);

    if (current_rank == 2) {
        for (int proc = 0; proc < total_processes; ++proc) {
            printf("Process %d data:", proc);
            const int* process_data = ptr_gathered_data + proc * SIZE_ARRAY;
            for (int i = 0; i < SIZE_ARRAY; ++i) {
                printf(" %d", process_data[i]);
            }
            printf("\n");
        }
        free(ptr_gathered_data);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}