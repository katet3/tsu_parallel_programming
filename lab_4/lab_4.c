/*
https://www.wolframalpha.com/input?i=integrate+arctan%280.7*x%29%2F%28x%2B1.48%29+from+0.2+to+0.5
http://www.machinelearning.ru/wiki/index.php?title=%D0%9C%D0%B5%D1%82%D0%BE%D0%B4%D1%8B_%D0%BF%D1%80%D1%8F%D0%BC%D0%BE%D1%83%D0%B3%D0%BE%D0%BB%D1%8C%D0%BD%D0%B8%D0%BA%D0%BE%D0%B2_%D0%B8_%D1%82%D1%80%D0%B0%D0%BF%D0%B5%D1%86%D0%B8%D0%B9
*/

/*
Ускорение S(p) = T(1) / T(p)
    T(1) - время выполнения последовательной версии
    T(p) - время выполнения параллельной версии на p процессах
*/

/*
Эффективность E(p) = S(p) / p => T(1) / (p * T(p))

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define INTERVAL_START 0.2
#define INTERVAL_END 0.5
#define FIXED_N 1000000  // Фиксированное количество разбиений

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
    
    // Распределение работы между процессами
    double segment_size = (INTERVAL_END - INTERVAL_START) / size;
    double local_a = INTERVAL_START + rank * segment_size;
    double local_b = INTERVAL_START + (rank + 1) * segment_size;
    
    // Вычисление интеграла с фиксированным n
    int local_n = FIXED_N / size;  // Распределяем общее n по процессам
    double local_result = integrate_segment(local_a, local_b, local_n);
    
    // Сбор результатов со всех процессов
    double global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double end_time = MPI_Wtime();
    double max_time;
    double local_time = end_time - start_time;
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Выполняем последовательную версию для сравнения
        double serial_start = MPI_Wtime();
        double serial_result = integrate_segment(INTERVAL_START, INTERVAL_END, FIXED_N);
        double serial_time = MPI_Wtime() - serial_start;
        
        printf("Integral result: %.15f\n", global_result);
        printf("Fixed n per process: %d\n", local_n);
        printf("Total n: %d\n", FIXED_N);
        printf("Parallel time: %.6f seconds\n", max_time);
        printf("Number of processes: %d\n", size);
        
        printf("\nSerial result: %.15f\n", serial_result);
        printf("Serial time: %.6f seconds\n", serial_time);
        
        // Рассчитываем ускорение и эффективность
        double speedup = serial_time / max_time;
        double efficiency = speedup / size * 100.0;
        
        printf("\nSpeedup: %.2f\n", speedup);
        printf("Efficiency: %.2f%%\n", efficiency);
    }
    
    MPI_Finalize();
    return 0;
}