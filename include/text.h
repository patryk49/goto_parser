#pragma once

#include "utils.h"

#include <stdlib.h>
#include <stdio.h>


#ifndef FILES_EXPECTED_LENGTH
	#define FILES_EXPECTED_LENGTH 64


String read_text(FILE *input){
	size_t capacity = FILES_EXPECTED_LENGTH;
	String res = {malloc(capacity), 0};
	if (res.ptr == nullptr) return res;
	
	for (;;){
		int c = getc(input);
		[[unlikely]] if (c == EOF) break;

		if (res.size == capacity){
			char *new_ptr = realloc(res.data, res.size*2);
			if (new_ptr == nullptr){
				free(res.ptr);
				res.ptr = new_ptr;
				return res;
			}
			res.ptr = new_ptr;
			res.cap = new_cap;
		}

		res[res.size] = (char)c;
		res.size += 1;
	}

	return res;
}
