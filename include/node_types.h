#pragma once
#include <stdint.h>
#include <stddef.h>



static const size_t NodeTypeCount = 181;



typedef uint8_t NodeType;
enum NodeType{
Node_K_If, Node_K_Else, Node_K_While, Node_K_For, Node_K_Defer, Node_K_Return, Node_K_Break, Node_K_Continue, Node_K_Goto, Node_K_Assert, Node_K_Do, Node_K_Struct, Node_K_Enum, Node_K_Bitfield, Node_K_Expr, Node_D_Load, Node_D_Import, Node_D_Export, Node_D_If, Node_D_For, Node_D_Try, Node_D_Assert, Node_D_Class, Node_D_Size, Node_D_Alignof, Node_D_Isconst, Node_D_Fieldnames, Node_D_Filedcount, Node_D_At, Node_D_Align, Node_D_Inline, Node_D_Noinline, Node_D_Hot, Node_D_Cold, Node_D_Dsalloc, Node_D_Cdsalloc, Node_D_Bssalloc, Node_D_Run, Node_OpenPar, Node_OpenBracket, Node_OpenBrace, Node_OpenScope, Node_ProcedureBody, Node_DoBlock, Node_TryExpression, Node_S_Global, Node_S_Procedure, Node_S_DoBlock, Node_S_Struct, Node_S_Enum, Node_S_Initialize, Node_S_StructLiteral, Node_S_Parameters, Node_S_ArrayClass, Node_S_Call, Node_S_GetPointer, Node_S_Index, Node_S_StructIndex, Node_S_Conditional, Node_S_TryExpression, Node_ClosePar, Node_CloseBracket, Node_CloseBrace, Node_CloseScope, Node_Comma, Node_Semicolon, Node_Terminator, Node_Dereference, Node_GetField, Node_GetFieldIndexed, Node_Call, Node_Initialize, Node_GetProcedure, Node_IndexAccess, Node_Conditional, Node_Trait, Node_Pointer, Node_BitNot, Node_Span, Node_Plus, Node_Minus, Node_LogicNot, Node_Splat, Node_Broadcast, Node_ArrayClass, Node_ProcedureClass, Node_ProcedureLiteral, Node_Colon, Node_Variable, Node_Constant, Node_OptionalConstant, Node_Assign, Node_AssignBytes, Node_DestructuredAssign, Node_ConcatenateAssign, Node_ModuloAssign, Node_AddAssign, Node_SubtractAssign, Node_ModularAddAssign, Node_ModularSubtractAssign, Node_MultiplyAssign, Node_DivideAssign, Node_ModularMultiplyAssign, Node_ModularDivideAssign, Node_PowerAssign, Node_BitOrAssign, Node_BitNorAssign, Node_BitAndAssign, Node_BitNandAssign, Node_BitXorAssign, Node_LeftShiftAssign, Node_RightShiftAssign, Node_RotaryLeftShiftAssign, Node_RotaryRightShiftAssign, Node_Pipe, Node_Concatenate, Node_Modulo, Node_Add, Node_Subtract, Node_ModularAdd, Node_ModularSubtract, Node_Multiply, Node_Divide, Node_ModularMultiply, Node_ModularDivide, Node_Power, Node_BitOr, Node_BitNor, Node_BitAnd, Node_BitNand, Node_BitXor, Node_LeftShift, Node_RightShift, Node_RotaryLeftShift, Node_RotaryRightShift, Node_Range, Node_LogicOr, Node_LogicNor, Node_LogicAnd, Node_LogicNand, Node_Equal, Node_NotEqual, Node_Lesser, Node_Greater, Node_LesserEqual, Node_GreaterEqual, Node_Contains, Node_DoesntContain, Node_FullAdd, Node_FullSubtract, Node_FullMultiply, Node_FullDivide, Node_CrossProduct, Node_Cast, Node_Reinterpret, Node_Identifier, Node_Self, Node_Integer, Node_Unsigned, Node_Double, Node_Float, Node_Character, Node_String, Node_EnumLiteral, Node_ExprClass, Node_Lambda, Node_Unresolved, Node_ArrayLiteral, Node_StructLiteral, Node_Pound, Node_Class, Node_Null, Node_True, Node_False, Node_GetClass, Node_GetIsconst, Node_GetBytes, Node_GetAlignment, Node_GetFieldcount, Node_Error, Node_Skip
};



