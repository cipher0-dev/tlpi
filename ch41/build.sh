set -oeu pipefail

mkdir build 2>/dev/null || true

gcc -g -fPIC -fno-builtin -Wall --shared -o build/libtest.so libtest.c
gcc -g -Wall -fno-builtin -o build/test_linking_override test_linking_override.c -L./build -lc -ltest
