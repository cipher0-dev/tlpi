#!/bin/bash

function create_copy_files() {
  mkdir /tmp/copy_files
  for size in {1K,10K,100K,1M,10M,100M,1G,10G}; do
    dd if=/dev/urandom of=/tmp/copy_files/$size bs=$size count=1
  done
}

function run_tests() {
  make # run make once to ensure lib objects are built

  # buffer sizes: 1024, 2048, 4096, 8192, 16384
  for buf_size in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216; do
    # recompile with new buffer size
    if ! gcc -O2 -DBUF_SIZE="$buf_size" -Iref/lib ../ch4/ref/fileio/copy.c -o build/copy build/ref/lib/*; then
      printf 'compile failed for BUF_SIZE=%d\n' "$buf_size" >&2
      return 1
    fi

    # iterate over test files
    for file in $(ls -1 /tmp/copy_files/ | sort -h); do
      [ -f "/tmp/copy_files/$file" ] || continue

      echo -ne "$buf_size\t$file\t"
      ( time ./build/copy "/tmp/copy_files/$file" /tmp/test ) 2>&1 | grep real | awk '{print $2}'
    done
  done | awk '{
    split($3, a, /[ms]/);
    total = (a[1]*60) + a[2];
    print $1, $2, total
  }' | tee build/data.txt
}

function plot_results() {
  gnuplot plot.gp
}

function main() {
  case "$1" in
    "create_copy_files")
      create_copy_files
      ;;
    "run_tests")
      run_tests
      ;;
    "plot_results")
      plot_results
      ;;
    *)
      echo "usage: <create_copy_files|run_tests|plot_results>" >&2
      ;;
  esac
}

main $@
