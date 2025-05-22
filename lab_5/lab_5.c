#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define NUM_SAMPLES 1000000  // Общее количество случайных точек

// Функция f(x, y)
double f(double x, double y) {
    return (x * y) * (x * y);
}

// Проверка, принадлежит ли точка области D
int is_in_region(double x, double y) {
    return (x * y > 1 && x * y < 2 && fabs(x - y) < 1);
}

int main(int argc, char **argv) {
    int rank, size;
    double total_integral = 0.0;
    int count_in_region = 0;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Инициализация генератора случайных чисел
    srand(time(NULL) + rank);

    int samples_per_process = NUM_SAMPLES / size;
    double local_integral = 0.0;

    start_time = MPI_Wtime();

    // Генерация случайных точек и вычисление интеграла
    for (int i = 0; i < samples_per_process; i++) {
        double x = ((double)rand() / RAND_MAX) * 2; // Генерация x в [0, 2]
        double y = ((double)rand() / RAND_MAX) * 2; // Генерация y в [0, 2]

        if (is_in_region(x, y)) {
            local_integral += f(x, y);
            count_in_region++;
        }
    }

    // Сбор результатов от всех процессов
    int total_count_in_region;
    MPI_Reduce(&count_in_region, &total_count_in_region, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_integral, &total_integral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    // Вычисление окончательного значения интеграла на корневом процессе
    if (rank == 0) {
        double estimated_integral = (total_integral / total_count_in_region) * (4.0 * total_count_in_region / NUM_SAMPLES);
        double elapsed_time = end_time - start_time;

        printf("Приближенное значение двойного интеграла: %f\n", estimated_integral);
        printf("Время выполнения: %f секунд\n", elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
