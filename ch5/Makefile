.PHONY: all clean

REF_LIB_OBJS := $(patsubst ref/lib/%.c,build/ref/lib/%.o,$(wildcard ref/lib/*.c))
REF_PROG_SRCS := $(filter-out ref/lib/%.c,$(wildcard ref/*/*.c))
REF_PROG_BINS := $(patsubst ref/%.c,build/ref/%,$(REF_PROG_SRCS))
PROG_BINS := $(patsubst %.c,build/%,$(wildcard *.c)) build/large_file_lfs_32 build/large_file_lfs_64

all: $(PROG_BINS) $(REF_LIB_OBJS) $(REF_PROG_BINS)

clean:
	-rm -rf build 2>/dev/null

# programs binaries

build/%: %.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -I ref/lib -o $@ $^

# this target explicity sets the offset size to 32 bits
# this is ignored if linking against musl, glibc only
# when compiling the default large_file target on 32 bit
# it will have the same behavior as this target
build/large_file_lfs_32: large_file.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -D_FILE_OFFSET_BITS=32 -I ref/lib -o $@ $^

# this target explicity sets the offset size to 64 bits
# this is ignored if linking against musl, glibc only
build/large_file_lfs_64: large_file.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -D_FILE_OFFSET_BITS=64 -I ref/lib -o $@ $^

# ref lib objects

build/ref/lib/%.o: ref/lib/%.c
	mkdir -p $(@D)
	gcc -c -I ref/lib -o $@ $^

# ref program binaries

build/ref/fileio/%: ref/fileio/%.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -I ref/lib -o $@ $^
