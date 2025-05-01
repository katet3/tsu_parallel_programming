# Компилятор MPI
MPICC = mpicc
# Количество процессов по умолчанию
NP = 4

# Правило по умолчанию
.DEFAULT_GOAL := help

# Помощь
help:
	@echo "Использование:"
	@echo "  make build TARGET=<имя_программы>  - компилирует программу"
	@echo "  make run TARGET=<имя_программы>    - запускает программу"
	@echo "  make clean TARGET=<имя_программы>  - удаляет исполняемый файл"
	@echo ""
	@echo "Пример:"
	@echo "  make build TARGET=task_1"
	@echo "  make run TARGET=task_1 NP=4"
	@echo "  make clean TARGET=task_1"

# Сборка программы
build:
	$(MPICC) $(TARGET).c -o $(TARGET).out

# Запуск программы
run:
	mpirun --mca btl_vader_single_copy_mechanism none -np $(NP) --oversubscribe ./$(TARGET).out

# Очистка
clean:
	rm -f $(TARGET).out

.PHONY: help build run clean