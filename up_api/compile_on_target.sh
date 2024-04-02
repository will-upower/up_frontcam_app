#!/bin/bash

gcc -c -fpic col_cnvr.c -o col_cnvr.o -lOpenCL -lm
gcc -shared -o up_api.so col_cnvr.o
