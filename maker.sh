#!/bin/bash

gcc src/$1.c -o bin/$1 -O2 -mavx -std=c2x\
	-Iinclude \
	-Wall -Wextra -Wno-attributes -Wno-unused-function \
	$2 $3 $4 $5
