#!/usr/bin/env sh

make

dd if=/dev/urandom of=urandom bs=1MB count=32

cat urandom | ./ws | ./ws -d > urandom.trans
cat urandom | ./ws -a poly | ./ws -d -a poly > urandom.poly.trans
cat urandom | ./ws -a switch | ./ws -d -a switch > urandom.switch.trans
cat urandom | ./ws -a switch | ./ws -d -a poly > urandom.switch.poly.trans
md5sum urandom*

make clean

rm urandom*
