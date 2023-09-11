#pragma once

#include "utils.h"
#include "unicode.h"
#include "files.h"

#include <stdio.h>
#include <stdlib.h>

#include "classes.h"


static const char *KeywordNames[] = {
// KEYWORDS
	"if",       "else",      "while",       "for",
	"defer",    "return",
	"break",    "continue",  "goto",
	"assert",
	"do",
	
	"load",     "import",    "export",

	"struct",   "enum",      "bitfield",    "expr",

// DIRECTIVES
	"if",       "for",       "try",      // control statements
	"params",                            // module parameters declaration
	"asm",                               // inlineassembly
	"load",
	"assert",                            // assertion
	"at",                                // filed modifiers
	"size",     "aligning",  "const",    // data class information
	"fcount",   "fnames",    "cfnames",  // field information
	"dsalloc",  "cdsalloc",  "bssalloc", // data segment allocators
	"hot",      "cold",                  // compiler hints
	"run",      "pull",                  // expression modifiers
	"inl",      "noinl",     "callc",    // procedure flags

// BASIC CONSTANTS
	"Class",  "Null",   "True", "False",

// BASIC COMPILE TIME INFORMATION
	"class",   "isconst",
	"bytes",   "aligning",       "fcount",

// BASIC FUNCTIONS
	"abs",
	"min", "max",  "clamp",
};

static uint64_t KeywordNamesU64[SIZE(KeywordNames)];

static void init_keyword_names(){
	for (size_t i=0; i!=SIZE(KeywordNames); i+=1){
		for (size_t j=0; j!=sizeof(uint64_t); j+=1){
			if (KeywordNames[i][j] == '\0') break;
			KeywordNamesU64[i] |= (uint64_t)KeywordNames[i][j] << j*8;
		}
	}
}

static bool is_valid_name_char(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';
}

static bool is_valid_first_name_char(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';
}

static bool is_number(char c){
	return (uint8_t)(c-'0') < 10;
}

static bool is_whitespace(char c){
	return c==' ' || c=='\n' || c=='\t' || c=='\v';
}

static bool is_keyword_statement(enum NodeType type){
	return Node_K_If <= type && type <= Node_K_Expr;
}






// PARSING HELPERS
static const size_t MaxNameLengthNodes = 4;

size_t get_identifier_name(Node *dest, const char **iter) {
	const char *it = *iter;
	size_t count = 0;
	if (is_valid_first_name_char(it[count])){
		count += 1;
		while (is_valid_name_char(it[count])) count += 1;
	}
	size_t required_node_count = datanode_count(count);
	if (required_node_count > MaxNameLengthNodes) return SIZE_MAX;
	if (count != 0){
		dest[required_node_count].data.u64 = 0;
		memcpy(dest+1, *iter, count);
		*iter = it + count;
	}
	dest[0].size = count;
	return required_node_count;
}



uint32_t parse_character(const char **iter){
	const char *it = *iter;
	uint32_t c;

ParseCharacter:{
		c = *it;
		if (c != '\\') return utf8_parse(iter);
	
		int utf_base = 10;
	
		it += 1;
		c = *it;
		switch (c){
		case '\'':
		case '\"':
		case '\\':
			break;
		case 'x':
			utf_base = 16;
			goto ParseUtf8;
		case 'o':
			utf_base = 8;
			goto ParseUtf8;
		ParseUtf8:
			it += 1;
			[[fallthrough]];
		case '0' ... '9':{
			const char *old_iter = it;
			c = strtol(old_iter, (char **)&it, utf_base);
			if (old_iter == it) return UINT32_MAX;
			it -= 1;
			break;
		}
		case 'a':
			c = '\a';
			break;
		case 'b':
			c = '\b';
			break;
		case 'e':
			c = '\033';
			break;
		case 'f':
			c = '\f';
			break;
		case 'r':
			c = '\r';
			break;
		case 'n':
			c = '\n';
			break;
		case 't':
			c = '\t';
			break;
		case 'v':
			c = '\v';
			break;
		case '\n':
		case '\v':
		case '_':
			it += 1;
			goto ParseCharacter;
		default:
			return UINT32_MAX;
		}
	}

	it += 1;
	*iter = it;
	return c;
}



