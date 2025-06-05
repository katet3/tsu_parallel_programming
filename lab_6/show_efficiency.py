import matplotlib.pyplot as plt

# Данные
threads = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24]

# Эффективность для разных стратегий
static_eff = [101.52, 95.21, 95.81, 93.12, 89.95, 81.09, 47.87, 47.19, 45.61, 44.41, 45.29, 43.21]
dynamic_eff = [104.36, 100.73, 100.48, 95.41, 93.88, 87.82, 75.74, 66.22, 60.29, 51.69, 47.27, 42.20]
guided_eff = [101.20, 101.04, 93.69, 94.10, 91.88, 94.00, 75.62, 65.72, 59.63, 52.67, 47.56, 43.86]

# Идеальная эффективность
ideal_eff = [100] * len(threads)

# Создание графика
plt.figure(figsize=(12, 7))
plt.plot(threads, ideal_eff, 'k--', linewidth=1.5, label='Идеальная эффективность')
plt.plot(threads, static_eff, 'bo-', linewidth=2, markersize=8, label='Static')
plt.plot(threads, dynamic_eff, 'g^-', linewidth=2, markersize=8, label='Dynamic')
plt.plot(threads, guided_eff, 'rs-', linewidth=2, markersize=8, label='Guided')

# Настройка графика
plt.title('Зависимость эффективности от количества потоков', fontsize=14, fontweight='bold')
plt.xlabel('Количество потоков', fontsize=12)
plt.ylabel('Эффективность (%)', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(fontsize=12)
plt.xticks(threads, fontsize=10)
plt.yticks(fontsize=10)
plt.ylim(30, 110)  # Установка диапазона для лучшей визуализации
plt.tight_layout()

# Сохранение и отображение
plt.savefig('efficiency_comparison.png', dpi=300)
plt.show()