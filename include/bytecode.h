#pragma once

#include "parser.h"

typedef uint32_t RegId;



typedef uint8_t InstrType;
typedef enum InstrType{
	// basic data operatins
	IT_Data,
	IT_Load,
	IT_Store,

	// control flow
	IT_Goto,
	IT_CallStatic,
	IT_CallPointer,
	IT_Return,
	
	// arithmetic/logic (binary ops)
	IT_Add,
	IT_Sub,
	IT_Mul,
	IT_Div,
} ;


typedef uint8_t InstrDataType;
enum InstrDataType{
	IDT_Void = 0,
	IDT_U8, IDT_U16, IDT_U32, IDT_U64,
	IDT_I8, IDT_I16, IDT_I32, IDT_I64,
	IDT_Ptr,
	IDT_F32, IDT_F64
};


enum IAttr{
	IAttr_Static = 1 << 0
};



typedef struct InstrNode{
	InstrType type;          // type of instruction
	InstrDataType data_type; // type of data it returns
	RegId next;              // register id
	InstrAttr *attr;         // instruction attoributes used for debug information

	union{
		struct IN_RawBuf{
			uint8_t value[16];
		} raw_buf;

		struct IN_Num{
			uint64_t value;
		} u64;

		struct IN_Float32{
			float value;
		} f32;

		struct IN_Float64{
			double value;
		} f64;

		struct IN_Phi_Small{
			RegId inputs[2];
		} phi_2;

		struct IN_Phi_Big{
			RegId *inputs;
			uint32_t count;
		} phi_n;
	};
};
