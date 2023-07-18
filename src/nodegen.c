#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "files.h"

#define STRING(str) ((String){str, sizeof(str)-1})


static String make_string(size_t capacity){ return (String){malloc(capacity), 0}; }

static bool is_valid_name_char(char c){
	return c=='_' || ('a'<=c && c<='z') || ('A'<=c && c<='Z') || ('0'<=c && c<='9');
}

static String get_name(char **it){
	String res = {*it, 0};
	while (is_valid_name_char(res.ptr[res.size])) res.size += 1;
	*it = res.ptr + res.size;
	return res;
}
/*
static String u64_name(String s){
	static char buf[24];
	for (size_t i=0; i!=sizeof(buf); i+=1) buf[i] = '0';
	buf[1] = 'x';
	
	static const char HexTable[16] = "0123456789abcdef";
	for (size_t i=0; i!=s.size && i!=8; i+=1){
		buf[2+i] = HexTable[(tolower(s.ptr[i]) >> 4) & 0xf];
		buf[3+i] = HexTable[(tolower(s.ptr[i]) >> 0) & 0xf];
	}

	return (String){buf, 18};
}*/

static void string_append(String *dest, String src){
	memcpy(dest->ptr+dest->size, src.ptr, src.size);
	dest->size += src.size;
}

static void string_append_quoted(String *dest, String src){
	dest->ptr[dest->size] = '\"';
	memcpy(dest->ptr+dest->size+1, src.ptr, src.size);
	dest->ptr[dest->size+src.size+1] = '\"';
	dest->size += src.size + 2;
}

void print_index(char *it, size_t n){
	*it = '0'+(n / 1000); n %= 1000; it += 1;
	*it = '0'+(n /  100); n %=  100; it += 1;
	*it = '0'+(n /   10); n %=   10; it += 1;
	*it = '0'+(n /    1); n %=    1; it += 1;
}

void skip_spaces(char **iter){
	const char *it = *iter;
	for (;;){
		if (*it == '/'){
			it += 1;
			if (*it == '/'){
				while (it+=1, *it!='\n') if (*it=='\0') return;
			} else if (*it == '*'){
				while (it+=1, it[0]!='*'||it[1]=='/') if (*it=='\0') return;
				it += 1;
			}
		}
		if (*it!='\n' && *it!=' ') break;
		it += 1;
	}
	*iter = it;
}

typedef struct{
	size_t pos;
	String text;
} CommnentData;

static const String DefaultPrec = {"255", 3};

int main(int argc, char **argv){
	const char *inpath = "include/node_types.txt";
	const char *outpath = "include/node_types.h";
	
	FILE *infile = fopen(inpath, "r");
	if (infile == NULL){
		printf("File \"%s\" doesn\'t exist.\n", inpath);
		return -1;
	}
	String input = read_text(infile);
	fclose(infile);

	String res_enum = make_string(2*input.size);
	String res_str = make_string(2*input.size);
	String res_pl = make_string(input.size);
	String res_pr = make_string(input.size);
	String res_indexes = make_string(3*input.size);
	size_t count = 0;

	


	char *it = input.ptr;

	while (it < input.ptr+input.size){
		skip_spaces(&it);
		if (!is_valid_name_char(*it)){
			printf("WRONG FORMAT\n");
			return 1;
		}
		String name = get_name(&it);
		static const String PrefixString = {"Node_", 5};
		string_append(&res_enum, PrefixString);
		string_append(&res_enum, name);
		string_append_quoted(&res_str, name);
		static const String CommaString= {", ", 2};
		string_append(&res_enum, CommaString);
		string_append(&res_str, CommaString);
		char buf[16] = "----  -  ";
		print_index(buf, count);
		String IndexString = {NULL, 9}; IndexString.ptr = buf;
		string_append(&res_indexes, IndexString);
		string_append(&res_indexes, name);
		res_indexes.ptr[res_indexes.size] = '\n'; res_indexes.size += 1;
		count += 1;
		skip_spaces(&it);
		String pl = DefaultPrec;
		String pr = DefaultPrec;
		if (*it == ':'){
			it += 1;
			skip_spaces(&it);
			char *old_it = it;
			strtol(old_it, &it, 10);
			if (it == old_it){
				printf("WRONG FORMAT\n");
				return 1;
			}
			pl.ptr = old_it;
			pl.size = it - old_it;
			if (*it == '/'){
				it += 1;
				skip_spaces(&it);
				char *old_it = it;
				strtol(old_it, &it, 10);
				if (it == old_it){
					printf("WRONG FORMAT\n");
					return 1;
				}
				pr.ptr = old_it;
				pr.size = it - old_it;
			}
		}
		string_append(&res_pl, pl);
		string_append(&res_pl, STRING(", "));

		string_append(&res_pr, pr);
		string_append(&res_pr, STRING(", "));
	}

	// remove trailing commans
	if (count != 0){
		res_enum.size -= 2;
		res_str.size  -= 2;
		res_pl.size   -= 2;
		res_pr.size   -= 2;
	}


	FILE *out = fopen(outpath, "w");
	if (out == NULL){
		printf("File \"%s\" cannot be created.\n", outpath);
		return -1;
	}
	
	fputs("#pragma once\n", out);
	fputs("#include <stdint.h>\n", out);
	fputs("#include <stddef.h>\n\n\n\n", out);
	fprintf(out, "static const size_t NodeTypeCount = %lu;\n\n\n\n", count);
	
	fputs("typedef uint8_t NodeType;\nenum NodeType{\n", out);
	fwrite(res_enum.ptr, 1, res_enum.size, out);
	fputs("\n};\n\n\n\n", out);

	fputs("static const char *NodeTypeNames[] = {\n", out);
	fwrite(res_str.ptr, 1, res_str.size, out);
	fputs("\n};\n\n\n\n", out);

	fputs("static const uint8_t PrecsLeft[] = {\n", out);
	fwrite(res_pl.ptr, 1, res_pl.size, out);
	fputs("\n};\n\n\n\n", out);
	
	fputs("static const uint8_t PrecsRight[] = {\n", out);
	fwrite(res_pr.ptr, 1, res_pr.size, out);
	fputs("\n};\n\n\n\n", out);

	fputs("/*   << KEYWORD INDEXES >>\n", out);
	fwrite(res_indexes.ptr, 1, res_indexes.size, out);
	fputs("*/\n", out);

	fclose(out);
	return 0;
}