// TO DO: add separator syntax
bool get_number_token_from_iterator(
	Node *out_iter,
	const char **inp_iter,
	uint32_t pos
){
	out_iter[0].type = Node_Unsigned;
	out_iter[0].pos = pos;

	const char *prev_iter = *inp_iter;
	int int_base = 10;

	char c = **inp_iter;
	if (c == '.') goto ParseFloat;

	if (c == '0'){
		switch (**inp_iter){
		case 'x':
			int_base = 16;
			*inp_iter += 2;
			break;
		case 'o':
			int_base = 8;
			*inp_iter += 2;
			break;
		case 'b':
			int_base = 2;
			*inp_iter += 2;
			break;
		default: break;
		}
	}

	out_iter[1].data.u64 = (uint64_t)strtoul(*inp_iter, (char **)inp_iter, int_base);
	
	if (**inp_iter == 'f') goto ParseFloat;
	
	if (**inp_iter == '.'){
		if (int_base != 10) return true;
		if (*(*inp_iter+1) == '.') return false;
	ParseFloat:
		out_iter[0].type = Node_Double;
		out_iter[1].data.f64 = strtod(prev_iter, (char**)inp_iter);
		if (**inp_iter == 'f'){
			*inp_iter += 1;
			out_iter[0].type = Node_Float;
			out_iter[1].data.f32 = (float)out_iter[1].data.f64;
		}
	}
	return false;
}






// DEBUG INFORMATION HELPERS
static void print_codeline(const char *text, size_t position){
	size_t row = 0;
	size_t col = 0;
	size_t row_position = 0;

	for (size_t i=0; i!=position; ++i){
		col += 1;
		if (text[i]=='\n' || text[i]=='\v'){
			row_position += col;
			row += 1;
			col = 0;
		}
	}
	fprintf(stderr, " -> row: %lu, column: %lu\n", row, col);

	putchar('>');
	putchar('\n');
	
	putchar('>');
	putchar(' ');
	putchar(' ');
	for (size_t i=row_position;; ++i){
		char c = text[i];
		if (c=='\0' || c=='\n' || c=='\v') break;
		putchar(c);
	}
	putchar('\n');

	putchar('>');
	putchar(' ');
	putchar(' ');
	for (size_t i=0; i!=col; ++i) putchar(' ');
	putchar('^');
	putchar('\n');
	putchar('\n');
}


void raise_error(const char *text, const char *msg, uint32_t pos){
	fprintf(stderr, "error: \"%s\"", msg);
	print_codeline(text, pos);
	exit(1);
}





