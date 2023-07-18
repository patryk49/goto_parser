#pragma once
#include "tokenizer.h"

#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"


typedef struct ParseResult{
	NodeArray nodes;
	int *conditions;
	int procedure_body;
} ParseResult;


NodeArray parse_module(NodeArray tokens){
	#define RETURN_ERROR(msg, position) {   \
		tokens.ptr[0].type = Node_Error;     \
		tokens.ptr[0].pos = position;        \
		tokens.ptr[1].data.debug_text = msg; \
		tokens.size = UINT32_MAX;            \
		return tokens;                       \
	}
	ModulePool pool = {0};

	Node opers[256];
	size_t opers_size = 1;
	opers[0].data.u64 = 0;
	opers[0].type = Node_Terminator;

	Node scopes[64];
	size_t scopes_size = 1;
	scopes[0].type = Node_Terminator;
	
	Node *res_it = tokens.ptr+1;
	Node *it = tokens.ptr + 1;
	
	// IN PROGRESS (right now parse only expressions)
	goto ExpectValue;

	GlobalLevel:{
		Node curr = *it;
		it += 1;

		switch (curr.type){
		case Node_D_If:
			assert(scopes_size != SIZE(scopes));
			scopes[scopes_size] = curr; scopes_size += 1;
			goto ExpectValue;

		case Node_Splat:
			break;
	
		case Node_D_Assert:
			break;	
		[[unlikely]] default:
			RETURN_ERROR("this statement is not avalible at module level", curr.pos);
		}
	}
		
	StatementLevel:{
		res_it->type = Node_Terminator;
		return tokens;
		
		Node curr = *it;
		it += 1;

		switch (curr.type){
		case Node_CloseBrace:
		
		case Node_K_Return:
		case Node_K_Defer:

		case Node_K_If:
		case Node_K_While:
		case Node_K_For:
		
		case Node_K_Break:
		case Node_K_Continue:
		case Node_K_Goto:

		case Node_K_Assert:
		
		default:
			goto ExpectValue;
		}
	}

	ExpectValue:{
		// Handle literals, parenthesis and prefix operators
		Node curr = *it;
		it += 1;
		switch (curr.type){
		case Node_K_Do:          // Do local scope
			[[unlikely]] if (it->type != Node_OpenScope && it->type != Node_OpenBrace)
				RETURN_ERROR("do keyword must be followed by opening brace", curr.pos);
			it += 1;
			*res_it = curr;
			res_it += 1;
			curr.type = Node_DoBlock;
			goto SimpleOpeningSymbol;

		case Node_OpenBrace:     // Struct Literal
			if (it->type == Node_CloseBrace){
				it += 1;
				curr.type = Node_StructLiteral;
				curr.count = 0;
				goto SimpleLiteral;
			}
			goto SimpleOpeningSymbol;

		case Node_OpenPar:       // Precedence Modifier
			// handle procedure literals that take no arguments
			if (it->type == Node_ClosePar){
				it += 1;
				[[unlikely]] if (
					it->type != Node_ProcedureClass &&
					it->type != Node_ProcedureLiteral &&
					it->type != Node_OpenScope
				) RETURN_ERROR(
					"empty parenthesis must be followed by arrow symbol"
					"to denote procedure class or literal",
					curr.pos
				);
				if (it->type == Node_OpenScope){
					scopes[scopes_size].type = Node_ProcedureBody;
					scopes[scopes_size].pos = res_it - tokens.ptr;
					scopes_size += 1;
					*res_it = curr;
					res_it += 1;
					curr.type = Node_ProcedureBody;
				} else{
					curr.type = it->type;
				}
				it += 1;
				curr.count = 0;
				goto SimplePrefixOperator;
			}
			goto SimpleOpeningSymbol;

		case Node_OpenBracket:    // Array Literal
		SimpleOpeningSymbol:
			curr.count = 1;
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1; 
			assert(scopes_size != SIZE(scopes));
			scopes[scopes_size] = curr; scopes_size += 1;
			goto ExpectValue;

		case Node_Add:
			curr.type = Node_Plus;
			goto SimplePrefixOperator;
		case Node_Subtract:
			curr.type = Node_Minus;
			goto SimplePrefixOperator;
		case Node_Power:
			curr.type = Node_Pointer;
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1;
			curr.pos += 1;
			goto SimplePrefixOperator;
		case Node_Multiply:
			curr.type = Node_Pointer;
			goto SimplePrefixOperator;
		case Node_BitXor:
			curr.type = Node_BitNot;
			goto SimplePrefixOperator;
		case Node_D_Try:
			[[unlikely]] if (it->type != Node_OpenPar){
				RETURN_ERROR(
					"try keyword at expression level must be followed by opening parenthesis",
					curr.pos
				);
			}
			it += 1;
			curr.type = Node_TryExpression;
			curr.count = 1;
			goto SimplePrefixOperator;

		case Node_Broadcast:
			if (
				it->type == Node_BitNot   ||
				it->type == Node_LogicNot ||
				it->type == Node_Add      ||
				it->type == Node_Subtract
			){
				curr = *it;
				it += 1;
				if (curr.type == Node_Add) curr.type = Node_Plus;
				if (curr.type == Node_Subtract) curr.type = Node_Minus;
				curr.broadcast = true;
				goto SimplePrefixOperator;
			}
			if (it->type == Node_OpenPar || it->type == Node_Identifier){
				goto SimplePrefixOperator;
			}
			curr.type = Node_Self;
			goto SimpleLiteral;
		

		// declarations are treated like expressions
		case Node_Colon:
		case Node_Variable:
		case Node_Constant:
		case Node_OptionalConstant:{
			size_t size = datanode_count(curr.size);
			assert(opers_size+size < SIZE(opers));
			memcpy(&opers[opers_size], it, size*sizeof(Node));
			opers[opers_size+size] = curr;
			opers_size += 1 + size;
			it += size;
			goto ExpectValue;
		}

		case Node_BitNot:
		case Node_Pointer:
		case Node_Span:
		case Node_Splat:
		case Node_LogicNot:
		case Node_D_Run:
		case Node_ArrayClass:
		SimplePrefixOperator:
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1;
			goto ExpectValue;

		case Node_GetField:
			curr.type = Node_EnumLiteral;
			goto SimpleLiteral;

		case Node_Identifier:
		case Node_String:
		case Node_Unresolved:{
			size_t size = datanode_count(curr.size);
			memmove(res_it, it-1, (1+size)*sizeof(Node));
			res_it += (1+size);
			it += size;
			goto ExpectOperator;
		}
		case Node_Unsigned:
		case Node_Float:
		case Node_Double:
		case Node_Character:
			*(res_it+0) = curr;
			*(res_it+1) = *it;
			it += 1;
			res_it += 2;
			goto ExpectOperator;
		SimpleLiteral:
		case Node_Pound:  // '#' can be used for end of array
			*res_it = curr;
			res_it += 1;
			goto ExpectOperator;

		default:
			RETURN_ERROR("expected value", curr.pos);
		}
	}
	
	ExpectOperator:{
		Node curr = *it;
		it += 1;

		if (curr.type == Node_Semicolon){
			for (;;){
				if (opers_size == 1) goto StatementLevel;
				NodeType t = opers[opers_size-1].type;
				if (t == Node_ProcedureBody) break;
				if (Node_Colon <= t && t <= Node_OptionalConstant){
					size_t size = datanode_count(opers[opers_size-1].size);
					memcpy(res_it+1, &opers[opers_size-size-1], size*sizeof(Node));
					*res_it = opers[opers_size-1];
					res_it += 1 + size;
					opers_size -= size;
				} else if (t != Node_Comma){
					*res_it = opers[opers_size-1];
				 	res_it += 1;
				}
				opers_size -= 1;
			}
		}

		[[unlikely]] if (curr.type == Node_Broadcast){
			curr = *it;
			it += 1;
			[[unlikely]] if (Node_Concatenate>curr.type || curr.type>Node_Reinterpret)
				RETURN_ERROR("operator cannot be broadcasted", curr.pos);
			curr.broadcast = true;
		}

		if (Node_K_Do <= curr.type && curr.type <= Node_Reinterpret){
			for (;;){
				NodeType t = opers[opers_size-1].type;
				if (PrecsRight[curr.type] > PrecsLeft[t]) break;
				if (Node_Colon <= t && t <= Node_OptionalConstant){
					size_t size = datanode_count(opers[opers_size-1].size);
					memcpy(res_it+1, &opers[opers_size-size-1], size*sizeof(Node));
					*res_it = opers[opers_size-1];
					res_it += 1 + size;
					opers_size -= size;
				} else if (t != Node_Comma){
					*res_it = opers[opers_size-1];
					res_it += 1;
				}
				opers_size -= 1;
			}
			
			// if is binary operation
			if (Node_Assign <= curr.type){
				assert(opers_size != SIZE(opers));
				opers[opers_size] = curr; opers_size += 1;;
				goto ExpectValue;
			}

			switch (curr.type){
			case Node_Span:
			case Node_Dereference:
			case Node_GetField:
			case Node_Trait:
			SimplePostfixOperator:
				*res_it = curr;
				res_it += 1;
			//	push_value(opers, curr);
				goto ExpectOperator;
			
			case Node_Comma:
				scopes[scopes_size-1].count += 1;
				goto AddWithoutScope; 
			
			case Node_OpenPar:
				curr.type = Node_Call;
				if (opers[opers_size-1].type == Node_Broadcast){
					curr.broadcast = true;
					opers_size -= 1;
				}
				if (it->type == Node_ClosePar){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;
				}
				break;

			case Node_OpenBrace:
				curr.type = Node_Initialize;
				// handle empty initialization
				if (it->type == Node_CloseBrace){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;
				}
				break;

			case Node_OpenBracket:
				curr.type = Node_IndexAccess;
				break;

			case Node_Conditional:
				[[unlikely]] if (it->type != Node_OpenPar){
					RETURN_ERROR(
						"conditional operator must be followed by opening parenthesis",
						curr.pos
					);
				}
				it += 1;
				break;

			case Node_GetProcedure:
				if (it->type == Node_ClosePar){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;	
				}
			case Node_GetFieldIndexed:
				break;

			default: goto StatementLevel;
			}
			curr.count = 1;
			assert(scopes_size != SIZE(scopes));
			scopes[scopes_size] = curr; scopes_size += 1;
		AddWithoutScope:
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1;
			goto ExpectValue;
		}

		[[unlikely]] if (Node_ClosePar > curr.type || curr.type > Node_CloseBrace){
			RETURN_ERROR("expected operator or closing symbol", curr.pos);
		}

		// HANDLE CLOSING SYMBOLS
		for (;;){
			opers_size -= 1;
			NodeType t = opers[opers_size].type;
			if (
				(Node_GetFieldIndexed <= t && t <= Node_Conditional) ||
				(Node_OpenPar <= t && t <= Node_TryExpression)
			) break;
			[[unlikely]] if (t == Node_Terminator){
				static const char *NotOpenedText[] = {
					"unmatched parenthesis",
					"unmatched bracket",
					"unmatched brace",
					"unmatched brace",
				};
				RETURN_ERROR(NotOpenedText[curr.type-Node_ClosePar], curr.pos);
			}
			if (Node_Colon <= t && t <= Node_OptionalConstant){
				size_t size = datanode_count(opers[opers_size].size);
				memcpy(res_it+1, &opers[opers_size-size], size*sizeof(Node));
				*res_it = opers[opers_size];
				res_it += 1 + size;
				opers_size -= size;
			} else if (t != Node_Comma){
				*res_it = opers[opers_size];
				res_it += 1;
			}
		}

		scopes_size -= 1;
		Node sc = scopes[scopes_size];
		
		switch (sc.type){
		case Node_OpenPar:
			[[unlikely]] if (curr.type != Node_ClosePar)
				RETURN_ERROR("mismatched parenthesis", sc.pos);
			if (
				it->type == Node_ProcedureClass   ||
				it->type == Node_ProcedureLiteral ||
				it->type == Node_OpenScope
			){
				if (it->type == Node_OpenScope){
					scopes[scopes_size].type = Node_ProcedureBody;
					scopes[scopes_size].pos = res_it - tokens.ptr;
					scopes_size += 1;
					*res_it = sc;
					res_it += 1;
					sc.type = Node_ProcedureBody;
				} else{
					sc.type = it->type;
				}
				it += 1;
				assert(opers_size != SIZE(opers));
				opers[opers_size] = sc; opers_size += 1;
				goto ExpectValue;
			}
			[[unlikely]] if (sc.count != 1)
				RETURN_ERROR("unexpected comma inside of parenthesis", sc.pos);
			goto ExpectOperator;
		
		case Node_OpenBracket:
			[[unlikely]] if (curr.type != Node_CloseBracket)
				RETURN_ERROR("mismatched bracket", sc.pos);
			assert(opers_size != SIZE(opers));
			{
				NodeType t = it->type;
				if (
					t==Node_Span       || t==Node_OpenBrace ||
					t==Node_Multiply   || t==Node_Power     ||
					t==Node_Identifier || t==Node_D_Class   ||
					t==Node_Unresolved
				){
					sc.type = Node_ArrayClass;
					opers[opers_size] = sc; opers_size += 1;
					goto ExpectValue;
				}
			}
			sc.type = Node_ArrayLiteral;
			opers[opers_size] = sc; opers_size += 1;
			goto ExpectOperator;
		
		case Node_K_Do:
			[[unlikely]] if (curr.type != Node_CloseBrace)
				RETURN_ERROR("do body was not closed", sc.pos);
			if ((res_it-1)->type != Node_K_Return){
				res_it->type = Node_K_Return;
				res_it->pos = sc.pos;
				res_it += 1;
			}
			sc.type = Node_CloseScope;
			sc.pos = curr.pos;
			break;
		
		case Node_ProcedureBody:
			tokens.ptr[sc.pos].pos = res_it - tokens.ptr + 1;
			[[unlikely]] if (curr.type != Node_CloseBrace)
				RETURN_ERROR("procedure body was not closed", sc.pos);
			sc.type = Node_CloseScope;
			sc.pos = curr.pos;
			break;

		case Node_Call:
		case Node_GetProcedure:
			[[unlikely]] if (curr.type != Node_ClosePar)
				RETURN_ERROR("mismatched parenthesis", sc.pos);
			break;
		
		case Node_GetFieldIndexed:
		case Node_IndexAccess:
			[[unlikely]] if (curr.type != Node_CloseBracket)
				RETURN_ERROR("mismatched bracket", sc.pos);
			break;
	
		case Node_OpenBrace:
			[[unlikely]] if (curr.type != Node_CloseBrace)
				RETURN_ERROR("mismatched brace", sc.pos);
			sc.type = Node_StructLiteral;
			break;

		case Node_Initialize:
			[[unlikely]] if (curr.type != Node_CloseBrace)
				RETURN_ERROR("mismatched brace", sc.pos);
			break;
		
		case Node_Conditional:
		case Node_TryExpression:
			[[unlikely]] if (curr.type != Node_ClosePar)
				RETURN_ERROR("mismatched parenthesis", sc.pos);
			if (sc.count == 1){
				[[unlikely]] if (it->type != Node_OpenPar){
					if (sc.type == Node_Conditional){
						RETURN_ERROR("ternary operator expects opening parenthsis", curr.pos);
					} else{
						RETURN_ERROR("try expression expects opening parenthsis", curr.pos);
					}
				}
				it += 1;
				opers_size += 1; // bring back the sentinel for conditional node
				scopes[scopes_size].count = 2;
				scopes_size += 1;
				goto ExpectValue;
			} else{
				break;
			}
		
		default:
			RETURN_ERROR("this error should not be hapening", curr.pos);
		}

		*res_it = sc;
		res_it += 1;
		goto ExpectOperator;
	}
	return tokens;
#undef RETURN_ERROR
}

