#pragma once

#include "utils.h"

typedef uint32_t RegId;




// Calling conventions for procedures
typedef uint8_t CallingConvention;
enum CallingConvention{
	CC_Default = 0,
	CC_C
};





typedef uint8_t InstrType;
enum InstrType{
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
};


typedef uint8_t InstrDataType;
enum InstrDataType{
	IDT_Void = 0,
	IDT_U8, IDT_U16, IDT_U32, IDT_U64,
	IDT_I8, IDT_I16, IDT_I32, IDT_I64,
	IDT_Ptr,
	IDT_F32, IDT_F64
};


typedef uint8_t InstrFlags;
enum InstrFlags{
	IFlag_Hot    = 1 <<  0,
	IFlag_Cold   = 1 <<  1,
	IFlag_Static = 1 <<  2,
	IFlag_3      = 1 <<  3,
	IFlag_4      = 1 <<  4,
	IFlag_5      = 1 <<  5,
	IFlag_6      = 1 <<  6,
	IFlag_7      = 1 <<  7,
	IFlag_8      = 1 <<  8,
	IFlag_9      = 1 <<  9,
	IFlag_10     = 1 << 10,
	IFlag_11     = 1 << 11,
	IFlag_12     = 1 << 12,
	IFlag_13     = 1 << 13,
	IFlag_14     = 1 << 14,
	IFlag_15     = 1 << 15
};


typedef struct PhiLabel{
	RegId begin;
	RegId value;
} PhiLabel;



typedef struct InstrNode{
	InstrType type;          // type of instruction
	InstrDataType data_type; // type of data it returns
	InstrFlags flags;        // flags that don't fit in data union
	RegId next;              // register id

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
			PhiLabel inputs[2];
		} phi_2;

		struct IN_Phi_Big{
			PhiLabel *inputs;
			size_t count;
		} phi_n;
	};
} InstrNode;
