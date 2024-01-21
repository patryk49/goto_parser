#pragma once
#include <stdint.h>
#include <stddef.h>



static const size_t NodeTypeCount = 181;



typedef uint8_t NodeType;
enum NodeType{
Node_K_If, Node_K_Else, Node_K_While, Node_K_For, Node_K_Defer, Node_K_Return, Node_K_Break, Node_K_Continue, Node_K_Goto, Node_K_Assert, Node_K_Do, Node_K_Struct, Node_K_Enum, Node_K_Bitfield, Node_K_Expr, Node_D_Load, Node_D_Import, Node_D_Export, Node_D_If, Node_D_For, Node_D_Try, Node_D_Assert, Node_D_Class, Node_D_Size, Node_D_Alignof, Node_D_Isconst, Node_D_Fieldnames, Node_D_Filedcount, Node_D_At, Node_D_Align, Node_D_Inline, Node_D_Noinline, Node_D_Hot, Node_D_Cold, Node_D_Dsalloc, Node_D_Cdsalloc, Node_D_Bssalloc, Node_D_Run, Node_OpenPar, Node_OpenBracket, Node_OpenBrace, Node_OpenParams, Node_OpenScope, Node_DoBlock, Node_TryExpression, Node_S_Global, Node_S_Procedure, Node_S_DoBlock, Node_S_Struct, Node_S_Enum, Node_S_ArrayClass, Node_S_Call, Node_S_GetPointer, Node_S_Index, Node_S_StructIndex, Node_S_Conditional, Node_S_TryExpression, Node_ClosePar, Node_CloseBracket, Node_CloseBrace, Node_CloseScope, Node_Comma, Node_Semicolon, Node_Terminator, Node_Dereference, Node_GetField, Node_GetFieldIndexed, Node_Call, Node_Initialize, Node_GetProcedure, Node_IndexAccess, Node_Conditional, Node_Pointer, Node_DoublePointer, Node_InferedPointer, Node_BitNot, Node_Span, Node_InferedSpan, Node_Plus, Node_Minus, Node_LogicNot, Node_Splat, Node_Broadcast, Node_ArrayClass, Node_ProcedureClass, Node_ProcedureLiteral, Node_Variable, Node_Colon, Node_Constant, Node_Assign, Node_AssignBytes, Node_DestructuredAssign, Node_ConcatenateAssign, Node_ModuloAssign, Node_AddAssign, Node_SubtractAssign, Node_ModularAddAssign, Node_ModularSubtractAssign, Node_MultiplyAssign, Node_DivideAssign, Node_ModularMultiplyAssign, Node_ModularDivideAssign, Node_PowerAssign, Node_BitOrAssign, Node_BitNorAssign, Node_BitAndAssign, Node_BitNandAssign, Node_BitXorAssign, Node_LeftShiftAssign, Node_RightShiftAssign, Node_RotaryLeftShiftAssign, Node_RotaryRightShiftAssign, Node_Pipe, Node_Concatenate, Node_Modulo, Node_Add, Node_Subtract, Node_ModularAdd, Node_ModularSubtract, Node_Multiply, Node_Divide, Node_ModularMultiply, Node_ModularDivide, Node_Power, Node_BitOr, Node_BitNor, Node_BitAnd, Node_BitNand, Node_BitXor, Node_LeftShift, Node_RightShift, Node_RotaryLeftShift, Node_RotaryRightShift, Node_Range, Node_LogicOr, Node_LogicNor, Node_LogicAnd, Node_LogicNand, Node_Equal, Node_NotEqual, Node_Lesser, Node_Greater, Node_LesserEqual, Node_GreaterEqual, Node_Contains, Node_DoesntContain, Node_FullAdd, Node_FullSubtract, Node_FullMultiply, Node_FullDivide, Node_CrossProduct, Node_Cast, Node_Reinterpret, Node_Identifier, Node_Self, Node_Infered, Node_Integer, Node_Unsigned, Node_Double, Node_Float, Node_Character, Node_String, Node_EnumLiteral, Node_ExprClass, Node_Lambda, Node_Unresolved, Node_ParentProcedure, Node_ArrayLiteral, Node_StructLiteral, Node_Pound, Node_Class, Node_Null, Node_True, Node_False, Node_GetClass, Node_GetIsconst, Node_GetBytes, Node_GetAlignment, Node_GetFieldcount, Node_Error, Node_Skip
};



