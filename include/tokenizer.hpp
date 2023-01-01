#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "SPL/Arrays.hpp"
#include "SPL/Allocators.hpp"




constexpr Span<const char> KeywordNames[] = {
// KEYWORDS
	slice("if"), slice("else"), slice("while"), slice("for"),
	slice("defer"), slice("return"), slice("break"), slice("continue"),
	slice("do"), slice("try"),
	slice("assert"),
	
	slice("load"), slice("import"), slice("export"),

	slice("proc"), slice("struct"), slice("enum"), slice("bitfield"), slice("expr"),

	slice("asm"),
	slice("at"), slice("const"),

// BASIC CONSTANTS
	slice("Class"), slice("Null"), slice("True"), slice("False"),

// BASIC COMPILE TIME INFORMATION
	slice("class"), slice("isconst"),
	slice("bytes"), slice("alignment"), slice("fieldcount"), 

// BASIC FUNCTIONS
	slice("abs"),
	slice("min"), slice("max"), slice("clamp"),
};


// TOKEN CLASS
struct Node{
	enum Type : int16_t{
	// KEYWORDS
		If, Else, While, For,
		Defer, Return, Break, Continue,
		Do, Try,
		Assert,
	
		Load, Import, Export,
	
		Proc, Struct, Enum, Bitfield, Expr,

		Asm,
		At, Const,

	// BASIC CONSTANTS
		Class, Null, True, False,
	
	// BASIC COMPILE TIME INFORMATION
		GetClass, GetIsconst,
		GetBytes, GetAlignment, GetFieldcount,
	
	// CLOSING SYMBOLS
		ClosePar, CloseBracket, CloseBrace, CloseScope,
	
	// OPENING SYMBOLS
		OpenPar, OpenBracket, OpenBrace, OpenScope, OpenArrayType,
		
	// PREFIX OPERATNS
		Pointer, BitNot,
		Span, View,
		Minus, LogicNot,
		UnresolvedValue,
		ArrayType,
	
	// POSTFIX OPERATIONS
		Dereference,
		GetField,
		GetFieldIndexed,
		Call, BroadcastCall,
		Initialize,
		GetProcedure, IndexAccess,
		Conditional,
		Trait,
	
	// SEPARATORS
		Comma, Terminator,
		Colon, DoubleColon, TripleColon,
	
	// BINARY OPERATIONS
		Assign, AssignBytes, DestructuredAssign,
		
		ConcatenateAssign,
		ModuloAssign,
		AddAssign, SubtractAssign, ModularAddAssign, ModularSubtractAssign, 
		MultiplyAssign, DivideAssign, ModularMultiplyAssign, ModularDivideAssign,
		PowerAssign,
		BitOrAssign, BitNorAssign,
		BitAndAssign, BitNandAssign,
		BitXorAssign,
		LeftShiftAssign, RightShiftAssign,
	
		Concatenate,
		Modulo,
		Add, Subtract, ModularAdd, ModularSubtract, 
		Multiply, Divide, ModularMultiply, ModularDivide,
		Power,
		BitOr, BitNor,
		BitAnd, BitNand,
		BitXor,
		LeftShift, RightShift,
		
		Range,
		Pipe,
		LogicOr, LogicNor,
		LogicAnd, LogicNand,
		Equal, NotEqual, Lesser, Greater, LesserEqual, GreaterEqual,
		Contains, DoesntContain,
		FullAdd, FullSubtract,
		FullMultiply, FullDivide,
		CrossProduct,
		Cast, Reinterpret,
	
	// LITERALS
		Identifier,
		Integer, Unsigned, Double, Float,
		Character, String,
		EnumLiteral,
		ExprClass, Lambda,
		UnresolvedType,
		ArrayLiteral, StructLiteral,
	
	// MODIFIERS
		Pound, Broadcast,
	
	// DECLARATIONS
		Variable, Constant, Destructure,
	};

// DATA MEMBERS
		Type type;
		union{
			uint16_t index;
			uint16_t count;
			char character;
			bool owning;
			bool broadcast;
		};
		uint32_t pos;
};


bool is_valid_name_char(char c) noexcept{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_';
}

bool is_valid_first_name_char(char c) noexcept{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_';
}

bool is_number(char c) noexcept{ return (uint8_t)(c-'0') < 10; }

bool is_whitespace(char c) noexcept{ return c==' ' || c=='\n' || c=='\t' || c=='\v'; }

