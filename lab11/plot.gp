# plot_vcs.gnuplot

set terminal pngcairo size 1720,1080 font "Arial,20"
set output "vcs_comparison.png"

set title "Сравнение производительности СКВ" font "Arial,20"
set xlabel "Операция" font "Arial,18"
set ylabel "Время, с" font "Arial,18"
set grid ytics
set logscale y

set style data histograms
set style histogram clustered gap 1
set style fill solid border -1
set boxwidth 0.7
set xtics rotate by -45

set key outside top center horizontal

plot 'results_vcs.dat' using 2:xtic(1) title 'Git'        lc rgb "#4E79A7", \
                            '' using 3 title 'SVN'        lc rgb "#F28E2B", \
                            '' using 4 title 'Mercurial'  lc rgb "#E15759", \
                            '' using 5 title 'CVS'        lc rgb "#76B7B2", \
                            '' using 6 title 'Fossil'     lc rgb "#59A14F"