static const char *NodeTypeNames[] = {
"K_If", "K_Else", "K_While", "K_For", "K_Defer", "K_Return", "K_Break", "K_Continue", "K_Goto", "K_Assert", "K_Do", "K_Struct", "K_Enum", "K_Bitfield", "K_Expr", "D_Load", "D_Import", "D_Export", "D_If", "D_For", "D_Try", "D_Assert", "D_Class", "D_Size", "D_Alignof", "D_Isconst", "D_Fieldnames", "D_Filedcount", "D_At", "D_Align", "D_Inline", "D_Noinline", "D_Hot", "D_Cold", "D_Dsalloc", "D_Cdsalloc", "D_Bssalloc", "D_Run", "OpenPar", "OpenBracket", "OpenBrace", "OpenParams", "OpenScope", "DoBlock", "TryExpression", "S_Global", "S_Procedure", "S_DoBlock", "S_Struct", "S_Enum", "S_ArrayClass", "S_Call", "S_GetPointer", "S_Index", "S_StructIndex", "S_Conditional", "S_TryExpression", "ClosePar", "CloseBracket", "CloseBrace", "CloseScope", "Comma", "Semicolon", "Terminator", "Dereference", "GetField", "GetFieldIndexed", "Call", "Initialize", "GetProcedure", "IndexAccess", "Conditional", "Pointer", "DoublePointer", "InferedPointer", "BitNot", "Span", "InferedSpan", "Plus", "Minus", "LogicNot", "Splat", "Broadcast", "ArrayClass", "ProcedureClass", "ProcedureLiteral", "Variable", "Colon", "Constant", "Assign", "AssignBytes", "DestructuredAssign", "ConcatenateAssign", "ModuloAssign", "AddAssign", "SubtractAssign", "ModularAddAssign", "ModularSubtractAssign", "MultiplyAssign", "DivideAssign", "ModularMultiplyAssign", "ModularDivideAssign", "PowerAssign", "BitOrAssign", "BitNorAssign", "BitAndAssign", "BitNandAssign", "BitXorAssign", "LeftShiftAssign", "RightShiftAssign", "RotaryLeftShiftAssign", "RotaryRightShiftAssign", "Pipe", "Concatenate", "Modulo", "Add", "Subtract", "ModularAdd", "ModularSubtract", "Multiply", "Divide", "ModularMultiply", "ModularDivide", "Power", "BitOr", "BitNor", "BitAnd", "BitNand", "BitXor", "LeftShift", "RightShift", "RotaryLeftShift", "RotaryRightShift", "Range", "LogicOr", "LogicNor", "LogicAnd", "LogicNand", "Equal", "NotEqual", "Lesser", "Greater", "LesserEqual", "GreaterEqual", "Contains", "DoesntContain", "FullAdd", "FullSubtract", "FullMultiply", "FullDivide", "CrossProduct", "Cast", "Reinterpret", "Identifier", "Self", "Infered", "Integer", "Unsigned", "Double", "Float", "Character", "String", "EnumLiteral", "ExprClass", "Lambda", "Unresolved", "ParentProcedure", "ArrayLiteral", "StructLiteral", "Pound", "Class", "Null", "True", "False", "GetClass", "GetIsconst", "GetBytes", "GetAlignment", "GetFieldcount", "Error", "Skip"
};



static const uint8_t PrecsLeft[] = {
1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 255, 86, 86, 80, 86, 28, 86, 86, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 15, 15, 15, 15, 15, 15, 15, 84, 84, 84, 84, 80, 84, 84, 32, 72, 72, 72, 72, 72, 72, 72, 72, 72, 82, 72, 72, 72, 28, 18, 18, 18, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 32, 38, 39, 40, 40, 40, 40, 41, 41, 41, 41, 46, 50, 50, 51, 51, 52, 53, 53, 53, 53, 29, 32, 32, 33, 33, 34, 34, 35, 35, 35, 35, 36, 36, 40, 40, 41, 41, 42, 71, 71, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};



