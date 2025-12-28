.PHONY: getppid sys_getppid asm_getppid errno reboot syscall_speed nosyscall_speed clean

build:
	mkdir build

build/ref/lib: build
	mkdir -p build/ref/lib

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

build/ref/lib/get_num.o: build/ref/lib ref/lib/get_num.c
	gcc -I ref/lib -o build/ref/lib/get_num.o -c ref/lib/get_num.c

build/syscall_speed: build build/ref/lib/get_num.o ref/progconc/syscall_speed.c
	gcc -I ref/lib -o build/syscall_speed build/ref/lib/get_num.o ref/progconc/syscall_speed.c

build/nosyscall_speed: build ref/progconc/syscall_speed.c
	gcc -I ref/lib -DNOSYSCALL -o build/nosyscall_speed build/ref/lib/get_num.o ref/progconc/syscall_speed.c

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

syscall_speed: build/syscall_speed
	bash -c 'time build/syscall_speed 10000000'

nosyscall_speed: build/nosyscall_speed
	bash -c 'time build/nosyscall_speed 10000000'
