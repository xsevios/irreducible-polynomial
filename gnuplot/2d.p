reset
set encoding utf8
set style fill transparent solid 0.65

set xlabel 'deg'
set ylabel 'dim'

set yr [0:400]
set xyplane at 0

plot 'berlekamp.txt' matrix using ($2*5.12) with lines title "Алгоритм Берлекэмпа", 'cantorzassenhaus.txt' matrix using 1:($2*5.12) with lines title "Алгоритм Кантора-Цассенхауса", 'rabin.txt' matrix using 1:($2*5.12) with lines title "Тест Рабина"