static const uint8_t PrecsRight[] = {
1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 84, 84, 0, 0, 0, 0, 0, 0, 82, 82, 82, 82, 82, 82, 82, 82, 82, 22, 82, 82, 27, 18, 18, 18, 18, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 68, 38, 39, 40, 40, 40, 40, 41, 41, 41, 41, 45, 50, 50, 51, 51, 52, 53, 53, 53, 53, 29, 32, 32, 33, 33, 34, 34, 35, 35, 35, 35, 36, 36, 40, 40, 41, 41, 42, 70, 70, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
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
0041  -  OpenParams
0042  -  OpenScope
0043  -  DoBlock
0044  -  TryExpression
0045  -  S_Global
0046  -  S_Procedure
0047  -  S_DoBlock
0048  -  S_Struct
0049  -  S_Enum
0050  -  S_ArrayClass
0051  -  S_Call
0052  -  S_GetPointer
0053  -  S_Index
0054  -  S_StructIndex
0055  -  S_Conditional
0056  -  S_TryExpression
0057  -  ClosePar
0058  -  CloseBracket
0059  -  CloseBrace
0060  -  CloseScope
0061  -  Comma
0062  -  Semicolon
0063  -  Terminator
0064  -  Dereference
0065  -  GetField
0066  -  GetFieldIndexed
0067  -  Call
0068  -  Initialize
0069  -  GetProcedure
0070  -  IndexAccess
0071  -  Conditional
0072  -  Pointer
0073  -  DoublePointer
0074  -  InferedPointer
0075  -  BitNot
0076  -  Span
0077  -  InferedSpan
0078  -  Plus
0079  -  Minus
0080  -  LogicNot
0081  -  Splat
0082  -  Broadcast
0083  -  ArrayClass
0084  -  ProcedureClass
0085  -  ProcedureLiteral
0086  -  Variable
0087  -  Colon
0088  -  Constant
0089  -  Assign
0090  -  AssignBytes
0091  -  DestructuredAssign
0092  -  ConcatenateAssign
0093  -  ModuloAssign
0094  -  AddAssign
0095  -  SubtractAssign
0096  -  ModularAddAssign
0097  -  ModularSubtractAssign
0098  -  MultiplyAssign
0099  -  DivideAssign
0100  -  ModularMultiplyAssign
0101  -  ModularDivideAssign
0102  -  PowerAssign
0103  -  BitOrAssign
0104  -  BitNorAssign
0105  -  BitAndAssign
0106  -  BitNandAssign
0107  -  BitXorAssign
0108  -  LeftShiftAssign
0109  -  RightShiftAssign
0110  -  RotaryLeftShiftAssign
0111  -  RotaryRightShiftAssign
0112  -  Pipe
0113  -  Concatenate
0114  -  Modulo
0115  -  Add
0116  -  Subtract
0117  -  ModularAdd
0118  -  ModularSubtract
0119  -  Multiply
0120  -  Divide
0121  -  ModularMultiply
0122  -  ModularDivide
0123  -  Power
0124  -  BitOr
0125  -  BitNor
0126  -  BitAnd
0127  -  BitNand
0128  -  BitXor
0129  -  LeftShift
0130  -  RightShift
0131  -  RotaryLeftShift
0132  -  RotaryRightShift
0133  -  Range
0134  -  LogicOr
0135  -  LogicNor
0136  -  LogicAnd
0137  -  LogicNand
0138  -  Equal
0139  -  NotEqual
0140  -  Lesser
0141  -  Greater
0142  -  LesserEqual
0143  -  GreaterEqual
0144  -  Contains
0145  -  DoesntContain
0146  -  FullAdd
0147  -  FullSubtract
0148  -  FullMultiply
0149  -  FullDivide
0150  -  CrossProduct
0151  -  Cast
0152  -  Reinterpret
0153  -  Identifier
0154  -  Self
0155  -  Infered
0156  -  Integer
0157  -  Unsigned
0158  -  Double
0159  -  Float
0160  -  Character
0161  -  String
0162  -  EnumLiteral
0163  -  ExprClass
0164  -  Lambda
0165  -  Unresolved
0166  -  ParentProcedure
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