// TOKENIZATION PROCEDURE
// array that stores names
// takes null terminated string
NodeArray make_tokens(const char *input){
	init_keyword_names();

	const char *text_begin = input;
	NodeArray res = {NULL, 0, 0};
	NodeArray_push(&res)->data.u64 = 0;

#define RETURN_ERROR(msg, position) {   \
	res.ptr[0].type = Node_Error;       \
	res.ptr[0].pos = position;          \
	res.ptr[1].data.debug_text = msg;   \
	res.size = UINT32_MAX;               \
	return res;                          \
}

	uint32_t scope_openings[32];
	size_t scope_openings_size = 0;

	Node curr;
	size_t iprev = 0;
	size_t position = 0;
	
	for (;;){
		curr.data.u64 = 0;
		const char *prev_input = input;

		switch (*input){
		case '=': input += 1;
			if (*input == '='){
				input += 1;
				curr.type = Node_Equal;
				goto AddToken;
			}
			if (*input == '>'){
				input += 1;
				curr.type = Node_ProcedureLiteral;
				goto AddToken;
			}
			if (!is_whitespace(*(input-2))){
				enum NodeType prev_type = res.ptr[iprev].type;
				if (Node_Concatenate <= prev_type && prev_type <= Node_RotaryRightShift){
					res.ptr[iprev].type = prev_type + (Node_AddAssign - Node_Add);
					goto NextToken;
				}
			}
			curr.type = Node_Assign;
			goto AddToken;

		case '+': input += 1;
			if (*input == '%'){
				curr.type = Node_ModularAdd;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node_FullAdd;
				input += 1;
			} else{
				curr.type = Node_Add;
			}
			goto AddToken;


		case '-': input += 1;
			if (*input == '>'){
				input += 1;
				curr.type = Node_ProcedureClass;
			} else if (*input == '%'){
				input += 1;
				curr.type = Node_ModularSubtract;
			} else if (*input == '&'){
				input += 1;
				curr.type = Node_FullSubtract;
			} else{
				curr.type = Node_Subtract;
			}
			goto AddToken;


		case '*': input += 1;
			if (*input == '%'){
				curr.type = Node_ModularMultiply;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node_FullMultiply;
				input += 1;
			} else if (*input == '*'){
				curr.type = Node_Power;
				input += 1;
			} else{
				curr.type = Node_Multiply;
			}
			goto AddToken;


		case '/': input += 1;
			if (*input == '/'){
				input += 1;
				while (*input!='\0' && *input!='\n') input += 1;
				goto SkipToken;
			}
			if (*input == '*'){
				size_t depth = 1;
				for (;;){
					input += 1;
					[[unlikely]] if (*input == '\0')
						RETURN_ERROR("unfinished comment", position);
					
					if (*input=='*' && *(input+1)=='/'){
						input += 1;
						depth -= 1;
						if (depth == 0){
							input += 1;
							goto SkipToken;
						}
					} else if (input[0]=='/' && input[1]=='*'){
						input += 1;
						depth += 1;
					}
				}
			}
			if (*input == '%'){
				curr.type = Node_ModularDivide;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node_FullDivide;
				input += 1;
			} else{
				curr.type = Node_Divide;
			}
			goto AddToken;


		case '%': input += 1;
			curr.type = Node_Modulo;
			goto AddToken;


		case '|': input += 1;
			if (*input == '|'){
				input += 1;
				curr.type = Node_LogicOr;
			} else if (*input == '>'){
				input += 1;
				curr.type = Node_Pipe;
			} else{
				curr.type = Node_BitOr;
			}
			goto AddToken;


		case '&': input += 1;
			if (*input == '&'){
				input += 1;
				curr.type = Node_LogicAnd;
			} else
				curr.type = Node_BitAnd;
			goto AddToken;


		case '~': input += 1;
			if (*input == '%' && *(input+1) == '~'){
				input += 2;
				curr.type = Node_Reinterpret;
			} else{
				curr.type = Node_Cast;
			}
			goto AddToken;


		case '<': input += 1;
			if (*input == '='){
				input += 1;
				curr.type = Node_LesserEqual;
			} else if (*input == '>'){
				input += 1;
				curr.type = Node_Concatenate;
			} else if (*input == '<'){
				input += 1;
				if (*input == '%'){
					input += 1;
					curr.type = Node_RotaryLeftShift;
				} else{
					curr.type = Node_LeftShift;
				}
			} else{
				curr.type = Node_Lesser;
			}
			goto AddToken;


		case '>': input += 1;
			if (*input == '='){
				input += 1;
				curr.type = Node_GreaterEqual;
			} else if (*input == '>'){
				input += 1;
				if (*input == '%'){
					input += 1;
					curr.type = Node_RotaryRightShift;
				} else{
					curr.type = Node_RightShift;
				}
			} else if (*input == '<'){
				input += 1;
				curr.type = Node_CrossProduct;
			} else{
				curr.type = Node_Greater;
			}
			goto AddToken;

		case '!': input += 1;
			if (*input == '='){
				curr.type = Node_NotEqual;
				input += 1;
			} else if (*input == '|'){
				curr.type = Node_LogicNor;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node_LogicNand;
				input += 1;
			} else if (*input == '*'){
				curr.type = Node_Pointer;
				curr.owning = true;
				input += 1;
			} else if (*input == '@' && *(input+1) == '='){
				curr.type = Node_DoesntContain;
				input += 2;
			} else{
				curr.type = Node_LogicNot;
			}
			goto AddToken;


		case '(': input += 1;
			// for later usege in determining of constant field invocation
			if (res.ptr[iprev].type == Node_Identifier) curr.flags |= NodeFlag_DirectIdentifier;
			curr.type = Node_OpenPar;
			goto AddToken;
		

		case ')': input += 1;
			curr.type = Node_ClosePar;
			goto AddToken;
		
		
		case '{': input += 1;
			scope_openings[scope_openings_size] = res.size;
			scope_openings_size += 1;
			curr.type = Node_OpenBrace;
			curr.index = UINT16_MAX;
			goto AddToken;
		
		
		case '}': input += 1;
			[[unlikely]] if (scope_openings_size == 0)
				RETURN_ERROR("too many closing braces", position);
			scope_openings_size -= 1;
			curr.type = Node_CloseBrace;
			goto AddToken;
		
		
		case '[': input += 1;
			if (*input == ']'){
				input += 1;
				curr.type = Node_Span;
			} else if (*input == '!' && *(input+1) == ']'){
				input += 2;
				curr.type = Node_Span;
				curr.owning = true;
			} else{
				curr.type = Node_OpenBracket;
			}
			goto AddToken;
		
		
		case ']': input += 1;
			curr.type = Node_CloseBracket;
			goto AddToken;


		case '^': input += 1;
			if (*input == '|'){
				curr.type = Node_BitNor;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node_BitNand;
				input += 1;
			} else{
				curr.type = Node_BitXor;
			}
			goto AddToken;
		

		case '@': input += 1;
			if (*input == '='){
				curr.type = Node_Contains;
				input += 1;
			} else{
				curr.type = Node_Broadcast;
			}
			goto AddToken;

		case '#': input += 1;
			curr.type = Node_Pound;
			goto AddToken;


		case '?': input += 1;
			// TODO: add ?^Name syntax for generic parameter wiht automatic selection of
			// how it is passed.
			if (is_valid_first_name_char(*input)){
				NodeArray_reserve(&res, res.size+1+MaxNameLengthNodes);
				size_t s = get_identifier_name(res.ptr+res.size, &input);
				if (s == SIZE_MAX) RETURN_ERROR("name is too long", position);
				res.ptr[res.size].type = Node_Unresolved;
				res.ptr[res.size].pos = position;
				iprev = res.size;
				res.size += 1 + s;
				goto NextToken;
			}
			curr.type = Node_Conditional;
			goto AddToken;


		case ':':{
			input += 1;
			if (*input == ':'){
				curr.type = Node_Constant;
				input += 1;
				if (*input == ':'){
					curr.type = Node_OptionalConstant;
					input += 1;
				}
			} else if (*input == '='){
				curr.type = Node_Variable;
				input += 1;
			} else{
				curr.type = Node_Colon;
			}

			if (res.ptr[iprev].type == Node_Identifier){
				res.ptr[iprev].type = curr.type;
				goto NextToken;
			}
			curr.size = UINT16_MAX;
			goto AddToken;
		}
		

		case ',': input += 1;
			curr.type = Node_Comma;
			goto AddToken;


		case '.': input += 1;
			if (*input == '.'){
				input += 1;
				if (*input == '.'){
					input += 1;
					curr.type = Node_Splat;
				} else{
					curr.type = Node_Range;
				}
			} else if (*input == '('){
				input += 1;
				curr.type = Node_GetProcedure;
			} else if (*input == '['){
				input += 1;
				curr.type = Node_GetFieldIndexed;
			} else if (is_number(*input)){
				input -= 1;
				if (get_number_token_from_iterator(res.ptr+res.size, &input, position))
					RETURN_ERROR("invalid floating point literal", position);
				iprev = res.size;
				res.size += 2;
				goto NextToken;
			} else{
				curr.type = Node_GetField;
				if (*input == '>'){
					input += 1;
					curr.type = Node_Trait;
				}
				NodeArray_reserve(&res, res.size+1+MaxNameLengthNodes);
				res.ptr[res.size] = curr;
				size_t s = get_identifier_name(res.ptr+res.size, &input);
				if (s == 0) RETURN_ERROR("missing field name", position);
				if (s == SIZE_MAX) RETURN_ERROR("field name is too long", position);
				iprev = res.size;
				res.size += 1 + s;
				goto NextToken;
			}
			goto AddToken;


		case ';':
			input += 1;
			curr.type = Node_Semicolon;
			if (scope_openings_size != 0)
				res.ptr[(size_t)scope_openings[scope_openings_size-1]].type = Node_OpenScope;
			goto AddToken;
		
		
		case '\'':{
			input += 1;
			if (*input == '\''){
				curr.type = Node_Dereference;
				goto AddToken;
			}

			const char *old_input = input;
			uint32_t c = parse_character(&input);

			if (c == UINT32_MAX || *input != '\''){
				input = old_input;
				curr.type = Node_Dereference;
				goto AddToken;
			}
			input += 1;

			iprev = res.size;
			curr.type = Node_Character;
			*NodeArray_push(&res) = curr;
			NodeArray_push(&res)->data.rune = c;
			goto NextToken;
		}
		
		case '\"':{
			input += 1;
			size_t count = 0;
			while (*input != '\"'){
				uint32_t c = parse_character(&input);
				if (c == UINT32_MAX)
					RETURN_ERROR("invalid string literal", input-text_begin);
				NodeArray_reserve(&res, res.size+count/4);
				count += utf8_write((char *)(res.ptr+res.size+1)+count, c);
			}
			input += 1;
			curr.type = Node_String;
			curr.size = count;
			size_t node_count = datanode_count(count);
			if (count != 0){
				size_t n_free = node_count*sizeof(Node) - count;
				memset((char *)(res.ptr+res.size+1)+count, 0, n_free);
			}
			res.ptr[res.size] = curr;

			if (*input == '{'){
				input += 1;
				RETURN_ERROR("[UNIMPLEMENTED] ", position);
				/*curr.type = Node_OpenScope;
				curr.index = add_name(slice(string, 4, string.size), scope_names, al);
				scope_openings[scope_openings_size] = res.size;
				scope_openings_size += 1;*/
			}
			enum NodeType prev_type = res.ptr[iprev].type;
			if (Node_K_Break <= prev_type && prev_type <= Node_K_Goto){
				/*res.ptr[res.size-1].index = add_name(
					Span<const char>{(const char *)string.ptr+4, string.size-4},
					scope_names, al
				);
				goto NextToken;*/
				RETURN_ERROR("[UNIMPLEMENTED] ", position);
			}
	
			iprev = res.size;
			res.size += 1 + node_count;
			goto NextToken;
		}


		case ' ':
		case '\t':
		case '\n':
			input += 1;
			goto SkipToken;
		
		case '\0':
			curr.type = Node_Terminator;
			curr.pos = position; 
			*NodeArray_push(&res) = curr;
			return res;

		case '_':
			if (!is_valid_name_char(*(input+1))){
				input += 1;
				curr.type = Node_Identifier;
				curr.size = UINT16_MAX - 1;
			}
			[[fallthrough]];

		default:{
			NodeArray_reserve(&res, res.size+1+MaxNameLengthNodes);
			size_t s = get_identifier_name(res.ptr+res.size, &input);
			if (s == SIZE_MAX) RETURN_ERROR("name is too long", position);

			if (s != 0){
				if (s == 1){
					uint64_t text = res.ptr[res.size+1].data.u64;
					for (size_t i=(size_t)Node_K_If; i<=(size_t)Node_K_Expr; i+=1)
						if (text == KeywordNamesU64[(size_t)i]){
							curr.type = (NodeType)i;
							curr.index = UINT16_MAX;    // this is for break and continue
							goto AddToken;
						}
				}
				res.ptr[res.size].type =  Node_Identifier;
#ifdef CARE_ABOUT_VALGRIND
				res.ptr[res.size].flags = 0;
#endif
				iprev = res.size;
				res.size += 1 + s;
				goto NextToken;
			}
			input -= s;
			if (is_number(*input)){
				if (get_number_token_from_iterator(res.ptr+res.size, &input, position))
					RETURN_ERROR("invalud number literal", position);
#ifdef CARE_ABOUT_VALGRIND
				res.ptr[res.size].flags = 0;
#endif
				iprev = res.size;
				res.size += 2;
				goto NextToken;
			}
			RETURN_ERROR("unrecognized token", position);
		}}

	AddToken:
		iprev = res.size;
		*NodeArray_push(&res) = curr;
	NextToken:
		res.ptr[iprev].pos = position;
	SkipToken:
		position += input - prev_input;
	}
#undef RETURN_ERROR
}



