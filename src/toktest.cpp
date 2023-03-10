#include "SPL/Scans.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"


constexpr const char *token_names[] = {
// KEYWORDS
	"If", "Else", "While", "For",
	"Defer", "Return",
	"Break", "Continue", "Goto",
	"Do", "Try",
	"Assert",

	"Load", "Import", "Export",

	"Proc", "Struct", "Enum", "Bitfield", "Expr",

	"Asm",
	"At", "Const",

// OPENING SYMBOLS
	"OpenPar", "OpenBracket", "OpenBrace", "OpenScope",
	"OpenArrayClass", "OpenProcedureLiteral",
	"DoBlock", "TryExpression",

// PREFIX OPERATIONS
	"Pointer", "BitNot",
	"Span", "View",
	"Minus", "LogicNot",
	"UnresolvedValue",
	"ArrayClass", "ProcedureClass", "ProcedureLiteral",
	"Broadcast",

// POSTFIX OPERATIONS
	"Dereference",
	"GetField", "GetFieldIndexed",
	"Call", "Initailize",
	"GetProcedure", "IndexAccess",
	"Conditional",
	"Trait",

// SEPARATORS
	"Separator",
	"Colon", "DoubleColon", "TripleColon",

// BINARY OPERATIONS
	"Assign", "AssignBytes", "DestructuredAssign",
	
	"ConcatenateAssign",
	"ModuloAssign",
	"AddAssign", "SubtractAssign", "ModularAddAssign", "ModularSubtractAssign", 
	"MultiplyAssign", "DivideAssign", "ModularMultiplyAssign", "ModularDivideAssign",
	"PowerAssign",
	"BitOrAssign", "BitNorAssign",
	"BitAndAssign", "BitNandAssign",
	"BitXorAssign",
	"LeftShiftAssign", "RightShiftAssign",
	"RotaryLeftShiftAssign", "RotaryRightShiftAssign",

	"Pipe",
	
	"Concatenate",
	"Modulo",
	"Add", "Subtract", "ModularAdd", "ModularSubtract", 
	"Multiply", "Divide", "ModularMultiply", "ModularDivide",
	"Power",
	"BitOr", "BitNor",
	"BitAnd", "BitNand",
	"BitXor",
	"LeftShift", "RightShift", "RotaryLeftShift", "RotaryRightShift",

	"Range",
	"LogicOr", "LogicNor",
	"LogicAnd", "LogicNand",
	"Equal", "NotEqual", "Lesser", "Greater", "LesserEqual", "GreaterEqual",
	"Contains", "DoesntContain",
	"FullAdd", "FullSubtract",
	"FullMultiply", "FullDivide",
	"CrossProduct",
	"Cast", "Reinterpret",

// LITERALS
	"Identifier",
	"Integer", "Unsigned", "Double", "Float",
	"Character", "String",
	"EnumLiteral",
	"ExprClass", "Lambda",
	"UnresolvedType",
	"ArrayLiteral", "StructLiteral",

// MODIFIERS
	"Pound",

// BASIC CONSTANTS
	"Class", "Null", "True", "False",

// BASIC COMPILE TIME INFORMATION
	"GetClass", "GetIsconst",
	"GetBytes", "GetAlignment", "GetFieldcount",
	
// CLOSING SYMBOLS
	"ClosePar", "CloseBracket", "CloseBrace", "CloseScope",


// DECLARATIONS
	"Variable", "Constant", "Destructure",
};


void print_error(Error err) noexcept{
	for (size_t i=err.pos; i!=0;){ i-=1; putc(' ', stderr); }
	putc('^', stderr);
	putc(' ', stderr);
	fputs(err.msg.ptr, stderr);
	putc('\n', stderr);
	exit(1);
}



int main(){
	MallocAllocator al = {};
	Array<char> text = read_text(stdin, al);
	push_value(text, '\0', al);

	auto[tokens, names, foreign_names, data, error] = make_tokens((char *)text.ptr, al);
	
	if (error.msg.ptr != nullptr) print_error(error);

	auto[nodes, err] = parse(slice(tokens));
	
	if (err.msg.ptr != nullptr) print_error(err);

	for (auto &it : nodes){
		Node::Type t = it.type;
		printf("%s", token_names[(size_t)t]);
		switch (t){
		case Node::String:
			printf(": \"");
			for (size_t i=0; i!=*(uint32_t *)data[it.index].string; i+=1)
				putchar(data[it.index].string[4+i]);
			putchar('\"');
			break;
		case Node::Character:
			printf(": \'%c\'", it.character);
			break;
		case Node::Integer:
			printf(": %li", data[it.index].i64);
			break;
		case Node::Unsigned:
			printf(": %lu", data[it.index].u64);
			break;
		case Node::Float:
			printf(": %f", data[it.index].f32);
			break;
		case Node::Double:
			printf(": %f", data[it.index].f64);
			break;
		case Node::OpenScope:
		case Node::Break:
		case Node::Continue:
		case Node::Goto:
			if (it.index == UINT16_MAX){
				printf(": ---");
			} else{
				printf(": %u", it.index);
			}
			break;
		case Node::Identifier:
		case Node::Variable:
		case Node::Constant:
		case Node::Colon:
		case Node::DoubleColon:
		case Node::TripleColon:
		case Node::UnresolvedType:
		case Node::UnresolvedValue:
			if (it.index == UINT16_MAX){ 
				printf(": ---");
			} else{
				printf(": ");
				for (size_t i=0; i!=*(uint8_t *)names[it.index]; i+=1)
					putchar(names[it.index][1+i]);
			}
			break;
		case Node::GetField:
		case Node::EnumLiteral:
		case Node::Trait:
			if (it.index == UINT16_MAX){ 
				printf(": ---");
			} else{
				printf(": ");
				for (size_t i=0; i!=*(uint8_t *)foreign_names[it.index]; i+=1)
					putchar(foreign_names[it.index][1+i]);
			}
			break;
		case Node::Call:
		case Node::GetProcedure:
		case Node::IndexAccess:
		case Node::GetFieldIndexed:
		case Node::Initialize:
		case Node::ArrayLiteral:
		case Node::StructLiteral:
		case Node::ArrayClass:
			printf(": %u", (unsigned)it.count);
			break;
		case Node::ProcedureClass:
		case Node::ProcedureLiteral:
			printf(": %u, captures: %u", (unsigned)it.count, (unsigned)it.capture_count);
			break;
		default: 
			if (it.broadcast && (
				(Node::Concatenate <=it.type && it.type <= Node::Reinterpret) ||
				(it.type == Node::BitNot || it.type == Node::LogicNot || it.type == Node::Minus)
			)) printf(" ~ Broadcasted");
			break;
		}
		putchar('\n');
	}

	return 0;
}
