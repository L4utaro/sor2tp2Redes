set terminal png
set output "dumbbell-tp2-cwnd-ssthresh.png"
set xlabel "Time(seconds)"
set ylabel "Segmentos"
set title "2 tcp Congestion Window Plot"
plot "dumbbell-tp2-20-cwnd.data" using 1:2 title "cwnd-nodo2-interfaz0"with lines, \
"dumbbell-tp2-20-ssth.data" using 1:2 title "ssthresh-nodo2-interfaz0" with lines, \
"dumbbell-tp2-30-cwnd.data" using 1:2 title "cwnd-nodo3-interfaz0"with lines, \
"dumbbell-tp2-30-ssth.data" using 1:2 title "ssthresh-nodo3-interfaz0" with lines
