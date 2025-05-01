#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int current_rank, total_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &current_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

    if (argc != 2) {
        if (current_rank == 0) {
            fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const int n = atoi(argv[1]);
    if (n <= 0) {
        if (current_rank == 0) {
            fprintf(stderr, "n must be positive integer\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const int log2n = (int)log2(n);
    if ((1 << log2n) != n) {
        if (current_rank == 0) {
            fprintf(stderr, "n must be power of two\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const int group_size = log2n;
    const int num_groups = n / group_size;
    
    if (total_processes != num_groups) {
        if (current_rank == 0) {
            fprintf(stderr, "Required %d processes\n", num_groups);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Инициализация данных
    int* local_data = (int*)malloc(group_size * sizeof(int));
    for (int i = 0; i < group_size; ++i) {
        local_data[i] = current_rank * group_size + i + 1;
    }

    // Этап 1: Локальное суммирование
    int local_sum = 0;
    for (int i = 0; i < group_size; ++i) {
        local_sum += local_data[i];
    }

    // Этап 2: Каскадное суммирование
    int result = local_sum;
    for (int step = 1; step < num_groups; step *= 2) {
        if (current_rank % (2 * step) == 0) {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, current_rank + step, 0, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += temp;
        } else if (current_rank % step == 0) {
            MPI_Send(&result, 1, MPI_INT, current_rank - step, 0, 
                    MPI_COMM_WORLD);
            break;
        }
    }

    // Вывод результата
    if (current_rank == 0) {
        printf("Total sum: %d\n", result);
    }

    free(local_data);
    MPI_Finalize();
    return EXIT_SUCCESS;
}