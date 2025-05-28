import numpy as np

def f(x, y):
    return (x * y) ** 2

def is_in_region(x, y):
    return (1 < x * y < 2) and (abs(x - y) < 1)

# Параметры
num_samples = 1_000_000  # Увеличим число точек для точности
x_min, x_max = 0.5, 2.0  # Оптимизированные границы
y_min, y_max = 0.5, 2.0
area = (x_max - x_min) * (y_max - y_min)  # Площадь области генерации

# Генерация случайных точек
x_samples = np.random.uniform(x_min, x_max, num_samples)
y_samples = np.random.uniform(y_min, y_max, num_samples)

# Суммируем f(x, y) только для точек в D
sum_f = 0.0
count_in_D = 0

for x, y in zip(x_samples, y_samples):
    if is_in_region(x, y):
        sum_f += f(x, y)
        count_in_D += 1

# Вычисление интеграла
if count_in_D > 0:
    integral_value = (sum_f / num_samples) * area
else:
    integral_value = 0

print(f"Приближенное значение интеграла: {integral_value:.6f}")
print(f"Точность: ~{area / np.sqrt(count_in_D):.2e}")  # Оценка погрешности