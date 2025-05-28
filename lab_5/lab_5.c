/*
    https://www.mkurnosov.net/teaching/docs/pct-lecture4.pdf
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

/** 
 * Вычисляет значение функции в точке (x, y)
 */
double CalculateFunctionValue(double x, double y) {
    return (x * y) * (x * y);
}

/** 
 * Проверяет принадлежность точки (x, y) области интегрирования
 */
bool IsPointInIntegrationDomain(double x, double y) {
    return (x * y > 1 && x * y < 2) && (fabs(x - y) < 1);
}

int main(int argc, char* argv[]) {
    int process_rank;
    int process_count;
    long long total_point_count = 10000000;
    long long local_point_count;
    long long points_in_domain = 0;
    double x_coordinate;
    double y_coordinate;
    double local_integral_sum = 0.0;
    double global_integral_sum = 0.0;
    double computation_start_time;
    double computation_end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    
    local_point_count = total_point_count / process_count;
    srand(time(NULL) + process_rank);

    const double x_min_bound = 0.5;
    const double x_max_bound = 2.0;
    const double y_min_bound = 0.5;
    const double y_max_bound = 2.0;
    const double generation_area = (x_max_bound - x_min_bound) * (y_max_bound - y_min_bound);

    computation_start_time = MPI_Wtime();
    
    for (long long iteration = 0; iteration < local_point_count; iteration++) {
        x_coordinate = x_min_bound + (x_max_bound - x_min_bound) * rand() / RAND_MAX;
        y_coordinate = y_min_bound + (y_max_bound - y_min_bound) * rand() / RAND_MAX;
        
        if (IsPointInIntegrationDomain(x_coordinate, y_coordinate)) {
            local_integral_sum += CalculateFunctionValue(x_coordinate, y_coordinate);
            points_in_domain++;
        }
    }

    long long total_points_in_domain;
    MPI_Reduce(&points_in_domain, &total_points_in_domain, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_integral_sum, &global_integral_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    computation_end_time = MPI_Wtime();

    if (process_rank == 0) {
        double integral_approximation = generation_area * global_integral_sum / total_point_count;
        
        printf("Approximate integral value = %.10f\n", integral_approximation);
        printf("Points inside domain = %lld / %lld (%.2f%%)\n", 
               total_points_in_domain, total_point_count, 
               100.0 * total_points_in_domain / total_point_count);
        printf("Computation time = %.4f seconds\n", computation_end_time - computation_start_time);
        
        if (total_points_in_domain > 0) {
            double standard_error = generation_area * 
                                  sqrt(global_integral_sum / total_point_count - 
                                  (integral_approximation / generation_area) * 
                                  (integral_approximation / generation_area)) / 
                                  sqrt(total_point_count);
            printf("Estimated standard error = %.2e\n", standard_error);
        }
    }

    MPI_Finalize();
    return 0;
}