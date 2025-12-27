.PHONY: unsafe safe clean

build:
	mkdir build

clean:
	rm -r build

build/unsafe: build
	gcc -g -O0 -o build/unsafe unsafe_environ_race.c

build/safe: build
	gcc -g -O0 -o build/safe safe_environ_race.c

unsafe: build/unsafe
	valgrind --tool=helgrind build/unsafe

safe: build/safe
	valgrind --tool=helgrind build/safe
