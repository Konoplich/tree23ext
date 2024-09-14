#!/bin/bash
rm ./ok
gcc main.c -o ok -L. -ltree23ext -Wl,-rpath,. -I.
