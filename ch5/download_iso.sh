#!/bin/bash

set -e

mkdir -p vm

name=""
url=""
case "$1" in
  "alpine")
    name=alpine
    url=https://dl-cdn.alpinelinux.org/alpine/latest-stable/releases/x86/alpine-virt-3.23.2-x86.iso
  ;;
  "void")
    name=void
    url=https://repo-default.voidlinux.org/live/current/void-live-i686-20250202-base.iso
  ;;
  *)
    echo "usage: $0 <alpine|void>"
    exit 1
  ;;
esac

wget -O vm/$name.iso $url
