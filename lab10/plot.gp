reset
set terminal pngcairo size 1920,1080 font "Arial,18"
set output "lab10_branch_predictions.png"

set title "Влияние макросов likely/unlikely и уровня оптимизации" font "Arial,20"
set ylabel "Время выполнения, с" font "Arial,18"
set xlabel "Уровень оптимизации компилятора" font "Arial,18"
set grid ytics

# Стиль группированных гистограмм
set style data histograms
set style histogram clustered gap 1
set style fill solid border -1
set boxwidth 0.9

# Легенда (ключ) вверху
set key outside samplen 1

# Ось x: подписи – уровни оптимизации
set xtics rotate by -45

# Читаем данные из файла: 
#   1-й столбец – строковый “Уровень” (O0, O1, …), 
#   2-й – Baseline, 3-й – Unlikely, 4-й – Likely, 5-й – Inverted
plot "lab10_data.dat" \
    using 2:xtic(1) title "Baseline (без подсказок)" lc rgb "#1f77b4", \
    "" using 3 title "Unlikely (верная подсказка)" lc rgb "#2ca02c", \
    "" using 4 title "Likely (неверная подсказка)" lc rgb "#d62728", \
    "" using 5 title "Inverted (перевернутая подсказка)" lc rgb "#9467bd"
