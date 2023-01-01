#pragma once

#include "tokenizer.hpp"

#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

uint8_t precs_curr[] = {
// OPENING SYMBOLS
	80, 80, 80, 80, 80,     // OpenPar, OpenBracket, OpenBrace, OpenScope, OpenArrayType,
	
// PREFIX
	72, 72,                 // Pointer, BitNot,
	72, 72,                 // Span, View,
	72, 72,                 // Minus, LogicNot,
	72,                     // UnresolvedValue,
	72,                     // ArrayType,

// POSTFIX
	84,                     // Dereference,
	84,                     // GetField,
	84,                     // GetFiledIndexed,
	84, 84,                 // Call, BroadcastCall, 
	80,                     // Initialize
	84, 84,                 // GetProcedure, IndexAccess,
	32,                     // Conditional,
	84,                     // GetTrait,

// SEPARATORS
	20,  9,                 // Comma, Terminator
	12, 12, 12,             // Colon, DoubleColon, TripleColon,

// ASSIGNS
	30, 30, 30,     // Assign, AssignBytes, DestructuredAssign
	30,             // ConcatenateAssign,
	30,             // ModuloAssign,
	30, 30, 30, 30, // AddAssign, SubtractAssign, ModularAddAssign, ModularSubtractAssign, 
	30, 30, 30, 30, // MultiplyAssign, DivideAssign, ModularMultiplyAssign, ModularDivideAssign,
	30,             // PowerAssign,
	30, 30,         // BitOrAssign, BitNorAssign,
	30, 30,         // BitAndAssign, BitNandAssign,
	30,             // BitXorAssign,
	30, 30,         // LeftShiftAssign, RightShiftAssign,

// BINARY
	38,                     // Concatenate,
	39,                     // Modulo,
	40, 40, 40, 40,         // Add, Subtract, ModularAdd, ModularSubtract, 
	41, 41, 41, 41,         // Multiply, Divide, ModularMultiply, ModularDivide,
	46, /* RTL */           // Power,
	50, 50,                 // BitOr, BitNor,
	51, 51,                 // BitAnd, BitNand,
	52,                     // BitXor,
	53, 53,                 // LeftShift, RightShift,

	32,                     // Pipe,
	33, 33,                 // LogicOr, LogicNor,
	34, 34,                 // LogicAnd, LogicNand,
	35, 35, 35, 35, 35, 35, // Equal, NotEqual, Lesser, Greater, LesserEqual, GreaterEqual,
	36, 36,                 // Contains, DoesntContain,
	40, 40,                 // FullAdd, FullSubtract,
	41, 41,                 // FullMultiply, FullDivide,
	42,                     // CrossProduct,
	71, 71, /* RTL */       // Cast, Reinterpret,
};


uint8_t precs_stack[] = {
// OPENING SYMBOLS
	0, 0, 0, 0, 0,          // OpenPar, OpenBracket, OpenBrace, OpenScope, OpenArrayType,
	
// PREFIX
	82, 82,                 // Pointer, BitNot,
	82, 82,                 // Span, View,
	82, 82,                 // Minus, LogicNot,
	82,                     // UnresolvedValue,
	82,                     // ArrayType,

// POSTFIX
	84,                     // Dereference,
	84,                     // GetField,
	 0,                     // GetFiledIndexed,
	 0,  0,                 // Call, BroadcastCall, 
	 0,                     // Initialize
	 0,  0,                 // GetProcedure, IndexAccess,
	 0,                     // Conditional,
	84,                     // GetTrait,

// SEPARATORS
	20,  8,                 // Comma, Terminator
	12, 12, 12,             // Colon, DoubleColon, TripleColon,

// ASSIGNS
	30, 30, 30,     // Assign, AssignBytes, DestructuredAssign
	30,             // ConcatenateAssign,
	30,             // ModuloAssign,
	30, 30, 30, 30, // AddAssign, SubtractAssign, ModularAddAssign, ModularSubtractAssign, 
	30, 30, 30, 30, // MultiplyAssign, DivideAssign, ModularMultiplyAssign, ModularDivideAssign,
	30,             // PowerAssign,
	30, 30,         // BitOrAssign, BitNorAssign,
	30, 30,         // BitAndAssign, BitNandAssign,
	30,             // BitXorAssign,
	30, 30,         // LeftShiftAssign, RightShiftAssign,

// BINARY
	38,                     // Concatenate,
	39,                     // Modulo,
	40, 40, 40, 40,         // Add, Subtract, ModularAdd, ModularSubtract, 
	41, 41, 41, 41,         // Multiply, Divide, ModularMultiply, ModularDivide,
	45, /* RTL */           // Power,
	50, 50,                 // BitOr, BitNor,
	51, 51,                 // BitAnd, BitNand,
	52,                     // BitXor,
	53, 53,                 // LeftShift, RightShift,

	31,                     // Pipe,
	33, 33,                 // LogicOr, LogicNor,
	34, 34,                 // LogicAnd, LogicNand,
	35, 35, 35, 35, 35, 35, // Equal, NotEqual, Lesser, Greater, LesserEqual, GreaterEqual,
	36, 36,                 // Contains, DoesntContain,
	40, 40,                 // FullAdd, FullSubtract,
	41, 41,                 // FullMultiply, FullDivide,
	42,                     // CrossProduct,
	70, 70, /* RTL */       // Cast, Reinterpret,
};

