########################################################################
# plot_lab8.gp — три гистограммы по результатам ЛР 8
# 1) Время сборки
# 2) Время выполнения
# 3) Размер исполняемых файлов (с диапазоном 16–22 КБ, подписи на русском)
########################################################################

reset
set terminal pngcairo size 900,600 enhanced font "Arial,12"
set style data histograms
set style histogram clustered gap 1
set style fill solid border -1
set boxwidth 0.7
set grid ytics
set xtics rotate by -45 font ",10"
set key outside top center horizontal
set key autotitle columnhead

# -------------- 1) Время сборки --------------
set output "lab8_compile_time.png"
set title "Время сборки: компилятор clang + LTO" font "Arial,14"
set xlabel "Режим LTO" font "Arial,12"
set ylabel "Время сборки (сек)" font "Arial,12"

plot "lab8_data.dat" using 2:xtic(1) title "–O2" lc rgb "#1f77b4", \
     ""              using 3        title "–Oz" lc rgb "#ff7f0e"


# -------------- 2) Время выполнения --------------
set output "lab8_run_time.png"
set title "Время выполнения: решение 50 млн уравнений" font "Arial,14"
set xlabel "Режим LTO" font "Arial,12"
set ylabel "Время работы (сек)" font "Arial,12"

plot "lab8_data.dat" using 4:xtic(1) title "–O2" lc rgb "#1f77b4", \
     ""              using 5        title "–Oz" lc rgb "#ff7f0e"


# -------------- 3) Размер исполняемого файла --------------
set output "lab8_binary_size.png"
set title "Размер исполняемого файла (КБ)" font "Arial,14"
set xlabel "Режим LTO" font "Arial,12"
set ylabel "Размер (КБ)" font "Arial,12"
set yrange [16:22]

plot "lab8_data.dat" using 6:xtic(1) title "–O2" lc rgb "#1f77b4", \
     ""              using 7        title "–Oz" lc rgb "#ff7f0e"
