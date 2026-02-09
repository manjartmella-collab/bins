set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output 'regression_exp.png'
set grid
set xlabel 'x'
set ylabel 'y'
plot 'donnees_plot.txt' with points pt 7 ps 1.5 title 'Données', \
     'exp_plot.txt' with lines lw 2 lc rgb 'red' title sprintf('a=0.290341 b=0.034736', 0.290341, 0.034736)
