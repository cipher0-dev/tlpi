set terminal pngcairo size 800,600 enhanced font 'Arial,12'
set output 'build/plot.png'

set title "Benchmark by File Size"
set xlabel "Buffer Size (bytes)"
set ylabel "Time (seconds)"
set logscale x 2
set grid

# Plot each file size as a separate line
plot \
    'build/data.txt' u 1:(strcol(2) eq "1K" ? $3 : 1/0) w linespoints lt rgb "blue" pt 7 lw 2 title "1K", \
    'build/data.txt' u 1:(strcol(2) eq "10K" ? $3 : 1/0) w linespoints lt rgb "red" pt 7 lw 2 title "10K", \
    'build/data.txt' u 1:(strcol(2) eq "100K" ? $3 : 1/0) w linespoints lt rgb "green" pt 7 lw 2 title "100K", \
    'build/data.txt' u 1:(strcol(2) eq "1M" ? $3 : 1/0) w linespoints lt rgb "orange" pt 7 lw 2 title "1M", \
    'build/data.txt' u 1:(strcol(2) eq "10M" ? $3 : 1/0) w linespoints lt rgb "purple" pt 7 lw 2 title "10M", \
    'build/data.txt' u 1:(strcol(2) eq "100M" ? $3 : 1/0) w linespoints lt rgb "brown" pt 7 lw 2 title "100M", \
    'build/data.txt' u 1:(strcol(2) eq "1G" ? $3 : 1/0) w linespoints lt rgb "cyan" pt 7 lw 2 title "1G", \
    'build/data.txt' u 1:(strcol(2) eq "10G" ? $3 : 1/0) w linespoints lt rgb "black" pt 7 lw 2 title "10G"
