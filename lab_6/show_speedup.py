import matplotlib.pyplot as plt

# Данные
threads = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24]

# Ускорение для разных стратегий
static_speedup = [2.03, 3.81, 5.75, 7.45, 8.99, 9.73, 6.70, 7.55, 8.21, 8.88, 9.96, 10.37]
dynamic_speedup = [2.09, 4.03, 6.03, 7.63, 9.39, 10.54, 10.60, 10.59, 10.85, 10.34, 10.40, 10.13]
guided_speedup = [2.02, 4.04, 5.62, 7.53, 9.19, 11.28, 10.59, 10.52, 10.73, 10.53, 10.46, 10.53]

# Идеальное ускорение
ideal = threads

# Создание графика
plt.figure(figsize=(12, 7))
plt.plot(threads, ideal, 'k--', linewidth=1.5, label='Идеальное ускорение')
plt.plot(threads, static_speedup, 'bo-', linewidth=2, markersize=8, label='Static')
plt.plot(threads, dynamic_speedup, 'g^-', linewidth=2, markersize=8, label='Dynamic')
plt.plot(threads, guided_speedup, 'rs-', linewidth=2, markersize=8, label='Guided')

# Настройка графика
plt.title('Зависимость ускорения от количества потоков', fontsize=14, fontweight='bold')
plt.xlabel('Количество потоков', fontsize=12)
plt.ylabel('Ускорение (Speedup)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=12)
plt.xticks(threads, fontsize=10)
plt.yticks(fontsize=10)
plt.tight_layout()

# Сохранение и отображение
plt.savefig('speedup_comparison.png', dpi=300)
plt.show()