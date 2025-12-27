.PHONY: getppid sys_getppid asm_getppid clean

build:
	mkdir build

clean:
	-rm -r build

build/getppid: build getppid.c
	gcc -o build/getppid getppid.c

build/sys_getppid: build sys_getppid.c
	gcc -o build/sys_getppid sys_getppid.c

build/asm_getppid: build asm_getppid.c
	gcc -nostdlib -o build/asm_getppid asm_getppid.c

getppid: build/getppid
	build/getppid

sys_getppid: build/sys_getppid
	build/sys_getppid

asm_getppid: build/asm_getppid
	build/asm_getppid