bool is_keyword_statement(Node::Type type) noexcept{
	return (
		(uint32_t)type >= (uint32_t)Node::If && (uint32_t)type <= (uint32_t)Node::Const
	);
}


union NodeData{
	char *string;
	int64_t sint;
	uint64_t uint;
	float float32;
	double float64;
	uint64_t unicode_char;
};


// TO DO: add utf-8 support
uint32_t get_char_from_iterator(const char **inp_iter) noexcept{
	uint32_t c = **inp_iter;
	*inp_iter += 1;

	if (c=='\n' || c=='\0') return (uint32_t)-1; // -1 means error

	if (c == '\\')
		switch (**inp_iter){
		case '\'':
		case '\"':
		case '\\':
			c = **inp_iter;
			*inp_iter += 1;
			break;
		case '0' ... '9':
			c = strtol(*inp_iter, (char **)inp_iter, 10);
			if (c > 127){
				// PLACEHOLDER FOR: translate to utf-8
				return 0;
			}
			break;
		case 'n': ++*inp_iter;
			return '\n';
		case 't': ++*inp_iter;
			return '\t';
		case 'v': ++*inp_iter;
			return '\v';
		case '\n':
		case '\v': ++*inp_iter;
			return (uint32_t)-2; // -2 means that it should be skipped
		}
	return c;
}

// TO DO: save number of the required bits for integer into the u16 field
// TO DO: add separator syntax from C++
template<class A>
Node get_number_token_from_iterator(
	const char **inp_iter,
	uint32_t pos,
	Array<NodeData> &data,
	A &al
) noexcept{
	Node node = {.type = Node::Integer, .index=data.size, .pos = pos};

	const char *prev_iter = *inp_iter;

	char c = **inp_iter;
	if (c == '.'){
		push(data, al);
		goto ParseFloat;
	}

	if (c == '0'){
		*inp_iter += 1;
		switch (**inp_iter){
		case 'x':
			push_value(data, NodeData{
				.uint = (uint64_t)strtoul(*inp_iter+1, (char **)inp_iter, 16)
			}, al);
			goto ReturnInt;
		
		case 'o':
			push_value(data, NodeData{
				.uint = (uint64_t)strtoul(*inp_iter+1, (char **)inp_iter, 8)
			}, al);
			goto ReturnInt;

		case 'b':
			push_value(data, NodeData{
				.uint = (uint64_t)strtoul(*inp_iter+1, (char **)inp_iter, 2)
			}, al);
			goto ReturnInt;

		default: break;
		}
	}
	{
		push_value(data, NodeData{
			.uint = (uint64_t)strtoul(*inp_iter, (char **)inp_iter, 10)
		}, al);
		
		if (**inp_iter == '.'){
			if (*(*inp_iter+1) == '.') goto JustReturn;
		ParseFloat:
			node.type = Node::Double;
			data[data.size-1].float64 = strtod(prev_iter, (char**)inp_iter);
		}
	}

ReturnInt:
	if (**inp_iter == 'u'){
		*inp_iter += 1;
		node.type = Node::Unsigned;
	} else if (**inp_iter == 'f'){
		*inp_iter += 1;
		data[data.size-1].float32 = (
			node.type==Node::Integer ?
			(float)data[data.size-1].sint : (float)data[data.size-1].float64
		);
		node.type = Node::Float;
	}
JustReturn:
	return node;
}




char *code_text;

