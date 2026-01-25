export LD_LIBRARY_PATH=./build

echo "=== Running Normally"
echo "===   Only one call is prefixed with !! which demonstrates that the"
echo "===   stdlib printf is otherwise used"
./build/test_linking_override
echo
echo

echo "=== Running with LD_PRELOAD"
echo "===   All calls are prefixed with !! to demonstrate that libtest's"
echo "===   printf overrides the stdlib printf in both libtest and main"
LD_PRELOAD=./build/libtest.so ./build/test_linking_override
