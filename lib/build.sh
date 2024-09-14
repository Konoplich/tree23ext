#!/bin/bash
F=knowhow.c
I=./test_interface/interface/test_public/public/test_protected/protected/test_private/private/test_knowhow/knowhow
gcc -shared -o libtree23ext.so -fPIC ${F} -I${I}
cp libtree23ext.so ..
cp libtree23ext.h ..