void print_codeline(const char *text, size_t position) noexcept{
	size_t row = 0;
	size_t col = 0;
	size_t row_position = 0;

	for (size_t i=0; i!=position; ++i){
		++col;
		if (text[i]=='\n' || text[i]=='\v'){
			row_position += col;
			++row;
			col = 0;
		}
	}
	printf(" -> row: %lu, column: %lu\n", row, col);

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


void raise_error(const char *msg, uint32_t pos) noexcept{
	fprintf(stderr, "error: \"%s\"", msg);
	print_codeline(code_text, pos);
	exit(1);
}


Span<const char> get_identifier_name(const char **iter) noexcept{
	const char *it = *iter;
	Span<const char> res = {it, 0};

	if (is_valid_first_name_char(*it)){
		it += 1;
		while (is_valid_name_char(*it)) it += 1;
	}
	*iter = it;
	res.size = it - res.ptr;
	return res;
}

template<class A>
uint16_t add_name(Span<const char> name, Array<const char *> &names, A &al) noexcept{
	if (name.size == 1 && name[0] == '_') return UINT16_MAX;

	for (size_t i=0; i!=names.size; i+=1)
		if (name == Span<const char>{names[i]+1, *names[i]}) return i;

	Memblock name_data = alloc(al, name.size+1);
	name_data[0] = name.size;
	for (size_t i=0; i!=name.size; i+=1) name_data[1+i] = name[i];
	push_value(names, name_data.ptr, al);
	return names.size - 1;
}

struct Error{
	Span<const char> msg;
	uint32_t pos;
};

struct TokensResult{
	Array<Node> tokens;
	Array<const char *> names;
	Array<const char *> foreign_names;
	Array<NodeData> data;
	Error err;
};

// array that stores names
// takes null terminated string
template<class A>
TokensResult make_tokens(const char *input, A &al) noexcept{
	TokensResult res = {};
	push_value(res.tokens, Node{.type=Node::Null, .index=0, .pos=0}, al);
#define ERROR_RETURN(msg, pos) { res.err = {slice(msg), (pos)}; goto Return; }

	FiniteArray<uint32_t, 32> scope_openings;
	Array<const char *> scope_names = {};
	Node curr;
	curr.pos = 0;
	
	for (;;){
		curr.broadcast = false;
		curr.owning = false;
		const char *prev_input = input;

		switch (*input){
		case '=': input += 1;
			if (*input == '='){
				curr.type = Node::Equal;
				input += 1;
				goto AddToken;
			}
			if (!is_whitespace(*(input-2))){
				Node::Type prev = res.tokens[res.tokens.size-1].type;
				if (Node::Concatenate <= prev && prev <= Node::RightShift){
					res.tokens[res.tokens.size-1].type = (Node::Type)(
						(uint16_t)res.tokens[res.tokens.size-1].type +
						(Node::AddAssign - Node::Add)
					);
					goto Break;
				}
			}
			curr.type = Node::Assign;
			goto AddToken;


		case '+': input += 1;
			if (*input == '+'){
				curr.type = Node::Concatenate;
				input += 1;
			} else if (*input == '%'){
				curr.type = Node::ModularAdd;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::FullAdd;
				input += 1;
			} else{
				curr.type = Node::Add;
			}
			goto AddToken;


		case '-': input += 1;
			if (*input == '>'){
				ERROR_RETURN("Operator -> is not implemented.", curr.pos);
				goto Break;
			}
			if (*input == '%'){
				curr.type = Node::ModularSubtract;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::FullSubtract;
				input += 1;
			} else{
				curr.type = Node::Subtract;
			}
			goto AddToken;

// 1 + (2 + 3 * a) + f(1+3*a, 3*4,5, 1*2+3*z) * 3 + 1
		case '*': input += 1;
			if (*input == '%'){
				curr.type = Node::ModularMultiply;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::FullMultiply;
				input += 1;
			} else if (*input == '*'){
				curr.type = Node::Power;
				input += 1;
			} else{
				curr.type = Node::Multiply;
			}
			goto AddToken;


		case '/': input += 1;
			if (*input == '/'){
				input += 1;
				while (*input!='\0' && *input!='\n') input += 1;
				break;
			}
			if (*input == '*'){
				input += 1;
				size_t depth = 1;
				for (;;){
					if (*input == '\0') break;
					if (input[0]=='*' && input[1]=='/'){
						depth -= 1;
						input += 1;
						if (depth == 0) break;
					} else if (input[0]=='/' && input[1]=='*'){
						depth += 1;
						input += 1;
					}
					input += 1;
				}
				break;
			}
			if (*input == '%'){
				curr.type = Node::ModularDivide;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::FullDivide;
				input += 1;
			} else{
				curr.type = Node::Divide;
			}
			goto AddToken;


		case '%': input += 1;
			curr.type = Node::Modulo;
			goto AddToken;


		case '|': input += 1;
			if (*input == '|'){
				curr.type = Node::LogicOr;
				input += 1;
			} else if (*input == '>'){
				curr.type = Node::Pipe;
				input += 1;
			} else{
				curr.type = Node::BitOr;
			}
			goto AddToken;


		case '&': input += 1;
			if (*input == '&'){
				curr.type = Node::LogicAnd;
				input += 1;
			} else
				curr.type = Node::BitAnd;
			goto AddToken;


		case '~': input += 1;
			if (*input == '%' && *(input+1) == '~'){
				curr.type = Node::Reinterpret;
				input += 2;
			} else if (*input == '%' && *(input+1) == '='){
				curr.type = Node::AssignBytes;
				input += 2;
			} else{
				curr.type = Node::Cast;
			}
			goto AddToken;


		case '<': input += 1;
			if (*input == '='){
				curr.type = Node::LesserEqual;
				input += 1;
			} else if (*input == '>'){
				curr.type = Node::View;
				input += 1;
			} else if (*input == '!' && *(input+1) == '>'){
				curr.type = Node::View;
				curr.owning = true;
				input += 2;
			} else if (*input == '<'){
				curr.type = Node::LeftShift;
				input += 1;
			} else{
				curr.type = Node::Lesser;
			}
			goto AddToken;


		case '>': input += 1;
			if (*input == '='){
				curr.type = Node::GreaterEqual;
				input += 1;
			} else if (*input == '>'){
				curr.type = Node::RightShift;
				input += 1;
			} else if (*input == '<'){
				curr.type = Node::CrossProduct;
				input += 1;
			} else{
				curr.type = Node::Greater;
			}
			goto AddToken;


		case '!': input += 1;
			if (*input == '='){
				curr.type = Node::NotEqual;
				input += 1;
			} else if (*input == '|'){
				curr.type = Node::LogicNor;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::LogicNand;
				input += 1;
			} else if (*input == '*'){
				curr.type = Node::Multiply;
				curr.owning = true;
				input += 1;
			} else if (*input == '^'){
				curr.type = Node::BitNot;
				curr.owning = true;
				input += 1;
			} else if (*input == '@' && *(input+1) == '='){
				curr.type = Node::DoesntContain;
				input += 2;
			} else{
				curr.type = Node::LogicNot;
			}
			goto AddToken;


		case '(': input += 1;
			curr.type = Node::OpenPar;
			goto AddToken;
		

		case ')': input += 1;
			curr.type = Node::ClosePar;
			goto AddToken;
		
		
		case '{': input += 1;
			if (*input == '}'){
				input += 1;
				curr.type = Node::Initialize;
				curr.count = 0;
			} else{
				push_value(scope_openings, res.tokens.size);
				curr.type = Node::OpenBrace;
				curr.index = UINT16_MAX;
			}
			goto AddToken;
		
		
		case '}': input += 1;
			[[unlikely]] if (is_empty(scope_openings))
				ERROR_RETURN("too many closing braces", curr.pos);
			pop(scope_openings);
			curr.type = Node::CloseBrace;
			goto AddToken;
		
		
		case '[': input += 1;
			if (*input == ']'){
				input += 1;
				curr.type = Node::Span;
			} else if (*input == ':'){
				input += 1;
				curr.type = Node::OpenArrayType;
			} else if (*input == '!' && *(input+1) == ']'){
				curr.type = Node::Span;
				curr.owning = true;
			} else{
				curr.type = Node::OpenBracket;
			}
			goto AddToken;
		
		
		case ']': input += 1;
			curr.type = Node::CloseBracket;
			goto AddToken;


		case '^': input += 1;
			if (*input == '|'){
				curr.type = Node::BitNor;
				input += 1;
			} else if (*input == '&'){
				curr.type = Node::BitNand;
				input += 1;
			} else{
				curr.type = Node::BitXor;
			}
			goto AddToken;
		

		case '@': input += 1;
			if (*input == '='){
				curr.type = Node::Contains;
				input += 1;
			} else{
				curr.type = Node::Broadcast;
			}
			goto AddToken;

		case '#': input += 1;
			curr.type = Node::Pound;
			goto AddToken;


		case '?':{
			input += 1;
			Span<const char> text = get_identifier_name(&input);
			if (text.size == 0){
				curr.type = Node::Conditional;
				curr.index = UINT16_MAX;
			} else{
				curr.type = Node::UnresolvedType;
				curr.index = add_name(text, res.names, al);
			}
			goto AddToken;
		}


		case ':':{
			Node *prev = &res.tokens[res.tokens.size-1];

			input += 1;
			if (*input == ':'){
				curr.type = Node::DoubleColon;
				input += 1;
				if (*input == ':'){
					curr.type = Node::TripleColon;
					input += 1;
				} else if (*input == '='){
					curr.type = Node::Constant;
					input += 1;
				}
			} else if (*input == '='){
				curr.type = Node::Variable;
				input += 1;
			} else{
				if (prev->type == Node::UnresolvedType){
					prev->type = Node::UnresolvedValue;
					goto Break;
				}
				curr.type = Node::Colon;
			}

			if (prev->type == Node::Identifier){
				prev->type = curr.type;
				goto Break;
			}
			curr.index = UINT16_MAX;
			goto AddToken;
		}
		

		case ',': input += 1;
			curr.type = Node::Comma;
			goto AddToken;


		case '.': input += 1;
			if (*input == '.'){
				input += 1;
				curr.type = Node::Range;
			} else if (*input == '('){
				input += 1;
				curr.type = Node::GetProcedure;
			} else if (*input == '['){
				input += 1;
				curr.type = Node::GetFieldIndexed;
			} else if (is_number(*input)){
				--input;
				curr = get_number_token_from_iterator(&input, curr.pos, res.data, al);
			} else{
				if (*input == '#'){
					input += 1;
					curr.type = Node::Trait;
				} else{
					curr.type = Node::GetField;
				}
				Span<const char> name = get_identifier_name(&input);
				[[unlikely]] if (name.size == 0) ERROR_RETURN("missing field name", curr.pos);
				curr.index = add_name(name, res.foreign_names, al);
			}
			goto AddToken;


		case ';':
			if (!is_empty(scope_openings))
				res.tokens[(size_t)scope_openings[scope_openings.size-1]].type = Node::OpenScope;
			input += 1;
			curr.type = Node::Terminator;
			goto AddToken;
		
		
		case '\'':{
				input += 1;
				const char *input_backup = input;
				uint32_t c = get_char_from_iterator(&input);
				if (*input != '\''){
					curr.type = Node::Dereference;
					input = input_backup;
					goto AddToken;
				}
	// TO DO: add unicode support
				curr.type = Node::Character;
				curr.character = c;

				input += 1;
				goto AddToken;
			}
		
	// TO DO: add raw string literals
	// TO DO: add utf-8 support
		case '\"':{
			input += 1;
			Array<char> string;
			resize(string, 4, al);
			while (*input != '\"'){
				[[unlikely]] if (*input == '\0') ERROR_RETURN("unfinished string literal", curr.pos);

				uint32_t c = get_char_from_iterator(&input);
				[[unlikely]] if (c == (uint32_t)-1)
					ERROR_RETURN("invalid character at string literal", curr.pos);
				
				if (c == (uint32_t)-2) continue; // skipping newlines 
				push_value(string, c, al);
			}
			input += 1;
			*(uint32_t *)string.ptr = string.size - 4;

			if (*input == '{'){
				input += 1;
				curr.type = Node::OpenScope;
				curr.index = add_name(slice(string, 4, string.size), scope_names, al);
				push_value(scope_openings, res.tokens.size);
				deinit(string, al);
			} else{
				Node::Type last_node_type = res.tokens[res.tokens.size-1].type;
				if (last_node_type == Node::Break || last_node_type == Node::Continue){
					res.tokens[res.tokens.size-1].index = add_name(
						Span<const char>{(const char *)string.ptr+4, string.size-4},
						scope_names, al
					);
					goto Break;
				} else{
					curr.type = Node::String;
					curr.index = res.data.size;
					push_value(res.data, NodeData{.string=(char *)string.ptr}, al);
				}
			}
			goto AddToken;
		}

		case ' ':
		case '\t':
		case '\n':
			input += 1;
			goto Break;

		default:{
			Span<const char> text = get_identifier_name(&input);
		
			if (text.size != 0){
				for (size_t i=(size_t)Node::If; i<=(size_t)Node::Const; i+=1)
					if (text == KeywordNames[(size_t)i]){
						curr.type = (Node::Type)i;
						curr.index = UINT16_MAX;    // this is for break and continue
						goto AddToken;
					}

				[[unlikely]] if (text.size > 32) ERROR_RETURN("name is too long", curr.pos);
				
				curr.type = Node::Identifier;
				curr.index = add_name(text, res.names, al);
				goto AddToken;
			}
			input -= text.size;
			if (is_number(*input)){
				curr = get_number_token_from_iterator(&input, curr.pos, res.data, al);
				goto AddToken;
			}
			curr.type = Node::Null;
			push_value(res.tokens, curr, al);
			goto Return;
		}}
	AddToken:
		push_value(res.tokens, curr, al);
	Break:
		curr.pos += input - prev_input;
	}
#undef ERROR_RETURN
Return:
	deinit(scope_names, al);
	return res;
}



