#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARRAY_SIZE 10000000

void fill_random_array(double* array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] = (double)rand() / RAND_MAX;
    }
}

double compute_partial_sum(const double* array, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += array[i]*array[i];
    }
    return sum;
}

void test_blocking_version(int rank, int size, double* data, int data_size) {
    double start_time, end_time;
    int elements_per_proc = data_size / size;
    double* local_data = (double*)malloc(elements_per_proc * sizeof(double));
    double partial_sum, total_sum = 0.0;
    MPI_Status status;

    if (rank == 0) {
        start_time = MPI_Wtime();
        
        // Распределяем данные
        for (int i = 1; i < size; i++) {
            MPI_Send(data + i * elements_per_proc, elements_per_proc, 
                    MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        
        // Обрабатываем свою часть
        for (int i = 0; i < elements_per_proc; i++) {
            local_data[i] = data[i];
        }
        
        partial_sum = compute_partial_sum(local_data, elements_per_proc);
        total_sum = partial_sum;
        
        // Получаем результаты от других процессов
        for (int i = 1; i < size; i++) {
            MPI_Recv(&partial_sum, 1, MPI_DOUBLE, i, 0, 
                    MPI_COMM_WORLD, &status);
            total_sum += partial_sum;
        }
        
        end_time = MPI_Wtime();
        printf("Blocking (MPI_Send) total sum = %.2f, time = %.6f sec\n", 
               total_sum, end_time - start_time);
    } else {
        // Получаем данные
        MPI_Recv(local_data, elements_per_proc, MPI_DOUBLE, 0, 0,
                MPI_COMM_WORLD, &status);
        
        // Вычисляем частичную сумму
        partial_sum = compute_partial_sum(local_data, elements_per_proc);
        
        // Отправляем результат обратно
        MPI_Send(&partial_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    free(local_data);
}

void test_nonblocking_version(int rank, int size, double* data, int data_size) {
    double start_time, end_time;
    int elements_per_proc = data_size / size;
    double* local_data = (double*)malloc(elements_per_proc * sizeof(double));
    double partial_sum, total_sum = 0.0;
    MPI_Request* requests = (MPI_Request*)malloc((size-1) * sizeof(MPI_Request));
    MPI_Status status;

    if (rank == 0) {
        start_time = MPI_Wtime();
        
        // Распределяем данные
        for (int i = 1; i < size; i++) {
            MPI_Isend(data + i * elements_per_proc, elements_per_proc, 
                     MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &requests[i-1]);
        }
        
        // Обрабатываем свою часть
        for (int i = 0; i < elements_per_proc; i++) {
            local_data[i] = data[i];
        }
        
        partial_sum = compute_partial_sum(local_data, elements_per_proc);
        total_sum = partial_sum;
        
        // Ждем завершения всех отправок
        MPI_Waitall(size-1, requests, MPI_STATUSES_IGNORE);
        
        // Получаем результаты от других процессов
        for (int i = 1; i < size; i++) {
            MPI_Recv(&partial_sum, 1, MPI_DOUBLE, i, 0, 
                    MPI_COMM_WORLD, &status);
            total_sum += partial_sum;
        }
        
        end_time = MPI_Wtime();
        printf("Non-blocking (MPI_Isend) total sum = %.2f, time = %.6f sec\n", 
               total_sum, end_time - start_time);
    } else {
        // Получаем данные
        MPI_Recv(local_data, elements_per_proc, MPI_DOUBLE, 0, 0,
                MPI_COMM_WORLD, &status);
        
        // Вычисляем частичную сумму
        partial_sum = compute_partial_sum(local_data, elements_per_proc);
        
        MPI_Send(&partial_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    free(local_data);
    free(requests);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    double* data = NULL;
    
    if (rank == 0) {
        data = (double*)malloc(ARRAY_SIZE * sizeof(double));
        srand(time(NULL));
        fill_random_array(data, ARRAY_SIZE);
    }
    
    // Синхронизация перед тестами
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Тестируем блокирующую версию
    if (rank == 0) printf("\nTesting blocking version (MPI_Send):\n");
    test_blocking_version(rank, size, data, ARRAY_SIZE);
    
    // Синхронизация между тестами
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Тестируем неблокирующую версию
    if (rank == 0) printf("\nTesting non-blocking version (MPI_Isend):\n");
    test_nonblocking_version(rank, size, data, ARRAY_SIZE);
    
    if (rank == 0) free(data);
    MPI_Finalize();
    return 0;
}