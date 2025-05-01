#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int current_rank, total_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

    // Проверка входных параметров
    if (argc != 2) {
        if (current_rank == 0) fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const int n = atoi(argv[1]);
    if (n <= 0) {
        if (current_rank == 0) fprintf(stderr, "n must be positive\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Вычисление размера группы и количества групп
    const int log2n = (int)ceil(log2(n));
    const int num_groups = (n + log2n - 1) / log2n;

    // Проверка соответствия процессов
    if (total_processes != num_groups) {
        if (current_rank == 0) {
            fprintf(stderr, "Required %d processes\n", num_groups);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Распределение данных (без дополнения нулями)
    int elements_per_proc = log2n;
    if (current_rank == num_groups - 1) {
        elements_per_proc = n - (num_groups - 1) * log2n;
    }

    int* local_data = (int*)malloc(elements_per_proc * sizeof(int));
    for (int i = 0; i < elements_per_proc; ++i) {
        local_data[i] = current_rank * log2n + i + 1;
    }

    // Локальное суммирование (только реальные элементы)
    int local_sum = 0;
    for (int i = 0; i < elements_per_proc; ++i) {
        local_sum += local_data[i];
    }

    // Каскадное суммирование с проверкой рангов
    int parallel_sum = local_sum;
    for (int step = 1; step < num_groups; step *= 2) {
        if (current_rank % (2 * step) == 0) {
            int partner_rank = current_rank + step;
            if (partner_rank < total_processes) {
                int temp;
                MPI_Recv(&temp, 1, MPI_INT, partner_rank, 0, 
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                parallel_sum += temp;
            }
        } else if (current_rank % step == 0) {
            int partner_rank = current_rank - step;
            if (partner_rank >= 0) {
                MPI_Send(&parallel_sum, 1, MPI_INT, partner_rank, 0, 
                        MPI_COMM_WORLD);
                break;
            }
        }
    }

    // Вывод результата и проверка
    if (current_rank == 0) {
        const int expected_sum = n * (n + 1) / 2;
        printf("Parallel sum: %d\n", parallel_sum);
        printf("Expected sum: %d\n", expected_sum);
        printf("Result: %s\n", 
              (parallel_sum == expected_sum) ? "CORRECT" : "INCORRECT");
    }

    free(local_data);
    MPI_Finalize();
    return EXIT_SUCCESS;
}