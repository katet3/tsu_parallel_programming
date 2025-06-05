# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -fopenmp -Wall -O3

# Флаги линковки
LDFLAGS = -fopenmp

# Исходные файлы (по умолчанию)
SRCS = lab_6.c

# Определяем имя целевого файла, если не задано через параметр
ifndef TARGET
    TARGET = omp_program
endif

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Количество процессов для запуска (по умолчанию 1)
ifndef NP
    NP = 1
endif

# Параметры расписания (по умолчанию static)
ifndef SCHEDULE
    SCHEDULE = 0
endif

# Размер чанка (по умолчанию 1000)
ifndef CHUNK
    CHUNK = 1000
endif

.PHONY: all build run clean

all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm

run: $(TARGET)
	@echo "Running $(TARGET) with $(NP) threads..."
	@OMP_NUM_THREADS=$(NP) ./$(TARGET) $(SCHEDULE) $(CHUNK)

clean:
	rm -f $(OBJS) $(TARGET)

help:
	@echo "Available targets:"
	@echo "  make build TARGET=lab_6    - Build executable with specified name"
	@echo "  make run TARGET=lab_6 NP=4 - Run program with 4 threads"
	@echo "  make clean                 - Remove object files and executable"
	@echo "  make help                  - Show this help message"
	@echo ""
	@echo "Additional run options:"
	@echo "  SCHEDULE - scheduling type (0=static, 1=dynamic, 2=guided)"
	@echo "  CHUNK    - chunk size for dynamic/guided scheduling"