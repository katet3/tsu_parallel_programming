/*
    МК17
    Как из 5 занятия
    Умножать кол-во процессов на 2 и менять ключи
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define NUM_SAMPLES 10000000
#define EPSILON 1e-5

double f(double x, double y) {
    return (x * y) * (x * y);
}

int is_in_region(double x, double y) {
    return (x * y > 1 && x * y < 2 && fabs(x - y) < 1);
}

int main() {
    double integral = 0.0;
    int count_in_region = 0;
    double area = 4.0; // Площадь [0,2]×[0,2]

    srand(time(NULL));

    // Тестируем разные стратегии распараллеливания
    struct {
        const char* name;
        omp_sched_t kind;
    } schedules[] = {
        {"static",  omp_sched_static},
        {"dynamic", omp_sched_dynamic},
        {"guided",  omp_sched_guided}
    };
    int num_schedules = sizeof(schedules)/sizeof(schedules[0]);

    for (int s = 0; s < num_schedules; s++) {
        integral = 0.0;
        count_in_region = 0;
        double start_time = omp_get_wtime();

        // Устанавливаем стратегию распараллеливания
        omp_set_schedule(schedules[s].kind, 0);

        #pragma omp parallel reduction(+:integral, count_in_region)
        {
            unsigned int seed = rand() + omp_get_thread_num();

            #pragma omp for schedule(runtime)
            for (int i = 0; i < NUM_SAMPLES; i++) {
                double x = ((double)rand_r(&seed) / RAND_MAX) * 2;
                double y = ((double)rand_r(&seed) / RAND_MAX) * 2;

                if (is_in_region(x, y)) {
                    integral += f(x, y);
                    count_in_region++;
                }
            }
        }

        double result = (count_in_region > 0) ? (integral / count_in_region) * area : 0.0;
        double end_time = omp_get_wtime();

        printf("Schedule: %s\n", schedules[s].name);
        printf("Integral value: %.10f\n", result);
        printf("Execution time: %.4f seconds\n", end_time - start_time);
        printf("----------------------------------------\n");
    }

    return 0;
}