// LIST OF OPENING SYMBOLS
// OpenPar, OpenBracket, OpenBrace, OpenScope, OpenArrayType,
// GetFieldIndexed,
// Call, BroadcastCall,
// Initialize,
// GetProcedure, IndexAccess,
// Conditional
// Do, Try,

struct ParseResult{
	Span<Node> nodes;
	Error err;
};

ParseResult parse(Span<Node> tokens) noexcept{
	Error error = {};
	#define ERROR_RETURN(msg, pos) {error = {slice(msg), (pos)}; goto Return; }

	FiniteArray<Node, 64> opers;
	push_value(opers, Node{.type=Node::Terminator, .index=0, .pos=0});

	constexpr Node::Type FromNode = Node::OpenPar;

	FiniteArray<Node, 64> scopes;
	
	Node *res_it = tokens.ptr;
	Node *it = tokens.ptr + 1;
	
	// IN PROGRESS (right now parse only expressions)
	goto ExpectValue;

	GlobalLevel:{

	}

	StatementLevel:{
		goto Return;
	}

	ExpectValue:{
		// Handle literals, parenthesis and prefix operators
		Node curr = *it;
		it += 1;
		switch (curr.type){
		case Node::OpenPar:       // Precedence Modifier
		case Node::OpenBrace:     // Struct Literal
		case Node::OpenBracket:   // Array Literal
		case Node::OpenArrayType:     // Array Type Modifier
			curr.count = 1;
			push_value(opers, curr);
			push_value(scopes, curr);
			goto ExpectValue;

		case Node::Broadcast:
			curr = *it;
			it += 1;
			[[unlikely]] if (
				curr.type != Node::BitNot &&
				curr.type != Node::LogicNot &&
				curr.type != Node::Subtract
			) ERROR_RETURN("expression cannot be broadcasted", curr.pos);
			if (curr.type == Node::Subtract) curr.type = Node::Minus;
			curr.broadcast = true;
			goto ExpectValue;

		case Node::Subtract:
			curr.type = Node::Minus;
			goto SimplePrefixOperator;
		case Node::Multiply:
			curr.type = Node::Pointer;
			goto SimplePrefixOperator;
		case Node::BitXor:
			curr.type = Node::BitNot;
			goto SimplePrefixOperator;

		case Node::BitNot:
		case Node::Span:
		case Node::View:
		case Node::LogicNot:
		case Node::Const:
		case Node::UnresolvedValue:
		case Node::ArrayType:
		SimplePrefixOperator:
			push_value(opers, curr);
			goto ExpectValue;

		case Node::Initialize:  // empty struct literal handlling
			curr.type = Node::StructLiteral;
			goto SimpleLiteral;
		case Node::GetField:
			curr.type = Node::EnumLiteral;
			goto SimpleLiteral;

		case Node::Identifier:
		case Node::Integer:
		case Node::Unsigned:
		case Node::Float:
		case Node::Double:
		case Node::Character:
		case Node::String:
		SimpleLiteral:
			*res_it = curr;
			res_it += 1;
			goto ExpectOperator;
		default:
			ERROR_RETURN("expected value", curr.pos);
		}
	}
	
	ExpectOperator:{
		Node curr = *it;
		it += 1;
		
		if (curr.type == Node::Broadcast){
			curr = *it;
			it += 1;
			if (curr.type == Node::OpenPar){
				curr.type = Node::BroadcastCall;
			} else{
				[[unlikely]] if (Node::Concatenate>curr.type || curr.type>Node::Reinterpret)
					ERROR_RETURN("operator cannot be broadcasted", curr.pos);
				curr.broadcast = true;
			}
		}

		if (Node::OpenPar <= curr.type && curr.type <= Node::Reinterpret){
			for (;;){
				Node last_op = opers[opers.size-1];
				if (precs_curr[curr.type-FromNode] > precs_stack[last_op.type-FromNode]) break;
				
				if (last_op.type != Node::Comma){
					*res_it = last_op;
					res_it += 1;
				}
				pop(opers);
			}
			
			if (Node::Assign <= curr.type){
				push_value(opers, curr);
				goto ExpectValue;
			}

			switch (curr.type){
			case Node::Initialize:
			case Node::Span:
			case Node::Dereference:
			case Node::GetField:
			case Node::Trait:
			SimplePostfixOperator:
				*res_it = curr;
				res_it += 1;
			//	push_value(opers, curr);
				goto ExpectOperator;
			
			case Node::Comma:
				scopes[scopes.size-1].count += 1;
				goto AddWithoutScope; 
			
			case Node::OpenPar:
				curr.type = Node::Call;
				if (it->type == Node::ClosePar){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;	
				}
				break;

			case Node::OpenBrace:
				curr.type = Node::Initialize;
				break;

			case Node::OpenBracket:
				curr.type = Node::IndexAccess;
				break;

			case Node::Conditional:
				[[unlikely]] if (it->type != Node::OpenPar)
					ERROR_RETURN("conditional operator expects parenthesis", curr.pos);
				it += 1;
				break;

			case Node::GetProcedure:
			case Node::BroadcastCall:
				if (it->type == Node::ClosePar){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;	
				}
			case Node::GetFieldIndexed:
				break;

			default: goto StatementLevel;
			}
			curr.count = 1;
			push_value(scopes, curr);
		AddWithoutScope:
			push_value(opers, curr);
			goto ExpectValue;

		} else if (Node::ClosePar <= curr.type && curr.type <= Node::CloseBrace){
			for (;;){
				opers.size -= 1;
				Node::Type t = opers[opers.size].type;
				if (
					(Node::GetFieldIndexed <= t && t <= Node::Conditional) ||
					(Node::OpenPar <= t && t <= Node::OpenArrayType)
				) break;

				if (t != Node::Comma){
					*res_it = opers[opers.size];
					res_it += 1;
				}
			}

			scopes.size -= 1;
			Node sc = scopes[scopes.size];
			
			switch (sc.type){
			case Node::OpenPar:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", curr.pos);
				goto ExpectOperator;
			
			case Node::OpenArrayType:
				[[unlikely]] if (curr.type != Node::CloseBracket)
					ERROR_RETURN("mismatched bracket", curr.pos);
				sc.type = Node::ArrayType;
				push_value(opers, sc);
				goto ExpectValue;
			
			case Node::Call:
			case Node::BroadcastCall:
			case Node::GetProcedure:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", curr.pos);
				break;
			
			case Node::OpenBracket:
				sc.type = Node::ArrayLiteral;
			case Node::GetFieldIndexed:
			case Node::IndexAccess:
				[[unlikely]] if (curr.type != Node::CloseBracket)
					ERROR_RETURN("mismatched bracket", curr.pos);
				break;
	
			case Node::OpenBrace:
				sc.type = Node::StructLiteral;
			case Node::Initialize:
				[[unlikely]] if (curr.type != Node::CloseBrace)
					ERROR_RETURN("mismatched brace", curr.pos);
				break;
			
			case Node::Conditional:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", curr.pos);
				if (sc.count == 1){
					curr = *it;
					it += 1;
					[[unlikely]] if (curr.type != Node::OpenPar)
						ERROR_RETURN("ternary operator expects opening parenthsis", curr.pos);
					opers.size += 1;                   // bring back the sentinel for conditional node
					scopes[scopes.size].count = 2;
					scopes.size += 1;                   
					goto ExpectValue;
				} else{
					break;
				}
			default:
				ERROR_RETURN("this error should not be hapening", curr.pos);
			}
			*res_it = sc;
			res_it += 1;
			goto ExpectOperator;

		} else{
			goto StatementLevel;
		}
	}

Return:
	return ParseResult{Span{tokens.ptr, res_it-tokens.ptr}, error};
#undef ERROR_RETURN
}



struct GloabalIdentifier{
	enum Type{
		Procedure, Struct,
		Variable, Condition,
	};

	struct ProcedureData{
		FiniteArray<Node *, 16> args;
		Node *return_val;
		
		Node *body;
	};
	
	struct StructData{
		FiniteArray<Node *, 16> args;
		Node *return_val;
		
		Node *body;
	};

	struct ConditionData{
		Node *expr;
	};

	struct VariableData{
		Node *expression;
		Node *data_class;
		bool is_const;
	};
	
// DATA MEMBERS
	Type type;
	uint16_t name_index;

	union{
		ProcedureData procedure;
		StructData structure;
		ConditionData condition;
		VariableData variable;
	};
};
