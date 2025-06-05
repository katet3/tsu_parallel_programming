/*
    МК17
    Как из 5 занятия
    Умножать кол-во процессов на 2 и менять ключи
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <omp.h>

double CalculateFunctionValue(double x, double y) {
    return (x * y) * (x * y);
}

bool IsPointInIntegrationDomain(double x, double y) {
    return (x * y > 1 && x * y < 2) && (fabs(x - y) < 1);
}

int main(int argc, char* argv[]) {
    long long total_point_count = 10000000;
    long long points_in_domain = 0;
    double integral_sum = 0.0;
    double x_coordinate, y_coordinate;
    
    const double x_min_bound = 0.5;
    const double x_max_bound = 2.0;
    const double y_min_bound = 0.5;
    const double y_max_bound = 2.0;
    const double generation_area = (x_max_bound - x_min_bound) * (y_max_bound - y_min_bound);
    
    if (argc < 2) {
        printf("Usage: %s <schedule_type> [chunk_size]\n", argv[0]);
        printf("Schedule types: 0=static, 1=dynamic, 2=guided\n");
        return 1;
    }
    
    int schedule_type = atoi(argv[1]);
    int chunk_size = (argc > 2) ? atoi(argv[2]) : 1000;
    
    // Последовательное выполнение для сравнения
    double seq_start = omp_get_wtime();
    long long seq_points_in_domain = 0;
    double seq_integral_sum = 0.0;
    
    srand(time(NULL));
    for (long long i = 0; i < total_point_count; i++) {
        double x = x_min_bound + (x_max_bound - x_min_bound) * rand() / RAND_MAX;
        double y = y_min_bound + (y_max_bound - y_min_bound) * rand() / RAND_MAX;
        if (IsPointInIntegrationDomain(x, y)) {
            seq_integral_sum += CalculateFunctionValue(x, y);
            seq_points_in_domain++;
        }
    }
    double sequential_time = omp_get_wtime() - seq_start;
    
    // Параллельное выполнение
    double parallel_start = omp_get_wtime();
    
    // Устанавливаем стратегию распределения итераций
    omp_sched_t schedule;
    switch (schedule_type) {
        case 0: schedule = omp_sched_static; break;
        case 1: schedule = omp_sched_dynamic; break;
        case 2: schedule = omp_sched_guided; break;
        default: schedule = omp_sched_static;
    }
    omp_set_schedule(schedule, chunk_size);
    
    #pragma omp parallel private(x_coordinate, y_coordinate) reduction(+:integral_sum, points_in_domain)
    {
        unsigned int seed = time(NULL) + omp_get_thread_num();
        
        #pragma omp for schedule(runtime)
        for (long long iteration = 0; iteration < total_point_count; iteration++) {
            x_coordinate = x_min_bound + (x_max_bound - x_min_bound) * rand_r(&seed) / (double)RAND_MAX;
            y_coordinate = y_min_bound + (y_max_bound - y_min_bound) * rand_r(&seed) / (double)RAND_MAX;
            
            if (IsPointInIntegrationDomain(x_coordinate, y_coordinate)) {
                integral_sum += CalculateFunctionValue(x_coordinate, y_coordinate);
                points_in_domain++;
            }
        }
    }
    
    double parallel_time = omp_get_wtime() - parallel_start;
    
    // Вывод результатов
    printf("\n=== Integration Results ===\n");
    printf("Approximate integral value = %.10f\n", generation_area * integral_sum / total_point_count);
    printf("Points inside domain = %lld / %lld (%.2f%%)\n", 
           points_in_domain, total_point_count, 
           100.0 * points_in_domain / total_point_count);
    
    printf("\n=== Performance Metrics ===\n");
    printf("Schedule type: ");
    switch (schedule_type) {
        case 0: printf("static"); break;
        case 1: printf("dynamic"); break;
        case 2: printf("guided"); break;
    }
    if (schedule_type != 0) printf(" (chunk=%d)", chunk_size);
    printf("\n");
    
    printf("Threads count = %d\n", omp_get_max_threads());
    printf("Sequential time = %.4f seconds\n", sequential_time);
    printf("Parallel time = %.4f seconds\n", parallel_time);
    printf("Speedup = %.2f\n", sequential_time / parallel_time);
    printf("Efficiency = %.2f%%\n", 100.0 * sequential_time / (parallel_time * omp_get_max_threads()));
    
    if (points_in_domain > 0) {
        double standard_error = generation_area * 
                              sqrt(integral_sum / total_point_count - 
                              (generation_area * integral_sum / total_point_count / generation_area) * 
                              (generation_area * integral_sum / total_point_count / generation_area)) / 
                              sqrt(total_point_count);
        printf("\nEstimated standard error = %.2e\n", standard_error);
    }
    
    return 0;
}