#include "tokenizer.hpp"

#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

uint8_t precs_right[] = {
// KEYWORDS
	 1,  1,  1,  1,         // If, Else, While, For,
	 1,  1,                 // Defer, Return,
	 1,  1,  1,             // Break, Continue, Goto,
	 1,  1,                 // Do, Try,
	 1,                     // Assert,
	
	 1,  1,  1,             // Load, Import, Export,
	
	 1,  1,  1,  1,  1,     // Proc, Struct, Enum, Bitfield, Expr,

	 1,                     // Asm,
	27, 29,                 // At, Const,

// OPENING SYMBOLS
	86, 86, 80, 86,         // OpenPar, OpenBracket, OpenBrace, OpenScope,
	86, 86,                 // OpenArrayClass, OpenProcedureLiteral,
	86, 86,                 // DoBlock, TryExpression,
	
// PREFIX
	72, 72,                 // Pointer, BitNot,
	72, 72,                 // Span, View,
	72, 72,                 // Minus, LogicNot,
	72,                     // UnresolvedValue,
	72, 72, 72,             // ArrayClass, ProcedureClass, ProcedureLiteral,
	69,                     // Broadcast,

// POSTFIX
	84,                     // Dereference,
	84,                     // GetField,
	84,                     // GetFiledIndexed,
	84, 80,                 // Call, Initialize,
	84, 84,                 // GetProcedure, IndexAccess,
	32,                     // Conditional,
	84,                     // GetTrait,

// SEPARATORS
	20,                     // Separator
	12, 12, 12,             // Colon, DoubleColon, TripleColon,

// ASSIGNS
	28, 28, 28,     // Assign, AssignBytes, DestructuredAssign
	28,             // ConcatenateAssign,
	28,             // ModuloAssign,
	28, 28, 28, 28, // AddAssign, SubtractAssign, ModularAddAssign, ModularSubtractAssign, 
	28, 28, 28, 28, // MultiplyAssign, DivideAssign, ModularMultiplyAssign, ModularDivideAssign,
	28,             // PowerAssign,
	28, 28,         // BitOrAssign, BitNorAssign,
	28, 28,         // BitAndAssign, BitNandAssign,
	28,             // BitXorAssign,
	28, 28,         // LeftShiftAssign, RightShiftAssign,
	28, 28,         // RotaryLeftShiftAssign, RotaryRightShiftAssign,

// BINARY
	32,                     // Pipe,

	38,                     // Concatenate,
	39,                     // Modulo,
	40, 40, 40, 40,         // Add, Subtract, ModularAdd, ModularSubtract, 
	41, 41, 41, 41,         // Multiply, Divide, ModularMultiply, ModularDivide,
	46, /* RTL */           // Power,
	50, 50,                 // BitOr, BitNor,
	51, 51,                 // BitAnd, BitNand,
	52,                     // BitXor,
	53, 53, 53, 53,         // LeftShift, RightShift, RotaryLeftShift, RotaryRightShift,

	30,                     // Range,
	33, 33,                 // LogicOr, LogicNor,
	34, 34,                 // LogicAnd, LogicNand,
	35, 35, 35, 35, 35, 35, // Equal, NotEqual, Lesser, Greater, LesserEqual, GreaterEqual,
	36, 36,                 // Contains, DoesntContain,
	40, 40,                 // FullAdd, FullSubtract,
	41, 41,                 // FullMultiply, FullDivide,
	42,                     // CrossProduct,
	71, 71, /* RTL */       // Cast, Reinterpret,
};


