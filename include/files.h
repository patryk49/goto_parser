#pragma once

#include "utils.h"

#include <stdlib.h>
#include <stdio.h>


#ifndef FILES_EXPECTED_LENGTH
	#define FILES_EXPECTED_LENGTH 64
#endif

String read_text(FILE *input){
	size_t capacity = FILES_EXPECTED_LENGTH;
	String res = {malloc(capacity), 0};
	if (res.ptr == NULL) return res;
	
	for (;;){
		int c = getc(input);
		[[unlikely]] if (c == EOF) break;

		if (res.size == capacity-1){
			char *new_ptr = realloc(res.ptr, res.size*2);
			if (new_ptr == NULL){
				free(res.ptr);
				res.ptr = new_ptr;
				break;
			}
			res.ptr = new_ptr;
			capacity = res.size*2;
		}

		res.ptr[res.size] = (char)c;
		res.size += 1;
	}

	res.ptr[res.size] = '\0';
	return res;
}
