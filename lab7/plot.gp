# -----------------------------------------------
# plot_lab7_three_plots.gp
# Три отдельных графика: 
# 1) Время сборки (CompileTime)
# 2) Время выполнения (RunTime)
# 3) Размер исполняемого файла (Size_KB)
# Источник данных: lab7_data.dat
# -----------------------------------------------

# -------------------------------
# График 1: Время сборки
# -------------------------------
set terminal pngcairo size 1000,600 enhanced font "Arial,12"
set output "lab7_compile_time.png"

set title "Время сборки компилятора (CompileTime)" font "Arial,14"
set xlabel "Уровень оптимизации" font "Arial,12"
set ylabel "Время сборки, с"       font "Arial,12"

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.6

set grid ytics
set xtics rotate by -45

# lab7_data.dat:
#   Колонка 1 = уровень (O0, Og, …)
#   Колонка 2 = CompileTime
#   Колонка 3 = Size_KB
#   Колонка 4 = RunTime

plot 'lab7_data.dat' using 2:xtic(1) title "" lc rgb "#1f77b4"


# -------------------------------
# График 2: Время выполнения
# -------------------------------
set output "lab7_run_time.png"

set title "Время выполнения программы (RunTime)" font "Arial,14"
set xlabel "Уровень оптимизации"            font "Arial,12"
set ylabel "Время выполнения (50M уравнений), с" font "Arial,12"

# Повторяем стиль гистограммы
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.6

set grid ytics
set xtics rotate by -45

plot 'lab7_data.dat' using 4:xtic(1) title "" lc rgb "#ff7f0e"


# -------------------------------
# График 3: Размер исполняемого файла
# -------------------------------
set output "lab7_size.png"

set title "Размер исполняемого файла (Size_KB)" font "Arial,14"
set xlabel "Уровень оптимизации"      font "Arial,12"
set ylabel "Размер, КБ"               font "Arial,12"

# Сохраняем те же стили
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.6

set grid ytics
set xtics rotate by -45

plot 'lab7_data.dat' using 3:xtic(1) title "" lc rgb "#2ca02c"
