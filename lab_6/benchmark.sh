#!/bin/bash

# make build TARGET=lab_6
# make run TARGET=lab_6 SCHEDULE=1 CHUNK=500 NP=2

TARGET="lab_6"
OUTPUT_FILE="benchmark_results.csv"

echo "Compiling the program..."
make build TARGET=$TARGET || exit 1

echo "Schedule Type,Chunk Size,Threads,Time,Speedup,Efficiency" > $OUTPUT_FILE

run_test() {
    local schedule=$1
    local chunk=$2
    local threads=$3
    
    echo -n "Running $schedule schedule with $threads threads..."
    
    # Запускаем и сохраняем полный вывод во временный файл
    make -s run TARGET=$TARGET NP=$threads SCHEDULE=$schedule CHUNK=$chunk > tmp_output.txt 2>&1
    
    # Извлекаем метрики с учетом нового формата вывода
    local time=$(grep "Parallel time" tmp_output.txt | awk '{print $4}')
    local speedup=$(grep "Speedup" tmp_output.txt | awk '{print $3}')
    local efficiency=$(grep "Efficiency" tmp_output.txt | awk '{print $3}' | sed 's/%//')
    
    # Если значения не найдены, ставим N/A
    [ -z "$time" ] && time="N/A"
    [ -z "$speedup" ] && speedup="N/A"
    [ -z "$efficiency" ] && efficiency="N/A"
    
    echo "$schedule,$chunk,$threads,$time,$speedup,$efficiency" >> $OUTPUT_FILE
    echo " Done: Time=$time, Speedup=$speedup, Efficiency=$efficiency%"
    
    rm tmp_output.txt
}

for schedule in 0 1 2; do
    case $schedule in
        0) schedule_name="static"; chunk=0 ;;
        1) schedule_name="dynamic"; chunk=1000 ;;
        2) schedule_name="guided"; chunk=1000 ;;
    esac
    
    echo -e "\nTesting $schedule_name schedule..."
    
    for threads in 2 4 6 8 10 12 14 16 18 20 22 24; do
        run_test $schedule $chunk $threads
    done
done

echo -e "\nBenchmark completed! Results saved to $OUTPUT_FILE"