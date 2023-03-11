reset
set encoding utf8
set style fill transparent solid 0.65

set xlabel 'deg'
set ylabel 'dim'
set zlabel 't, мс' offset 8, 10, 0

set yr [0:400]
set xyplane at 0

splot '1.txt' matrix using 1:($2*5.12):3 with lines title "Алгоритм Берлекэмпа", '2.txt' matrix using 1:($2*5.12):3 with lines title "Алгоритм Кантора-Цассенхауса", '3.txt' matrix using 1:($2*5.12):3 with lines title "Тест Рабина"