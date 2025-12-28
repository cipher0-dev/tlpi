.PHONY: getppid sys_getppid asm_getppid errno reboot clean

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

build/errno: build errno.c
	gcc -o build/errno errno.c

build/reboot: build reboot.c
	gcc -o build/reboot reboot.c

getppid: build/getppid
	build/getppid

sys_getppid: build/sys_getppid
	build/sys_getppid

asm_getppid: build/asm_getppid
	build/asm_getppid

errno: build/errno
	build/errno

reboot: build/reboot
	build/reboot