static const char *NodeTypeNames[] = {
"K_If", "K_Else", "K_While", "K_For", "K_Defer", "K_Return", "K_Break", "K_Continue", "K_Goto", "K_Assert", "K_Do", "K_Struct", "K_Enum", "K_Bitfield", "K_Expr", "D_Load", "D_Import", "D_Export", "D_If", "D_For", "D_Try", "D_Assert", "D_Class", "D_Size", "D_Alignof", "D_Isconst", "D_Fieldnames", "D_Filedcount", "D_At", "D_Align", "D_Inline", "D_Noinline", "D_Hot", "D_Cold", "D_Dsalloc", "D_Cdsalloc", "D_Bssalloc", "D_Run", "OpenPar", "OpenBracket", "OpenBrace", "OpenScope", "ProcedureBody", "DoBlock", "TryExpression", "S_Global", "S_Procedure", "S_DoBlock", "S_Struct", "S_Enum", "S_Initialize", "S_StructLiteral", "S_Parameters", "S_ArrayClass", "S_Call", "S_GetPointer", "S_Index", "S_StructIndex", "S_Conditional", "S_TryExpression", "ClosePar", "CloseBracket", "CloseBrace", "CloseScope", "Comma", "Semicolon", "Terminator", "Dereference", "GetField", "GetFieldIndexed", "Call", "Initialize", "GetProcedure", "IndexAccess", "Conditional", "Trait", "Pointer", "BitNot", "Span", "Plus", "Minus", "LogicNot", "Splat", "Broadcast", "ArrayClass", "ProcedureClass", "ProcedureLiteral", "Colon", "Variable", "Constant", "OptionalConstant", "Assign", "AssignBytes", "DestructuredAssign", "ConcatenateAssign", "ModuloAssign", "AddAssign", "SubtractAssign", "ModularAddAssign", "ModularSubtractAssign", "MultiplyAssign", "DivideAssign", "ModularMultiplyAssign", "ModularDivideAssign", "PowerAssign", "BitOrAssign", "BitNorAssign", "BitAndAssign", "BitNandAssign", "BitXorAssign", "LeftShiftAssign", "RightShiftAssign", "RotaryLeftShiftAssign", "RotaryRightShiftAssign", "Pipe", "Concatenate", "Modulo", "Add", "Subtract", "ModularAdd", "ModularSubtract", "Multiply", "Divide", "ModularMultiply", "ModularDivide", "Power", "BitOr", "BitNor", "BitAnd", "BitNand", "BitXor", "LeftShift", "RightShift", "RotaryLeftShift", "RotaryRightShift", "Range", "LogicOr", "LogicNor", "LogicAnd", "LogicNand", "Equal", "NotEqual", "Lesser", "Greater", "LesserEqual", "GreaterEqual", "Contains", "DoesntContain", "FullAdd", "FullSubtract", "FullMultiply", "FullDivide", "CrossProduct", "Cast", "Reinterpret", "Identifier", "Self", "Integer", "Unsigned", "Double", "Float", "Character", "String", "EnumLiteral", "ExprClass", "Lambda", "Unresolved", "ArrayLiteral", "StructLiteral", "Pound", "Class", "Null", "True", "False", "GetClass", "GetIsconst", "GetBytes", "GetAlignment", "GetFieldcount", "Error", "Skip"
};



static const uint8_t PrecsLeft[] = {
1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 255, 86, 86, 80, 28, 86, 86, 86, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 15, 15, 15, 15, 15, 15, 15, 84, 84, 84, 84, 80, 84, 84, 32, 84, 72, 72, 72, 72, 72, 72, 82, 72, 72, 72, 28, 18, 18, 18, 18, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 32, 38, 39, 40, 40, 40, 40, 41, 41, 41, 41, 46, 50, 50, 51, 51, 52, 53, 53, 53, 53, 29, 32, 32, 33, 33, 34, 34, 35, 35, 35, 35, 36, 36, 40, 40, 41, 41, 42, 71, 71, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};



static const uint8_t PrecsRight[] = {
1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 84, 84, 0, 0, 0, 0, 0, 0, 84, 82, 82, 82, 82, 82, 82, 22, 82, 82, 27, 18, 18, 18, 18, 18, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 68, 38, 39, 40, 40, 40, 40, 41, 41, 41, 41, 45, 50, 50, 51, 51, 52, 53, 53, 53, 53, 29, 32, 32, 33, 33, 34, 34, 35, 35, 35, 35, 36, 36, 40, 40, 41, 41, 42, 70, 70, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};



