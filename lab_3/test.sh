#!/bin/bash

# Тестовые значения n (степени двойки)
TEST_VALUES=(2 4 8 16 32)

# Имя бинарного файла (замените на актуальное)
BINARY="./lab_3.out"

# Функция для вычисления log2(n)
log2() {
    local n=$1
    echo "l($n)/l(2)" | bc -l | awk '{print int($1)}'
}

# Функция проверки результата
run_test() {
    local n=$1
    local log2n=$(log2 $n)
    local num_procs=$((n / log2n))
    
    echo -n "Тест n=$n (процессов: $num_procs)... "
    
    # Запуск MPI-программы
    OUTPUT=$(mpirun --mca btl_vader_single_copy_mechanism none -np $num_procs --oversubscribe $BINARY $n 2>&1)
    EXIT_CODE=$?
    
    # Обработка ошибок выполнения
    if [ $EXIT_CODE -ne 0 ]; then
        echo -e "\e[31mОШИБКА\e[0m"
        echo "Детали: $OUTPUT"
        return 1
    fi
    
    # Проверка результата (ожидаемая сумма = n*(n+1)/2)
    local expected_sum=$((n * (n + 1) / 2))
    if [[ $OUTPUT == *"Total sum: $expected_sum"* ]]; then
        echo -e "\e[32mУСПЕХ\e[0m"
    else
        echo -e "\e[31mНЕСОВПАДЕНИЕ\e[0m"
        echo "Ожидалось: $expected_sum"
        echo "Получено: $OUTPUT"
    fi
}

# Запуск всех тестов
for n in "${TEST_VALUES[@]}"; do
    run_test $n
done

# Дополнительная проверка на некорректный ввод
echo -n "Тест с недопустимым n=3 (ожидается ошибка)... "
OUTPUT=$(mpirun -np 1 $BINARY 3 2>&1)
if [[ $OUTPUT == *"must be power of two"* ]]; then
    echo -e "\e[32mУСПЕХ\e[0m"
else
    echo -e "\e[31mОШИБКА\e[0m"
fi