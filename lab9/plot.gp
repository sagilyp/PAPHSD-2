###########################################################################
# plot.gp — три отдельных гистограммы на основе lab9_data.dat
#
# Запуск:
#     cd в папку с lab9_data.dat и plot.gp
#     gnuplot plot.gp
#
# В результате:
#   1) quadratic_times.png      — время решения квадратных уравнений (Часть A)
#   2) sum_times.png            — время «бесполезной суммы» (Часть B)
#   3) baseline_noomp_comp.png  — сравнение BASELINE_NOOMP по флагам
###########################################################################

# Общие настройки
set terminal pngcairo size 1000,600 font "Arial,12"
set datafile separator whitespace
set style histogram clustered gap 1
set style fill solid border -1
set key outside font "Arial,10"
set grid ytics

##########################################################
# 1) Часть A: Время решения квадратных уравнений (N = 50 000 000)
#    Данные занимают физические строки 3…130 в lab9_data.dat
#    (в Gnuplot они имеют индексы 2…129 при n=0).
#    Столбцы:
#       1 = компилятор       (например, -O0)
#       2 = режим (mode)     (например, BASELINE, CONST_ALL…)
#       3 = bound            (var/const)
#       4 = OpenMP (YES/NO)
#       5 = время (в секундах)
#
#    Подписи по оси X: «<1>_<2>_<3>_<4>», получаем через sprintf/ stringcolumn.
##########################################################

set output "quadratic_times.png"
set title "Часть A: Время решения квадратных уравнений (N=50 млн)" font "Arial,14"
set xlabel "Compiler_Mode_Bound_OMP" font "Arial,12"
set ylabel "Время (с)" font "Arial,12"
set xtics rotate by -65 font "Arial,8"

# Разбиваем файл: берем только строки с индексами 2…129 (то есть физические 3…130),
# из них колонка #5 — это высота столбика, а xtic-метка собирается из колонок 1…4.
plot 'lab9_data.dat' every ::2::129 using 5:xtic(sprintf("%s_%s_%s_%s", \
       stringcolumn(1),stringcolumn(2),stringcolumn(3),stringcolumn(4))) \
     title "" linecolor rgb "#1f77b4"


##########################################################
# 2) Часть B: «Бесполезная сумма» (volatile vs. non-volatile)
#    Данные занимают физические строки 133…140 (в Gnuplot — 132…139).
#    Столбцы:
#       1 = компилятор  (например, -O0)
#       2 = время sum_non_volatile
#       3 = время sum_volatile
##########################################################

set output "sum_times.png"
set title "Часть B: Время «бесполезной суммы»" font "Arial,14"
set xlabel "Компилятор" font "Arial,12"
set ylabel "Время (с)" font "Arial,12"
set xtics rotate by -45 font "Arial,10"

# Первая серия: non-volatile (колонка 2), вторая: volatile (колонка 3).
plot 'lab9_data.dat' every ::132::139 using 2:xtic(1) title "non-volatile" \
       linecolor rgb "#1f77b4", \
     ''                  every ::132::139 using 3 title "volatile" \
       linecolor rgb "#ff7f0e"


##########################################################
# 3) BASELINE_NOOMP — сравнение флагов компиляции (Часть A)
#    Нам нужны строки, где mode=BASELINE, bound=var, omp=NO.
#    В lab9_data.dat они расположены на следующих физ. строках:
#      -O0   : строка  3   (Gnuplot индекс 2)
#      -O1   : строка 19   (Gnuplot индекс 18)
#      -O2   : строка 35   (Gnuplot индекс 34)
#      -O3   : строка 51   (Gnuplot индекс 50)
#      -Ofast: строка 67   (Gnuplot индекс 66)
#      -Og   : строка 83   (Gnuplot индекс 82)
#      -Os   : строка 99   (Gnuplot индекс 98)
#      -Oz   : строка 115  (Gnuplot индекс 114)
#
#    Чтобы проще было, извлечём эти восемь значений во вспомогательный файл
#    и затем построим гистограмму. Но мы можем это сделать прямо из lab9_data.dat:
##########################################################

# Сначала соберём мини-таблицу «compiler time» в отдельный файл.
# (Запустите эту команду один раз в терминале, до вызова plot.gp):
# awk 'NR==3   { printf "-O0\t%s\n",   $5 }
#      NR==19  { printf "-O1\t%s\n",   $5 }
#      NR==35  { printf "-O2\t%s\n",   $5 }
#      NR==51  { printf "-O3\t%s\n",   $5 }
#      NR==67  { printf "-Ofast\t%s\n",$5 }
#      NR==83  { printf "-Og\t%s\n",   $5 }
#      NR==99  { printf "-Os\t%s\n",   $5 }
#      NR==115 { printf "-Oz\t%s\n",   $5 }' lab9_data.dat > baseline_noomp.dat

# После этого у вас появится файл baseline_noomp.dat с точно восемью строчками:
#     -O0      0.470883
#     -O1      0.0522144
#     -O2      0.0484383
#     -O3      0.0507948
#     -Ofast   0.0486733
#     -Og      0.313153
#     -Os      0.287595
#     -Oz      0.301222

set output "baseline_noomp_comp.png"
set title "BASELINE_NOOMP: сравнение флагов компиляции" font "Arial,14"
set xlabel "Флаг компиляции" font "Arial,12"
set ylabel "Время решения (с)" font "Arial,12"
set xtics rotate by -45 font "Arial,10"

# Теперь строим ровно 8 столбцов из baseline_noomp.dat (колонка 2 = height, xtic=колонка 1).
plot "baseline_noomp.dat" using 2:xtic(1) title "BASELINE_NOOMP" linecolor rgb "#2ca02c"

###########################################################################
# Конец скрипта
###########################################################################
