#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define SIZE(arr) (sizeof(arr)/sizeof(*arr))

typedef struct{
	char *ptr;
	size_t size;
} String;
