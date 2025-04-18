/*
    Задача 3:
    Написать параллельную программу умножения матрицы на вектор y=Ax. При
    условии, что исходная матрица «A» и вектор «x» хранятся и заполняются случайным
    числами на процессе с номером ноль. Можно использовать переменные A_global и
    x_global для их хранения. Необходимо используя операции коллективного
    взаимодействия MPI_Bcast() и MPI_Scatter() разослать вектор x_global на все
    процессы и распределить по процессам матрицу A_global.
    Далее каждый процесс умножит распределенные ему строки матрицы «А» на вектор
    «x» и получит свою часть вектора «y». В результате работы программы каждый
    процесс должен располагать в своем распоряжении целиком вектором «y», а не его
    частью. Для этого использовать операцию MPI_Allgather().
    Для проверки правильности работы параллельной программы на процессе с номером
    ноль умножить последовательно A_global на x_global и сравнить результат с
    полученным в результате работы параллельного алгоритма «у»
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void fill_random_matrix(double* matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = (double)rand() / RAND_MAX;
    }
}

void fill_random_vector(double* vec, int size) {
    for (int i = 0; i < size; ++i) {
        vec[i] = (double)rand() / RAND_MAX;
    }
}

void sequential_matrix_vector_mult(const double* matrix, const double* vec,
                                 double* result, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        result[i] = 0.0;
        for (int j = 0; j < cols; ++j) {
            result[i] += matrix[i * cols + j] * vec[j];
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Размеры матрицы и вектора, строки, столбцы
    const int N = 8;
    const int M = 8;
    
    double* A_global = NULL;
    double* x_global = (double*)malloc(M * sizeof(double));
    
    // Процесс 0 заполняет матрицу и вектор случайными числами
    if (rank == 0) {
        A_global = (double*)malloc(N * M * sizeof(double));
        srand(time(NULL));
        fill_random_matrix(A_global, N, M);
        fill_random_vector(x_global, M);
        
        printf("Matrix A:\n");
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                printf("%.4f ", A_global[i * M + j]);
            }
            printf("\n");
        }
        
        printf("\nVector x:\n");
        for (int j = 0; j < M; ++j) {
            printf("%.4f ", x_global[j]);
        }
        printf("\n");
    }
    
    // Рассылаем вектор x всем процессам
    MPI_Bcast(x_global, M, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Распределяем строки матрицы между процессами
    int rows_per_process = N / size;
    double* A_local = (double*)malloc(rows_per_process * M * sizeof(double));
    
    MPI_Scatter(A_global, rows_per_process * M, MPI_DOUBLE,
                A_local, rows_per_process * M, MPI_DOUBLE,
                0, MPI_COMM_WORLD);
    
    // Каждый процесс умножает свои строки на вектор
    double* y_local = (double*)malloc(rows_per_process * sizeof(double));
    for (int i = 0; i < rows_per_process; ++i) {
        y_local[i] = 0.0;
        for (int j = 0; j < M; ++j) {
            y_local[i] += A_local[i * M + j] * x_global[j];
        }
    }
    
    // Собираем все части вектора y на всех процессах
    double* y_global = (double*)malloc(N * sizeof(double));
    MPI_Allgather(y_local, rows_per_process, MPI_DOUBLE,
                  y_global, rows_per_process, MPI_DOUBLE,
                  MPI_COMM_WORLD);
    
    // Проверка на процессе 0
    if (rank == 0) {
        double* y_seq = (double*)malloc(N * sizeof(double));
        sequential_matrix_vector_mult(A_global, x_global, y_seq, N, M);
        
        printf("\nSequential result:\n");
        for (int i = 0; i < N; ++i) {
            printf("%.4f ", y_seq[i]);
        }
        printf("\n");
        
        printf("\nParallel result:\n");
        for (int i = 0; i < N; ++i) {
            printf("%.4f ", y_global[i]);
        }
        printf("\n");
        
        // Проверка совпадения результатов
        int correct = 1;
        for (int i = 0; i < N; ++i) {
            if (fabs(y_seq[i] - y_global[i]) > 1e-10) {
                correct = 0;
                break;
            }
        }
        
        if (correct) {
            printf("\nResults match!\n");
        } else {
            printf("\nResults don't match!\n");
        }
        
        free(y_seq);
    }
    
    if (rank == 0) {
        free(A_global);
    }
    free(A_local);
    free(y_local);
    free(y_global);
    free(x_global);
    
    MPI_Finalize();
    return 0;
}