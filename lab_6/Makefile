# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -fopenmp -Wall -O3

# Флаги линковки
LDFLAGS = -fopenmp

# Исходные файлы
SRCS = lab_6.c

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Имя исполняемого файла
TARGET = omp_program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean