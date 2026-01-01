#!/bin/bash

set -e

mkdir -p vm

image_name=""
case "$1" in
  "alpine") image_name=alpine
  ;;
  "void") image_name=void
  ;;
  *)
    echo "usage: $0 <alpine|void>"
    exit 1
  ;;
esac

qemu-img create -f qcow2 vm/$image_name.qcow2 10G
