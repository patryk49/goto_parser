#include "node_types.h"
#include "tokenizer.h"
#include "files.h"
#include "parser.h"

#include <time.h>


void print_nodes(NodeArray, const char *, const char *);


int main(int argc, char **argv){
	bool verbose = true;
	if (argc == 2 && argv[1][0]=='q' && argv[1][1]=='\0') verbose = false;

	time_t read_time = clock();
	String text = read_text(stdin);
	read_time = clock() - read_time;

	time_t tok_time = clock();
	NodeArray nodes = make_tokens(text.ptr);
	tok_time = clock() - tok_time;
	if (verbose){
		puts(">>> TOKENS >>>");
		print_nodes(nodes, text.ptr, "t");
		putchar('\n');
	}

	time_t parse_time = clock();
	parse_module(nodes);
	parse_time = clock() - parse_time;
	if (verbose){
		puts(">>> AST_NODES >>>");
		print_nodes(nodes, text.ptr, "p");
		putchar('\n');
	}
	printf("reading time: %lf\n", (double)read_time/1000000.0);
	printf("lexing time:  %lf\n", (double)tok_time/1000000.0);
	printf("parsing time: %lf\n", (double)parse_time/1000000.0);
	return 0;
}



void print_nodes(NodeArray nodes, const char *text, const char *prefix){
	if (nodes.ptr == NULL)
		raise_error(text, "[TOKENIZING EROROR]", 0);
	if (nodes.ptr[0].type == Node_Error)
		raise_error(text, nodes.ptr[1].data.debug_text, nodes.ptr[0].pos);

	for (size_t n=1; n<nodes.size; n+=1){
		if (n >= 1000) break;
		Node it = nodes.ptr[n];
		Value *data = (Value *)(nodes.ptr + n + 1);
		NodeType t = it.type;
		if (t >= NodeTypeCount){
			fprintf(stderr, "WRONG TYPE : %i\n", t);
			exit(1);
		}
		if (t == Node_Terminator) break;
		printf("%s %10lu %s", prefix, n-1, NodeTypeNames[(size_t)t]);
		switch (t){
		case Node_String:
			n += datanode_count(it.size);
			printf(": \"");
			for (size_t i=0; i!=it.size; i+=1)
				putchar(((const char *)data)[i]);
			putchar('\"');
			break;
		case Node_Character:
			n += 1;
			printf(": \'%c\' - %d", data->rune, data->rune);
			break;
		case Node_Integer:
			n += 1;
			printf(": %li", data->i64);
			break;
		case Node_Unsigned:
			n += 1;
			printf(": %lu", data->u64);
			break;
		case Node_Float:
			n += 1;
			printf(": %f", data->f32);
			break;
		case Node_Double:
			n += 1;
			printf(": %f", data->f64);
			break;
		case Node_OpenScope:
		case Node_K_Break:
		case Node_K_Continue:
		case Node_K_Goto:
			if (it.index == UINT16_MAX){
				printf(": ---");
			} else{
				printf(": %u", it.index);
			}
			break;
		case Node_Identifier:
		case Node_Unresolved:
		case Node_GetField:
		case Node_EnumLiteral:
		case Node_Trait:
		case Node_Colon:
		case Node_Variable:
		case Node_Constant:
		case Node_OptionalConstant:
			if (it.size == UINT16_MAX){ 
				printf(": ---");
			} else{
				n += datanode_count(it.size);
				printf(": ");
				for (size_t i=0; i!=it.size; i+=1)
					putchar(((const char *)data)[i]);
			}
			break;
		case Node_S_Call:
			printf(": %u %s", (unsigned)it.count, it.broadcast?"~ Broadcasted":"");
			break;
		case Node_S_GetPointer:
		case Node_S_Index:
		case Node_S_StructIndex:
		case Node_S_Initialize:
		case Node_S_ArrayClass:
		case Node_S_StructLiteral:
		case Node_S_Procedure:
		case Node_ArrayClass:
		case Node_ProcedureLiteral:
		case Node_ProcedureClass:
			printf(": %u", (unsigned)it.count);
			break;
		
		case Node_Skip:
			printf(": %u", (unsigned)it.pos);
			break;

		default: 
			if (it.broadcast && (
				(Node_Concatenate <= it.type && it.type <= Node_Reinterpret) ||
				(it.type == Node_BitNot || it.type == Node_LogicNot || it.type == Node_Minus)
			)) printf(" ~ Broadcasted");
			break;
		}
		putchar('\n');
	}
}

