#include <stdio.h>
#include <stdlib.h>
#include <SPL/Allocators.hpp>
#include <SPL/Utils.hpp>
#include <SPL/Scans.hpp>


int prec(char t) noexcept{
	switch (t){
	case '(': return 0;
	case '&': return 0;
	case ',': return 1;
	case '+': return 2;
	case '*': return 3;
	case '!': return 100;
	default: return 0;
	}
}

void print_error(const char *info, size_t pos){
	while (pos--) putc(' ', stderr);
	putc('^', stderr);
	putc('\n', stderr);
	fputs(info, stderr);
	putc('\n', stderr);
	exit(1);
}

MallocAllocator mallocator;

int main(){
	Array<char> input = read_text(stdin, mallocator);

	DynamicArray<char, mallocator> opers;
	push_value(opers, '\0');
	DynamicArray<size_t, mallocator> arg_counts;
	push(arg_counts);
	DynamicArray<char, mallocator> res;

	bool place_for_op = false;
	for (const char *it=beg(input); it!=end(input); ++it){
		char t = *it;
		if (('0' <= t && t <= '9') || ('a' <= t && t <= 'z')){
			[[unlikely]] if (place_for_op)
				print_error("expeceted operator, got value", it-beg(input));
			place_for_op = true;
			push_value(res, t);
		} else if (t=='+' || t=='*' || t=='!' || t==','){
			[[unlikely]] if (!place_for_op)
				print_error("expeceted value, got operator", it-beg(input));
			place_for_op = t == '!';
		
			arg_counts[arg_counts.size-1] += t == ',';
			for (;;){
				char last_op = opers[len(opers)-1];
				if (prec(t) > prec(last_op)) break;
				
				if (last_op != ',') push_value(res, last_op);
				pop(opers);
			}

			push_value(opers, t);
		} else if (t == '('){
			if (place_for_op){
				push_value(opers, '&');
				if (*(it+1) == ')'){
					it += 1;
					push_value(arg_counts, 0);
				} else{
					push_value(arg_counts, 1);
				}
			} else{
				push_value(opers, t);
			}
			place_for_op = false;
		} else if (t == ')'){
			while (opers[len(opers)-1] != '(' && opers[len(opers)-1] != '&'){
				if (opers[len(opers)-1] != ',') push_value(res, opers[opers.size-1]);
				pop(opers);
			}
			
			if (opers[len(opers)-1] == '&'){
				push_value(res, '[');
				push_value(res, '0'+arg_counts[arg_counts.size-1]);
				push_value(res, ']');
				pop(arg_counts);
			}
			pop(opers);
			place_for_op = true;
		}
	}
	for (;;){
		char last_op = opers[len(opers)-1];
		if (last_op == '\0') break;
		[[unlikely]] if (last_op == '(' || last_op == '&')
			print_error("not all pharenthesis were closed", len(input));
		if (opers[len(opers)-1] != ',') push_value(res, opers[opers.size-1]);
		pop(opers);
	}
	
	puts(beg(res));
	return 0;
}