/*   << KEYWORD INDEXES >>
0000  -  K_If
0001  -  K_Else
0002  -  K_While
0003  -  K_For
0004  -  K_Defer
0005  -  K_Return
0006  -  K_Break
0007  -  K_Continue
0008  -  K_Goto
0009  -  K_Assert
0010  -  K_Do
0011  -  K_Struct
0012  -  K_Enum
0013  -  K_Bitfield
0014  -  K_Expr
0015  -  D_Load
0016  -  D_Import
0017  -  D_Export
0018  -  D_If
0019  -  D_For
0020  -  D_Try
0021  -  D_Assert
0022  -  D_Class
0023  -  D_Size
0024  -  D_Alignof
0025  -  D_Isconst
0026  -  D_Fieldnames
0027  -  D_Filedcount
0028  -  D_At
0029  -  D_Align
0030  -  D_Inline
0031  -  D_Noinline
0032  -  D_Hot
0033  -  D_Cold
0034  -  D_Dsalloc
0035  -  D_Cdsalloc
0036  -  D_Bssalloc
0037  -  D_Run
0038  -  OpenPar
0039  -  OpenBracket
0040  -  OpenBrace
0041  -  OpenScope
0042  -  ProcedureBody
0043  -  DoBlock
0044  -  TryExpression
0045  -  S_Global
0046  -  S_Procedure
0047  -  S_DoBlock
0048  -  S_Struct
0049  -  S_Enum
0050  -  S_Initialize
0051  -  S_StructLiteral
0052  -  S_Parameters
0053  -  S_ArrayClass
0054  -  S_Call
0055  -  S_GetPointer
0056  -  S_Index
0057  -  S_StructIndex
0058  -  S_Conditional
0059  -  S_TryExpression
0060  -  ClosePar
0061  -  CloseBracket
0062  -  CloseBrace
0063  -  CloseScope
0064  -  Comma
0065  -  Semicolon
0066  -  Terminator
0067  -  Dereference
0068  -  GetField
0069  -  GetFieldIndexed
0070  -  Call
0071  -  Initialize
0072  -  GetProcedure
0073  -  IndexAccess
0074  -  Conditional
0075  -  Trait
0076  -  Pointer
0077  -  BitNot
0078  -  Span
0079  -  Plus
0080  -  Minus
0081  -  LogicNot
0082  -  Splat
0083  -  Broadcast
0084  -  ArrayClass
0085  -  ProcedureClass
0086  -  ProcedureLiteral
0087  -  Colon
0088  -  Variable
0089  -  Constant
0090  -  OptionalConstant
0091  -  Assign
0092  -  AssignBytes
0093  -  DestructuredAssign
0094  -  ConcatenateAssign
0095  -  ModuloAssign
0096  -  AddAssign
0097  -  SubtractAssign
0098  -  ModularAddAssign
0099  -  ModularSubtractAssign
0100  -  MultiplyAssign
0101  -  DivideAssign
0102  -  ModularMultiplyAssign
0103  -  ModularDivideAssign
0104  -  PowerAssign
0105  -  BitOrAssign
0106  -  BitNorAssign
0107  -  BitAndAssign
0108  -  BitNandAssign
0109  -  BitXorAssign
0110  -  LeftShiftAssign
0111  -  RightShiftAssign
0112  -  RotaryLeftShiftAssign
0113  -  RotaryRightShiftAssign
0114  -  Pipe
0115  -  Concatenate
0116  -  Modulo
0117  -  Add
0118  -  Subtract
0119  -  ModularAdd
0120  -  ModularSubtract
0121  -  Multiply
0122  -  Divide
0123  -  ModularMultiply
0124  -  ModularDivide
0125  -  Power
0126  -  BitOr
0127  -  BitNor
0128  -  BitAnd
0129  -  BitNand
0130  -  BitXor
0131  -  LeftShift
0132  -  RightShift
0133  -  RotaryLeftShift
0134  -  RotaryRightShift
0135  -  Range
0136  -  LogicOr
0137  -  LogicNor
0138  -  LogicAnd
0139  -  LogicNand
0140  -  Equal
0141  -  NotEqual
0142  -  Lesser
0143  -  Greater
0144  -  LesserEqual
0145  -  GreaterEqual
0146  -  Contains
0147  -  DoesntContain
0148  -  FullAdd
0149  -  FullSubtract
0150  -  FullMultiply
0151  -  FullDivide
0152  -  CrossProduct
0153  -  Cast
0154  -  Reinterpret
0155  -  Identifier
0156  -  Self
0157  -  Integer
0158  -  Unsigned
0159  -  Double
0160  -  Float
0161  -  Character
0162  -  String
0163  -  EnumLiteral
0164  -  ExprClass
0165  -  Lambda
0166  -  Unresolved
0167  -  ArrayLiteral
0168  -  StructLiteral
0169  -  Pound
0170  -  Class
0171  -  Null
0172  -  True
0173  -  False
0174  -  GetClass
0175  -  GetIsconst
0176  -  GetBytes
0177  -  GetAlignment
0178  -  GetFieldcount
0179  -  Error
0180  -  Skip
*/
