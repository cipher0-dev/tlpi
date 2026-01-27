.PHONY: all clean

REF_LIB_OBJS := $(patsubst ref/lib/%.c,build/ref/lib/%.o,$(wildcard ref/lib/*.c))
REF_PROG_SRCS := $(filter-out ref/lib/%.c,$(wildcard ref/*/*.c))
REF_PROG_BINS := $(patsubst ref/%.c,build/ref/%,$(REF_PROG_SRCS))
PROG_BINS := $(patsubst %.c,build/%,$(wildcard *.c))

all: $(PROG_BINS) $(REF_LIB_OBJS) $(REF_PROG_BINS)

clean:
	-rm -rf build 2>/dev/null

# programs binaries

build/%: %.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -I ref/lib -o $@ $^

# ref lib objects

build/ref/lib/%.o: ref/lib/%.c
	mkdir -p $(@D)
	gcc -c -I ref/lib -o $@ $^

# local targets

build/ref/proc/setenv: ref/proc/setenv.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -DTEST_IT -I ref/lib -o $@ $^
