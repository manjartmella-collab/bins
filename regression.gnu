# Script gnuplot pour la regression lineaire
set terminal pngcairo enhanced font 'Arial,12' size 800,600
set output 'regression_plot.png'
set title 'Regression lineaire par moindres carres\ny = -7.4230 + 10.0533 x'
set xlabel 'x'
set ylabel 'y'
set grid
set key top left
set xrange [0.600000:5.400000]
set yrange [1.267700:49.139301]

plot 'donnees_plot.txt' with points pt 7 ps 1.5 lc rgb 'blue' title 'Donnees', \
     'droite_plot.txt' with lines lw 2 lc rgb 'red' title 'Droite de regression'
