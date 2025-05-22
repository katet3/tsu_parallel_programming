import numpy as np

# Определяем функцию f(x, y)
def f(x, y):
    return (x * y) ** 2

# Определяем функцию для проверки, принадлежит ли точка области D
def is_in_region(x, y):
    return (1 < x * y < 2) and (abs(x - y) < 1)

# Параметры
num_samples = 100000  # Количество случайных точек

# Генерация случайных точек
x_samples = np.random.uniform(0, 2, num_samples)
y_samples = np.random.uniform(0, 2, num_samples)

# Фильтрация точек, которые находятся в области D
valid_points = [(x, y) for x, y in zip(x_samples, y_samples) if is_in_region(x, y)]

# Вычисление интеграла методом Монте-Карло
if valid_points:
    valid_x, valid_y = zip(*valid_points)
    integral_value = np.mean(f(np.array(valid_x), np.array(valid_y))) * len(valid_points) / num_samples * 4  # Умножаем на площадь области
else:
    integral_value = 0

print("Приближенное значение двойного интеграла:", integral_value)