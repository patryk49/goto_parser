#pragma once
#include "tokenizer.h"

#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

// POSSIBLE SCOPES:
// ;  Global
// ;} Procedure
// ;} DoBlock
// ;} Struct
// ,} Enum
// ,) Parameters
// ,] ArrayClass
// ,} Initialize
// ,) Call
// ,) GetPointer
// ,] Index
// ,] StructIndex
// ,} StructLiteral
//  ) Contitional
//  ) D_Try

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

	Node opers[256];
	opers[0].type = Node_S_Global;
	size_t opers_size = 1;

	size_t scope_index = 0;

	Node *res_it = tokens.ptr+1;
	Node *it = tokens.ptr + 1;

	// IN PROGRESS (right now parse only expressions)
	goto ExpectValue;

	GlobalLevel:{
		Node curr = *it;
		it += 1;

		switch (curr.type){
		case Node_D_If:
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
			[[unlikely]] if (it->type != Node_OpenBrace && it->type != Node_OpenScope)
				RETURN_ERROR("do keyword must be followed by opening brace", curr.pos);
			it += 1;
			curr.type = Node_S_DoBlock;
			*res_it = curr;
			res_it += 1;
			goto SimpleOpeningSymbol;

		case Node_OpenBrace:     // Struct Literal
			curr.type = Node_S_StructLiteral;
			if (it->type == Node_CloseBrace){
				it += 1;
				curr.count = 0;
				goto SimpleLiteral;
			}
			curr.count = 1;
			goto SimpleOpeningSymbol;

		case Node_OpenPar:       // Precedence Modifier
			curr.type = Node_S_Parameters;
			if (it->type == Node_ClosePar){
				it += 1;
				curr.count = 0;
				if (it->type == Node_ProcedureClass){
					curr.type = it->type;
					it += 1;
					goto SimplePrefixOperator;
				}
				[[unlikely]] if (it->type != Node_OpenScope && it->type != Node_ProcedureLiteral)
					RETURN_ERROR("empty parenthesis that are not a prameter list", curr.pos);
				curr.type = Node_S_Procedure;
				*res_it = curr;
				res_it += 2;
				if (it->type == Node_OpenScope){
					it += 1;
					assert(opers_size+1 < SIZE(opers));
					opers[opers_size].scope_info.position = res_it - tokens.ptr - 1;
					goto SimpleOpeningSymbol;
				}
				it += 1;
				// dont need to know position of this node so use it to store index
				curr.type = Node_ProcedureLiteral;
				curr.pos = res_it - tokens.ptr - 1;
				goto SimplePrefixOperator;
			}
			curr.count = 1;
			goto SimpleOpeningSymbol;

		case Node_OpenBracket:    // Array Literal
			curr.type = Node_S_ArrayClass;
			curr.count = 1;
		SimpleOpeningSymbol:
			assert(opers_size+1 < SIZE(opers));
			opers[opers_size].scope_info.index = scope_index;
			opers[opers_size+1] = curr;
			scope_index = opers_size + 1;
			opers_size += 2; 
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
				curr.flags |= NodeFlag_Broadcasted;
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
		case Node_K_Return:
		SimplePrefixOperator:
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1;
			goto ExpectValue;

		case Node_GetField:
			curr.type = Node_EnumLiteral;
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
		
		[[unlikely]] if (curr.type == Node_Broadcast){
			curr = *it;
			it += 1;
			[[unlikely]] if (Node_Concatenate>curr.type || curr.type>Node_Reinterpret)
				RETURN_ERROR("operator cannot be broadcasted", curr.pos);
			curr.flags |= NodeFlag_Broadcasted;
		}

		[[unlikely]] if (PrecsLeft[curr.type] == UINT8_MAX)
			RETURN_ERROR("expected operator", curr.pos);

		for (;;){
			enum NodeType t = opers[opers_size-1].type;
			if (PrecsRight[t] < PrecsLeft[curr.type]) break;
			// below condition looks wierd for optimization perpuses
			if (t == Node_ProcedureLiteral || (Node_Colon <= t && t <= Node_OptionalConstant)){
				if (t == Node_ProcedureLiteral){
					size_t skip_node_index = opers[opers_size-1].pos;
					tokens.ptr[skip_node_index].type = Node_Skip;
					tokens.ptr[skip_node_index].pos = res_it - tokens.ptr + 1;
					tokens.ptr[skip_node_index-1].flags |= NodeFlag_UsesShortSyntax;
					(res_it+0)->type = Node_K_Return;
					(res_it+0)->pos = curr.pos;
					(res_it+1)->type = Node_CloseScope;
					res_it += 2;
				} else{
					size_t size = datanode_count(opers[opers_size-1].size);
					memcpy(res_it+1, &opers[opers_size-size-1], size*sizeof(Node));
					*res_it = opers[opers_size-1];
					res_it += 1 + size;
					opers_size -= size;
				}
			} else{
				*res_it = opers[opers_size-1];
				res_it += 1;
			}
			opers_size -= 1;
		}
		
		// if is binary operation
		if (curr.type >= Node_Assign){
			assert(opers_size != SIZE(opers));
			opers[opers_size] = curr; opers_size += 1;
			goto ExpectValue;
		}

		// some safe guard
		if (Node_ClosePar <= curr.type && curr.type <= Node_CloseScope){
			assert(scope_index == opers_size-1);
		}

		switch (curr.type){
		case Node_Span:
		case Node_Dereference:
		SimplePostfixOperator:
			*res_it = curr;
			res_it += 1;
			goto ExpectOperator;
		
		case Node_GetField:{
			size_t size = datanode_count(curr.size);
			memmove(res_it, it-1, (1+size)*sizeof(Node));
			res_it += (1+size);
			it += size;
			goto ExpectOperator;
		}
		
		case Node_Comma:
			// TODO: Assert correct scope
			opers[scope_index].count += 1;
			goto ExpectValue;
		
		case Node_Semicolon:
			// TODO: Assert correct scope
			if (Node_ClosePar <= it->type && it->type <= Node_CloseScope) goto ExpectOperator;
			goto ExpectValue;
		
		case Node_Terminator:
			if (opers[scope_index].type != Node_S_Global)
				RETURN_ERROR("unexpected end of file", curr.pos);
			goto StatementLevel;

		case Node_Conditional:
			[[unlikely]] if (it->type != Node_OpenPar)
				RETURN_ERROR("conditional operator must be followed by opening parenthesis", curr.pos);
			it += 1;
			curr.type = Node_S_Conditional;
			curr.count = 1;
			goto AddWithScope;
		
		case Node_OpenPar:
			curr.type = Node_S_Call;
			if (opers[opers_size-1].type == Node_Broadcast){
				curr.flags |= NodeFlag_Broadcasted;
				opers_size -= 1;
			}
			if (it->type == Node_ClosePar){
				it += 1;
				curr.count = 0;
				goto SimplePostfixOperator;
			}
			curr.count = 1;
			goto AddWithScope;

		case Node_GetProcedure:
			curr.type = Node_S_GetPointer;
			if (it->type == Node_ClosePar){
				it += 1;
				curr.count = 0;
				goto SimplePostfixOperator;	
			}
			curr.count = 1;
			goto AddWithScope;

		case Node_OpenBrace:
			curr.type = Node_S_Initialize;
			if (it->type == Node_CloseBrace){
				it += 1;
				curr.count = 0;
				goto SimplePostfixOperator;
			}
			curr.count = 1;
			goto AddWithScope;

		case Node_OpenBracket:
			curr.type = Node_S_Index;
			curr.count = 1;
			goto AddWithScope;

		case Node_OpenScope:
			if (opers[opers_size-1].type != Node_ProcedureClass)
				RETURN_ERROR("procedure scope in unexpected place", curr.pos);
			opers_size -= 1;
			curr = opers[opers_size];
			curr.type = Node_S_Procedure;
			curr.flags |= NodeFlag_HasReturnType;
			*res_it = curr;
			res_it += 2;
			opers[opers_size].scope_info.position = res_it - tokens.ptr - 1;
			goto AddWithScope;
		
		case Node_GetFieldIndexed:
			curr.type = Node_S_StructIndex;
			curr.count = 1;
		AddWithScope:
			assert(opers_size+1 < SIZE(opers));
			opers[opers_size].scope_info.index = scope_index;
			opers[opers_size+1] = curr;
			scope_index = opers_size + 1;
			opers_size += 2; 
			goto ExpectValue;

		case Node_ClosePar:
			curr = opers[scope_index];
			opers_size -= 2;
			scope_index = opers[opers_size].scope_info.index;
			if (curr.type == Node_S_Parameters){
				if (it->type == Node_ProcedureClass){
					curr.type = it->type;
					it += 1;
					opers[opers_size] = curr; opers_size += 1;
					goto ExpectValue;
				}
				if (it->type != Node_OpenScope && it->type != Node_ProcedureLiteral){
					[[unlikely]] if (curr.count != 1)
						RETURN_ERROR("unexpected comma inside of parenthesis", curr.pos);
					goto ExpectOperator;
				}
				curr.type = Node_S_Procedure;
				*res_it = curr;
				res_it += 2;
				if (it->type == Node_OpenScope){
					it += 1;
					opers[opers_size].scope_info.position = res_it - tokens.ptr - 1;
					goto AddWithScope;
				}
				it += 1;
				curr.type = Node_ProcedureLiteral;
				curr.pos = res_it - tokens.ptr - 1;
				opers[opers_size] = curr; opers_size += 1;
				goto ExpectValue;
			}
			if (curr.type == Node_S_Call) goto SimplePostfixOperator;
			if (curr.type == Node_S_GetPointer) goto SimplePostfixOperator;
			if (curr.type != Node_S_Conditional && curr.type != Node_S_TryExpression)
				RETURN_ERROR("mismatched parenthesis", curr.pos);
			if (curr.count != 1) goto SimplePostfixOperator;
			[[unlikely]] if (it->type != Node_OpenPar){
				if (curr.type == Node_S_Conditional){
					RETURN_ERROR("ternary operator expects opening parenthsis", curr.pos);
				} else{
					RETURN_ERROR("try expression expects opening parenthsis", curr.pos);
				}
			}
			it += 1;
			curr.count = 2;
			goto AddWithScope;

		case Node_CloseBracket:
			curr = opers[scope_index];
			opers_size -= 2;
			scope_index = opers[opers_size].scope_info.index;
			if (curr.type == Node_S_Index) goto SimplePostfixOperator;
			if (curr.type == Node_S_StructIndex) goto SimplePostfixOperator;
			if (curr.type != Node_S_ArrayClass)
				RETURN_ERROR("mismatched bracket", curr.pos);
			curr.type = Node_ArrayClass;
			opers[opers_size] = curr; opers_size += 1;
			goto ExpectValue;

		case Node_CloseBrace:
			curr = opers[scope_index];
			opers_size -= 2;
			scope_index = opers[opers_size].scope_info.index;
			if (curr.type == Node_S_DoBlock){
				if ((res_it-1)->type != Node_K_Return){
					res_it->type = Node_K_Return;
					res_it->pos = curr.pos;
					res_it += 1;
				}
				curr.type = Node_CloseScope;
				goto SimplePostfixOperator;
			}
			if (curr.type == Node_S_Initialize) goto SimplePostfixOperator;
			if (curr.type == Node_S_StructLiteral) goto SimplePostfixOperator;
			if (curr.type == Node_S_Procedure){
				size_t skip_node_index = opers[opers_size].scope_info.position;
				tokens.ptr[skip_node_index].type = Node_Skip;
				tokens.ptr[skip_node_index].pos = res_it - tokens.ptr;
				curr.type = Node_CloseScope;
				goto SimplePostfixOperator;
			}
			if (curr.type != Node_S_Struct && curr.type != Node_S_Enum)
				RETURN_ERROR("mismatched bracket", curr.pos);
			curr.type = Node_CloseScope;
			goto SimplePostfixOperator;
		
		default:
			RETURN_ERROR("unexpected error", curr.pos);
		}
	}
	assert(false && "something wierd happened");
	return tokens;
#undef RETURN_ERROR
}

