#!/bin/bash

set -e

name=""
graphic_flag=""
case "$1" in
  "alpine")
    name=alpine
    graphic_flag="-nographic"
  ;;
  "void")
    name=void
  ;;
  *)
    echo "usage: $0 <alpine|void>"
    exit 1
  ;;
esac

qemu-system-i386 \
  -enable-kvm \
  -m 1G \
  -cpu host \
  -drive file=vm/$name.qcow2,format=qcow2,if=virtio \
  -virtfs local,path=$PWD,mount_tag=host_dir,security_model=none \
  $graphic_flag

# to mount the host dir:
#   mkdir -p /mnt/host
#   mount -t 9p -o trans=virtio,version=9p2000.L host_dir /mnt/host

# to install compiler
#   apk update
#   apk add build-base

# if anything goes wrong in qemu you can hit:
#    Ctrl+A Ctrl+C and then type quit to exit
