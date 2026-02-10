.PHONY: all clean

REF_LIB_OBJS := $(patsubst ref/lib/%.c,build/ref/lib/%.o,$(wildcard ref/lib/*.c))
REF_PROG_SRCS := $(filter-out ref/lib/%.c,$(wildcard ref/*/*.c))
REF_PROG_BINS := $(patsubst ref/%.c,build/ref/%,$(REF_PROG_SRCS))
PROG_BINS := $(patsubst %.c,build/%,$(wildcard *.c))
ZIG_PROG_BINS := $(patsubst %.zig,build/%,$(filter-out build.zig,$(wildcard *.zig)))
GO_PROG_BINS := $(patsubst %.go,build/%,$(wildcard *.go))

all: $(PROG_BINS) $(REF_LIB_OBJS) $(REF_PROG_BINS) $(ZIG_PROG_BINS) $(GO_PROG_BINS)

clean:
	-rm -rf build 2>/dev/null
	-rm -rf .zig-cache 2>/dev/null

# programs binaries

build/%: %.c $(REF_LIB_OBJS)
	mkdir -p $(@D)
	gcc -g -O0 -I ref/lib -o $@ $^

build/%: %.zig $(REF_LIB_OBJS)
	mkdir -p $(@D)
	zig cc -I ref/lib -o $@ $^

build/%: %.go
	mkdir -p $(@D)
	go build -o $@ $^

# ref lib objects

build/ref/lib/%.o: ref/lib/%.c
	mkdir -p $(@D)
	gcc -c -I ref/lib -o $@ $^
