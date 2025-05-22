/*
https://www.wolframalpha.com/input?i=integrate+arctan%280.7*x%29%2F%28x%2B1.48%29+from+0.2+to+0.5
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define EPSILON 1e-10
#define MAX_ITERATIONS 100000000
#define INTERVAL_START 0.2
#define INTERVAL_END 0.5

double calculate_function(double x) {
    return atan(0.7 * x) / (x + 1.48);
}

double integrate_segment(double a, double b, int n) {
    double h = (b - a) / n;
    double sum = 0.5 * (calculate_function(a) + calculate_function(b));
    
    for (int i = 1; i < n; i++) {
        sum += calculate_function(a + i * h);
    }
    
    return h * sum;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    double start_time = MPI_Wtime();
    
    // Параллельное вычисление: каждый процесс обрабатывает свой сегмент
    double local_a = INTERVAL_START + rank * (INTERVAL_END - INTERVAL_START) / size;
    double local_b = INTERVAL_START + (rank + 1) * (INTERVAL_END - INTERVAL_START) / size;
    
    // Начальное число разбиений для каждого процесса
    int local_n = 1000;
    double local_result = integrate_segment(local_a, local_b, local_n);
    double prev_result;
    int iterations = 1;
    
    // Итеративное уточнение результата
    do {
        prev_result = local_result;
        local_n *= 2;
        local_result = integrate_segment(local_a, local_b, local_n);
        iterations++;
    } while (fabs(local_result - prev_result) > EPSILON / size && iterations < MAX_ITERATIONS);
    
    // Сбор результатов со всех процессов
    double global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();
    double local_time = end_time - start_time;
    double max_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("Integral result: %.15f\n", global_result);
        printf("Time: %.6f seconds\n", max_time);
    }
    
    MPI_Finalize();
    return 0;
}