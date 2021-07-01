#!/usr/bin/env sh

make

dd if=/dev/urandom of=urandom bs=1MB count=32

cat urandom | ./ws | ./ws -d > urandom.trans
md5sum urandom*

make clean

rm urandom*
