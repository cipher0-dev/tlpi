#!/bin/bash

mkdir -p results

echo ============= PIPES

for n in 10 100 1000 10000 100000 1000000 10000000 100000000; do
  echo ===== "$n" $((10000000000 / n))
  ./build/measure_pipes "$n" $((10000000000 / n))
  echo
done | tee results/pipes.txt

echo ============= SYSV MESSAGE QUEUES

for n in 10 100 1000 10000 100000 1000000 10000000 100000000; do
  echo ===== "$n" $((10000000000 / n))
  ./build/measure_sysv_msgqs "$n" $((10000000000 / n))
  echo
done | tee results/sysv_msgqs.txt

echo ============= POSIX MESSAGE QUEUES

for n in 10 100 1000 10000 100000 1000000 10000000 100000000; do
  echo ===== "$n" $((10000000000 / n))
  ./build/measure_posix_msgqs "$n" $((10000000000 / n))
  echo
done | tee results/posix_msgqs.txt

echo ============= UNIX STREAM SOCKETS

for n in 10 100 1000 10000 100000 1000000 10000000 100000000; do
  echo ===== "$n" $((10000000000 / n))
  ./build/measure_unix_stream_sockets "$n" $((10000000000 / n))
  echo
done | tee results/unix_stream_sockets.txt

echo ============= UNIX DATAGRAM SOCKETS

for n in 10 100 1000 10000 100000 1000000 10000000 100000000; do
  echo ===== "$n" $((10000000000 / n))
  ./build/measure_unix_datagram_sockets "$n" $((10000000000 / n))
  echo
done | tee results/unix_datagram_sockets.txt
