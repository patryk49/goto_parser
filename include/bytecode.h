#pragma once

#include "utils.h"

typedef uint32_t RegId;
#define REG_MISMATCH (UINT32_MAX - 0) // indicates type mismatch
#define REG_OOM      (UINT32_MAX - 1) // indicates faild memory allocation
#define REG_VOID     (UINT32_MAX - 9) // means that there's no need for register

#define STATIC_PARAMS_MAX_SIZE 32


// Calling conventions for procedures
typedef uint8_t CallingConvention;
enum CallingConvention{
	CC_Default = 0,
	CC_C
};




// IR nodes are virtual registers that can also act like memory
typedef uint8_t InstrType;
enum InstrType{
	// Basic data operatins
	IT_Data,

	// Memory Operations
	IT_Load,  // load value from existing memory
	IT_Store, // store value in existing memory
	IT_Push,  // store value in allocated stack memory
	IT_Alloc, // allocate stack memory
	
	IT_Memcpy,
	IT_Memset,
	IT_Memcmp,

	// Register Operations
	IT_Pack2,
	IT_Pack3,
	IT_Pack4,
	IT_PackN,

	// Control Flow
	IT_Goto,
	IT_Call,
	IT_Return,
	
	// Porcedures
	IT_Call,
	IT_CallPtr,
	IT_ProcPointer,
	

	// Unary Arithmetic/Logic
	IT_Bitcast,
	IT_SignExtend,
	IT_Convert,

	// Arithmetic/Logic (binary ops)
	IT_Add,
	IT_Sub,
	IT_Mul,
	IT_Div,

};


typedef uint8_t InstrDataType;
enum InstrDataType{
	IDT_Void = 0,
	IDT_Raw,
	IDT_B8, IDT_B16, IDT_B32, IDT_B64,
	IDT_Ptr,
	IDT_F32, IDT_F64
	
	// data that is not stored dorectly in the node
	IDT_Indirect, // used only for data nodes
	IDT_Zeoros,   // used only for data nodes
};


typedef uint8_t InstrFlags;
enum InstrFlags{
	IFlag_Hot      = 1 <<  0,
	IFlag_Cold     = 1 <<  1,
	IFlag_Static   = 1 <<  2,
	IFlag_InMemory = 1 <<  3,
	IFlag_4        = 1 <<  4,
	IFlag_5        = 1 <<  5,
	IFlag_6        = 1 <<  6,
	IFlag_7        = 1 <<  7,
};


typedef struct PhiLabel{
	RegId begin;
	RegId value;
} PhiLabel;


// IDEA: store the additional data for node inline, and not behind the indirection
typedef struct IrNode{
	InstrType type;          // type of instruction
	InstrDataType data_type; // type of data it returns
	InstrFlags flags;        // flags that don't fit in data union
	uint8_t size;
	RegId next;              // register id

	union{
		struct IN_Load{
			RegId src_addr;
			uint32_t step;
			uint32_t offset;
		} load;
		
		struct IN_Store{
			RegId src;
			RegId dest_addr;
			uint32_t step;
			uint32_t offset;
		} store;
		
		struct IN_Push{
			RegId src;
			size_t alignment;
		} push;
		
		struct IN_Alloc{
			size_t size;
			size_t alignemnt;
		} alloc;

		struct IN_Call{
			uint32_t proc_index;
			uint32_t param_count;
			RegId args_data[2];
		} call;
		
		struct IN_ProcPointer{
			uint32_t proc_index;
		} proc_pointer;
		
		uint8_t  bytes[16];
		uint8_t  array_u8[16];
		uint16_t array_u16[8];
		uint32_t array_u32[4];
		uint64_t array_u64[2];
		void    *ptr;
		uint64_t u64;
		float    f32;
		double   f64;
		
		struct IN_Pack{
			RegId sources[4];
		} pack;

		struct IN_PhiBig{
			PhiLabel inputs[2];
		} phi_big;
		
		struct IN_UnaryOp{
			RegId arg;
		} binary_op;

		struct IN_BinaryOp{
			RegId lhs;
			RegId rhs;
		} binary_op;
	};
} IrNode;


typedef struct IrProcedure{
	uint32_t param_count;
	bool is_pure;
	
	uint32_t reg_size;
	uint32_t reg_capacity;
	IrNode  *regs;
	
	uint32_t static_param_size;
	uint32_t static_param_data[];
} IrProcedure;


static RegId ir_add_register(IrProcedure *proc, IrNode node){
	RegId result = proc->reg_size;
	
	if (proc->reg_size == proc->reg_capacity){
		IrNode *new_regs = realloc(2*proc->reg_size*sizeof(IrNode));
		if (new_regs == NULL) return REG_ERROR;
		proc->regs = new_regs;
		proc->reg_capacity = 2*proc->reg_size;
	}

	proc->regs[result] = node;
	proc->reg_size = result + 1;
	return result;
}

static RegId ir_alloc_register(IrProcedure *proc, size_t size){
	RegId result = proc->reg_size;
	size_t required_size = proc->reg_size + size;

	if (required_size > proc->reg_capacity){
		IrNode *new_regs = realloc(2*required_size*sizeof(IrNode));
		if (new_regs == NULL) return REG_ERROR;
		proc->regs = new_regs;
		proc->reg_capacity = 2*required_size;
	}

	proc->reg_size = result + size;
	return result;
}