uint8_t precs_left[] = {
// KEYWORDS
	 1,  1,  1,  1,         // If, Else, While, For,
	 1,  1,                 // Defer, Return,
	 1,  1,  1,             // Break, Continue, Goto,
	 1,  1,                 // Do, Try,
	 1,                     // Assert,
	
	 1,  1,  1,             // Load, Import, Export,
	
	 1,  1,  1,  1,  1,     // Proc, Struct, Enum, Bitfield, Expr,

	 1,                     // Asm,
	27, 29,                 // At, Const,

// OPENING SYMBOLS
	0, 0, 0, 0,            // OpenPar, OpenBracket, OpenBrace, OpenScope,
	0, 0,                  // OpenArrayClass, OpenProcedureLiteral,
	0, 0,                  // DoBlock, TryExpression,
	
// PREFIX
	82, 82,                 // Pointer, BitNot,
	82, 82,                 // Span, View,
	82, 82,                 // Minus, LogicNot,
	82,                     // UnresolvedValue,
	82, 82, 82,             // ArrayClass, ProcedureClass, ProcedureLiteral,
	69,                     // Broadcast,

// POSTFIX
	84,                     // Dereference,
	84,                     // GetField,
	 0,                     // GetFiledIndexed,
	 0,  0,                 // Call, Initialize,
	 0,  0,                 // GetProcedure, IndexAccess,
	 0,                     // Conditional,
	84,                     // GetTrait,

// SEPARATORS
	20,                     // Separator
	12, 12, 12,             // Colon, DoubleColon, TripleColon,

// ASSIGNS
	28, 28, 28,     // Assign, AssignBytes, DestructuredAssign
	28,             // ConcatenateAssign,
	28,             // ModuloAssign,
	28, 28, 28, 28, // AddAssign, SubtractAssign, ModularAddAssign, ModularSubtractAssign, 
	28, 28, 28, 28, // MultiplyAssign, DivideAssign, ModularMultiplyAssign, ModularDivideAssign,
	28,             // PowerAssign,
	28, 28,         // BitOrAssign, BitNorAssign,
	28, 28,         // BitAndAssign, BitNandAssign,
	28,             // BitXorAssign,
	28, 28,         // LeftShiftAssign, RightShiftAssign,
	28, 28,         // RotaryLeftShiftAssign, RotaryRightShiftAssign,

// BINARY
	68,                     // Pipe,
	
	38,                     // Concatenate,
	39,                     // Modulo,
	40, 40, 40, 40,         // Add, Subtract, ModularAdd, ModularSubtract, 
	41, 41, 41, 41,         // Multiply, Divide, ModularMultiply, ModularDivide,
	45, /* RTL */           // Power,
	50, 50,                 // BitOr, BitNor,
	51, 51,                 // BitAnd, BitNand,
	52,                     // BitXor,
	53, 53, 53, 53,         // LeftShift, RightShift, RotaryLeftShift, RotaryRightShift,

	30,                     // Range,
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
// OpenPar, OpenBracket, OpenBrace, OpenScope, OpenArrayClass, OpenProcedureLiteral,
// GetFieldIndexed,
// Call, Initialize,
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
	push_value(opers, Node{Node::Separator, 0, {.separator=Node::Separ::Semicolon}, .0});

	FiniteArray<Node, 64> scopes;
	
	Node *res_it = tokens.ptr;
	Node *it = tokens.ptr + 1;
	
	// IN PROGRESS (right now parse only expressions)
	goto ExpectValue;

	GlobalLevel:{

	}

	StatementLevel:{
		goto Return;
		
		Node curr = *it;
		it += 1;

		switch (curr.type){
		case Node::CloseBrace:
		
		case Node::Return:
		case Node::Defer:

		case Node::If:
		case Node::Else:
		case Node::While:
		case Node::For:
		
		case Node::Break:
		case Node::Continue:
		case Node::Goto:

		case Node::Assert:

		default:
			goto ExpectValue;
		}
	}

	ExpectValue:{
		// Handle literals, parenthesis and prefix operators
		Node curr = *it;
		it += 1;
		switch (curr.type){
		case Node::Do:            // Do local scope
			[[unlikely]] if (it->type != Node::OpenScope && it->type != Node::OpenBrace)
				ERROR_RETURN("do keyword mus be followed by opening brace", curr.pos);
			it += 1;
			curr.type = Node::DoBlock;
			goto SimpleOpeningSymbol;

		case Node::OpenBrace:     // Struct Literal
			if (it->type == Node::CloseBrace){
				it += 1;
				// handling of empty capture symbol
				if (it->type == Node::OpenPar){
					it += 1;

					if (it->type == Node::ClosePar){
						it += 1;
						[[unlikely]] if (
							it->type != Node::ProcedureClass && it->type != Node::ProcedureLiteral
						) ERROR_RETURN(
							"empty parenthesis must be followed by arrow symbol"
							"to denote procedure class or literal",
							curr.pos
						);
						curr.type = it->type;
						curr.count = 0;
						// for procedure class 1 means use the capture, and 0 means dont use the capture
						curr.capture_count = it->type == Node::ProcedureClass;
						it += 1;
						goto SimplePrefixOperator;
					}
					
					curr.type = Node::OpenProcedureLiteral;
					// later, if corresonding ProcedureClass is found, set it to 1
					curr.capture_count = 0;
					goto SimpleOpeningSymbol;
				}
				
				curr.type = Node::StructLiteral;
				curr.count = 0;
				goto SimpleLiteral;
			}
			goto SimpleOpeningSymbol;

		case Node::OpenPar:       // Precedence Modifier
			// handle procedure literals that take no arguments
			if (it->type == Node::ClosePar){
				it += 1;
				[[unlikely]] if (it->type!=Node::ProcedureClass && it->type!=Node::ProcedureLiteral)
					ERROR_RETURN(
						"empty parenthesis must be followed by arrow symbol"
						"to denote procedure class or literal",
						curr.pos
					);
				curr.type = it->type;
				curr.count = 0;
				curr.capture_count = 0;
				goto SimplePrefixOperator;
			}

		case Node::OpenBracket:    // Array Literal
		case Node::OpenArrayClass: // Array Class Modifier
		SimpleOpeningSymbol:
			curr.count = 1;
			push_value(opers, curr);
			push_value(scopes, curr);
			goto ExpectValue;
	
		case Node::Broadcast:
			if (
				it->type == Node::BitNot ||
				it->type == Node::LogicNot ||
				it->type == Node::Subtract
			){
				curr = *it;
				it += 1;
				if (curr.type == Node::Subtract) curr.type = Node::Minus;
				curr.broadcast = true;
			}
			goto SimplePrefixOperator;

		case Node::Subtract:
			curr.type = Node::Minus;
			goto SimplePrefixOperator;
		case Node::Power:
			curr.type = Node::Pointer;
			push_value(opers, curr);
			curr.pos += 1;
			goto SimplePrefixOperator;
		case Node::Multiply:
			curr.type = Node::Pointer;
			goto SimplePrefixOperator;
		case Node::BitXor:
			curr.type = Node::BitNot;
			goto SimplePrefixOperator;
		case Node::Try:
			[[unlikely]] if (it->type != Node::OpenPar){
				ERROR_RETURN(
					"try keyword at expression level must be followed by opening parenthesis",
					curr.pos
				);
			}
			it += 1;
			curr.type = Node::TryExpression;
			curr.count = 1;
			goto SimplePrefixOperator;

		case Node::BitNot:
		case Node::Span:
		case Node::View:
		case Node::LogicNot:
		case Node::Const:
		case Node::UnresolvedValue:
		case Node::ArrayClass:
		SimplePrefixOperator:
			push_value(opers, curr);
			goto ExpectValue;

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
		case Node::UnresolvedType:
		case Node::Pound:
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
			[[unlikely]] if (Node::Concatenate>curr.type || curr.type>Node::Reinterpret)
				ERROR_RETURN("operator cannot be broadcasted", curr.pos);
			curr.broadcast = true;
		}

		if (Node::Const <= curr.type && curr.type <= Node::Reinterpret){
			for (;;){
				Node last_op = opers[opers.size-1];
				if (precs_right[(size_t)curr.type] > precs_left[(size_t)last_op.type]) break;
				
				if (last_op.type != Node::Separator){
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
			case Node::Span:
			case Node::Dereference:
			case Node::GetField:
			case Node::Trait:
			SimplePostfixOperator:
				*res_it = curr;
				res_it += 1;
			//	push_value(opers, curr);
				goto ExpectOperator;
			
			case Node::Separator:
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
				// handle empty initialization
				if (it->type == Node::CloseBrace){
					it += 1;
					curr.count = 0;
					goto SimplePostfixOperator;
				}
				break;

			case Node::OpenBracket:
				curr.type = Node::IndexAccess;
				break;

			case Node::Conditional:
				[[unlikely]] if (it->type != Node::OpenPar){
					ERROR_RETURN(
						"conditional operator must be followed by opening parenthesis",
						curr.pos
					);
				}
				it += 1;
				break;

			case Node::GetProcedure:
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
					(Node::OpenPar <= t && t <= Node::TryExpression)
				) break;

				if (t != Node::Separator){
					*res_it = opers[opers.size];
					res_it += 1;
				}
			}

			scopes.size -= 1;
			Node sc = scopes[scopes.size];
			
			switch (sc.type){
			case Node::OpenPar:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", sc.pos);
				if (it->type == Node::ProcedureClass || it->type == Node::ProcedureLiteral){
					sc.type = it->type;
					sc.capture_count = 0;
					it += 1;
					push_value(opers, sc);
					goto ExpectValue;
				}
				[[unlikely]] if (sc.count != 1)
					ERROR_RETURN("unexpected separator inside the parenthesis", sc.pos);
				goto ExpectOperator;
			
			case Node::OpenArrayClass:
				[[unlikely]] if (curr.type != Node::CloseBracket)
					ERROR_RETURN("mismatched bracket", sc.pos);
				sc.type = Node::ArrayClass;
				push_value(opers, sc);
				goto ExpectValue;
			
			case Node::OpenProcedureLiteral:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", sc.pos);
				[[unlikely]] if (it->type != Node::ProcedureClass && it->type!=Node::ProcedureLiteral)
					ERROR_RETURN(
						"expected procedure class or procedure literal symbol \"->\" or \"=>\"",
						it->pos
					);
				sc.type = it->type;
				if (it->type == Node::ProcedureClass){
					[[unlikely]] if (sc.capture_count != 0)
						ERROR_RETURN("procedure class cannot capture values", sc.pos);
					sc.capture_count = 1;
				}
				it += 1;
				push_value(opers, sc);
				goto ExpectValue;
			
			case Node::Call:
			case Node::GetProcedure:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", sc.pos);
				break;
			
			case Node::OpenBracket:
				sc.type = Node::ArrayLiteral;
			case Node::GetFieldIndexed:
			case Node::IndexAccess:
				[[unlikely]] if (curr.type != Node::CloseBracket)
					ERROR_RETURN("mismatched bracket", sc.pos);
				break;
	
			case Node::OpenBrace:
				[[unlikely]] if (curr.type != Node::CloseBrace)
					ERROR_RETURN("mismatched brace", sc.pos);
				// handle capture expression for procedure literals
				if (it->type == Node::OpenPar){
					it += 1;
					sc.capture_count = sc.count;

					if (it->type == Node::ClosePar){
						it += 1;
						sc.type = Node::ProcedureLiteral;
						sc.count = 0;
						[[unlikely]] if (it->type != Node::ProcedureLiteral)
							ERROR_RETURN("expected procedure literal symbol \"=>\"", it->pos);
						it += 1;
						push_value(opers, sc);
						goto ExpectValue;
					}

					sc.type = Node::OpenProcedureLiteral;
					sc.count = 1;
					scopes[scopes.size] = sc;
					scopes.size += 1;
					opers[opers.size] = sc;
					opers.size += 1;
					goto ExpectValue;
				}
				sc.type = Node::StructLiteral;
				break;

			case Node::Initialize:
				[[unlikely]] if (curr.type != Node::CloseBrace)
					ERROR_RETURN("mismatched brace", sc.pos);
				break;
			
			case Node::Conditional:
			case Node::TryExpression:
				[[unlikely]] if (curr.type != Node::ClosePar)
					ERROR_RETURN("mismatched parenthesis", sc.pos);
				if (sc.count == 1){
					[[unlikely]] if (it->type != Node::OpenPar){
						if (sc.type == Node::Conditional){
							ERROR_RETURN("ternary operator expects opening parenthsis", curr.pos);
						} else{
							ERROR_RETURN("try expression expects opening parenthsis", curr.pos);
						}
					}
					it += 1;
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
		Varaible, Constant, Loads, Evaluation, Condition, Parameters,
	};

// DATA MEMBERS
	Type type;
	uint16_t name_index;
	uint32_t expr;
};
