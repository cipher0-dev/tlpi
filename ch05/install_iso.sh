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
  -cdrom vm/$name.iso \
  -drive file=vm/$name.qcow2,format=qcow2,if=virtio \
  -boot order=d \
  $graphic_flag

# once vm is booted of the cd, you need to install with:
#   setup-alpine
#   void-installer

# if anything goes wrong in qemu you can hit:
#    Ctrl+A Ctrl+C and then type quit to exit
