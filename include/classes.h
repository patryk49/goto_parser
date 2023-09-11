#pragma once

#include "utils.h"

#include "node_types.h"
#include "bytecode.h"



#define MAX_PARAM_COUNT 8



typedef uint8_t ClassType;
enum ClassType{
// SIMMPLE TERMINAL CLASS TYPES:
	// These are defined globally and are specified insde ClassId if necessary
	// Also their values must be known at compile time
	ClassType_Void = 0,
	ClassType_Indirection = 1, // used if class is big and cannot be stored in local buffer
	ClassType_Infered,

	// These are defined globally and are specified insde ClassId if necessary
	ClassType_Number,
	ClassType_Integer,
	ClassType_Bool,
	ClassType_Character,
	ClassType_Float,

	ClassType_Procedure,
	ClassType_Struct,
	ClassType_Enum,
	ClassType_Module,

	// Used only for code generation
	ClassType_Error,
	ClassType_InProgress, // type is specified before the assignment symbol
	ClassType_UnknownClass, // type is infered from value
	ClassType_Declaration, // declaration of variable, used for procedure parameters

// NON TERMINAL CLASS TYPES THAT HAVE ONLY COMPILE TIME VALUES
	ClassType_Class,
	ClassType_GenericEnum,

// NON TERMINAL CLASS TYPES WITH SINGLE ARGUMENT:
	ClassType_Pointer,
	ClassType_Span,
	ClassType_SpanWithStride,
	ClassType_Buffer,
	ClassType_FixedArray,
	ClassType_NumberRange,

// NON TERMINAL CLASS TYPES WITH MULTIPLE ARGUMENTS:
	ClassType_ProcPointer,
	ClassType_Tuple        // tuple is a struct without names
};



typedef int ClassFlags;
enum ClassFlags{
	ClassFlag_Const         =  1 << 0,
	ClassFlag_Restrict      =  1 << 1,
	ClassFlag_PossiblyConst =  1 << 2,
	ClassFlag_3             =  1 << 3,
	ClassFlag_4             =  1 << 4,
	ClassFlag_5             =  1 << 5,
	ClassFlag_6             =  1 << 6,
	ClassFlag_IsReversed    =  1 << 7
};

typedef struct ClassNode{
	uint32_t data;
} Class;

#define CLASS_VOID ((Class){0})
#define CLASS_U8 ((Class){0x00000804})
#define CLASS_MAX_AMOUNT ((1 << 24) - 1)

static ClassType       class_type(const ClassNode *c){ return c->data & 0xff; }
static size_t          class_index(const ClassNode *c){ return c->data >> 8; }
static size_t          class_amount(const ClassNode *c){ return c->data >> 8; }
static enum ClassFlags class_flags(const ClassNode *c){ return c->data >> 8; }

static void class_setindex(ClassNode *c, size_t index){
	c->data = (index<<8) | (c->data&0xff);
}
static void class_setamount(ClassNode *c, size_t amount){
	c->data = (amount<<8) | (c->data&0xff);
}
static void class_setflags(ClassNode *c, ClassFlags flags){
	c->data = (flags<<8) | (c->data&0xff);
}
static void class_addflags(ClassNode *c, ClassFlags flags){
	c->data |= flags << 8;
}
static void class_clearflags(Class *c, ClassFlags flags){
	c->data &= ~(flags << 8);
}


union Class{
	ClassNode data[4];
	struct{
		ClassNode data_0;
		uint32_t capacity;
		Class *ptr;
	};
};
typedef union Class Class;

static ClassNode *class_data(Class *bufer){
	return buffer->data[0].data == ClassType_Indirection ? buffer->ptr : buffer->data;
}

static size_t class_capacity(const Class *buffer){
	return buffer->data[0].data == ClassType_Indirection ? buffer->capacity : 4;
}

// returns new pointer to data
static ClassNode *class_reserve(Class *buffer, size_t size){
	size_t capacity = class_capacity(buffer);
	if (size <= capacity) return class_data(buffer);
	size_t new_capacity = 2*capacity;
	while (new_capacity < size) new_capacity *= 2;
	if (capacity <= 4){
		ClassNode *new_ptr = malloc(new_capacity*sizeof(ClassNode));
		if (new_ptr == NULL) return NULL;
		memcpy(new_ptr, buffer->data, 4*sizeof(ClassNode));
		buffer->data[0] = ClassType_Indirection;
	} else{
		ClassNode *new_ptr = realloc(buffer->ptr, new_capacity*sizeof(ClassNode));
		if (new_ptr == NULL) return NULL;
	}
	buffer->ptr = new_ptr;
	buffer->capacity = new_capacity;
	return new_ptr;
}

typedef uint16_t VarFlag;
enum VarFlag{
	VarFlag_Static       = 1 <<  0, // is a compile time variable
	VarFlag_Known        = 1 <<  1, // is known at compile time
	VarFlag_Lvalue       = 1 <<  2, // is assignable
	VarFlag_Constant     = 1 <<  3, // is non mutable
	VarFlag_ThreadLocal  = 1 <<  4, // is thread local
	VarFlag_Multiuse     = 1 <<  5, // is used more than one time
	VarFlag_NeedsAddress = 1 <<  6, // 
	VarFlag_7            = 1 <<  7, // 
	VarFlag_8            = 1 <<  8, // 
	VarFlag_9            = 1 <<  9, // 
	VarFlag_10           = 1 << 10, // 
	VarFlag_11           = 1 << 11, // 
	VarFlag_12           = 1 << 12, // 
	VarFlag_13           = 1 << 13, // 
	VarFlag_14           = 1 << 14, // 
	VarFlag_15           = 1 << 15  // 
};




typedef struct DefinitionPath{
	uint16_t module_id;
	uint16_t symbol_id;
	uint32_t pos;
} DefinitonPath;





typedef union Value{
	Class       class_buf;
	PhiLabel    ir;
	struct{
		RegId reg_id;
		VarFlags var_flags;
	};
	const char *debug_text;
	void       *data;
	void       *pointer;
	char        chars[16];
	uint64_t    u64;
	int64_t     i64;
	float       f32;
	double      f64;
	uint32_t    rune;
	bool        boolean;
} Value;




typedef uint8_t NodeFlags;
enum NodeFlags{
	NodeFlag_Broadcasted      = 1 << 0,
	NodeFlag_Infered          = 1 << 1,
	NodeFlag_DirectIdentifier = 1 << 2,
	NodeFlag_HasReturnType    = 1 << 3,
	NodeFlag_UsesShortSyntax  = 1 << 4,
	NodeFlag_5                = 1 << 5,
	NodeFlag_6                = 1 << 6,
	NodeFlag_7                = 1 << 7
};

typedef struct NodeScopeInfo{
	uint32_t index;
	uint32_t position;
} NodeScopeInfo;

typedef struct Node{ union{
	struct{
		NodeType type;
		NodeFlags flags;
		union{
			uint16_t  size;
			uint16_t  index;
			int16_t   offset;
			uint16_t  count;
			char      character;
			bool      owning;
		};
		uint32_t pos;
	};
	Value data;
	NodeScopeInfo scope_info;
}; } Node;


static size_t datanode_count(size_t size){
	return (size + sizeof(Node) - 1) / sizeof(Node);
}






typedef struct VariableName{
	char d[32];
} VariableName;

static VariableName name_from_node(const struct Node *node){
	VariableName res = {0};
	memcpy(res.d, node+1, node->size);
	return res;
}




typedef struct Variable{
	Class var_class;
	uint32_t node_index;
	VarFlags flags;
	uint32_t bitset; // Not used right now. Can be changed to something else it the future.
} Variable;






typedef struct ParamInfo{
	uint32_t offsets[];
	uint32_t [parameters];
} ParamInfo;

// structures that containt information abount non unique classes
typedef struct ProcedureClassInfo{
	uint8_t arg_count;
	uint8_t default_count;
	uint8_t infered_count;

	uint8_t is_arg_static;
	uint8_t is_arg_optionally_static;

	ClassId arg_class_ids[MAX_PARAM_COUNT];
	ClassId return_class_ids;
	
	Node *body;
	Variable *defaults;
	
	// array of compile time parameters that match specific implementation
	struct IrProcedure *implementations;
} ProcedureClassInfo;


typedef struct FieldHashElement{
	uint32_t name_index;
	uint16_t name_size;
	uint16_t field_index;
} FieldHashElement;

typedef struct FieldName{
	char data[32];
} FieldName;

typedef struct StructClassInfo{
	uint16_t field_count;
	uint16_t static_field_count;

	uint16_t bytesize;
	uint16_t alignment;

// right not don't use a hash table
//	uint32_t fields_ht_cap;
//	uint32_t statics_ht_cap;

//	FieldHashElement *fileds_ht;
//	FieldHashElement *statics_ht;
	Class classes[4];
	IrNode static_values[];
	uint32_t offsets[];
	FieldName names;
} StructClassInfo;


typedef struct GlobalInfo{
	ProcedureClassInfo *procedure_info;
	StructClassInfo    *struct_info;
	EnumClassInfo      *enum_info;

	size_t              procedure_count;
	size_t              struct_count;
	size_t              enum_count;
} GlobalInfo;


/*
static bool pool_push_variable(ModulePool *pool, VariableName name, ClassId class_id, Node *node){
	if (pool->var_size == pool->var_capacity){
		size_t new_capacity = 2*pool->var_capacity;
		VariableName *new_memory = realloc(
			pool->var_names,
			new_capacity*(sizeof(VariableName)+sizeof(ClassId)+sizeof(Node *))
		);
		if (new_memory == NULL) return true;
		pool->var_capacity    = new_capacity;
		pool->var_names       = new_memory;
		pool->var_classes     = (ClassId *)(new_memory + new_capacity*sizeof(VariableName));
		pool->var_expressions = (Node **)(new_memory + new_capacity*sizeof(ClassId));
	}
	size_t size = pool->var_size;
	pool->var_names[size]       = name;
	pool->var_classes[size]     = class_id;
	pool->var_expressions[size] = node;
	pool->var_size += 1;
	return false;
}
*/



// this procedure moves pointer before serialized class definition it's currently pointing to
static size_t treewalk_class(Class **const source_ptr){
	const Class *source = *source_ptr;
	*source_ptr = source - 1;
	enum ClassType source_type = class_type(*source_ptr);
	if (source_type < ClassType_Pointer) return 0;
	switch (source_type){
	case ClassType_Pointer:
	case ClassType_Span:
	case ClassType_FixedArray:
	case ClassType_FiniteArray:
		treewalk_class(source_ptr);
		return 0;
	case ClassType_Tuple:
		for (size_t i=0; i!=class_amount(source); i+=1) treewalk_class(source_ptr);
		return 0;
	default: break;
	}
	assert(false && "missing case");
}

// procedure that advances pointers and modifies source so that it matches target
static size_t match_classes(
	Class *source_buf,
	Class target_buf,
	size_t *source_index,
	size_t *target_index
){
	enum ClassType source_type = class_type(source);
	enum ClassType target_type = class_type(target);

	if (target_type == ClassType_Infered) return treewalk_class(source_ptr);
	if (source_type != target_type) return 1;
	
	if (source_type < ClassType_Pointer) return class_flags(source) != class_flags(dest);

	switch (source_type){
	case ClassType_Pointer:
	case ClassType_Span:
		if ((class_flags(source) & ~class_flags(target) & ClassFlag_Constant) != 0) return 2;
		class_setflags(source, class_flags(target));
		if (source_type == ClassType_Poiner){
			if (class_type(target-1) == ClassType_Void){
				*(source-1) = CLASS_VOID:
				return treewalk_class(source_ptr);
			}
		}
		return match_classes(source_ptr, target_ptr);
	case ClassType_FixedArray:
		if (class_amount(target) == CLASS_MAX_AMOUNT) return match_classes(source_ptr, target_ptr);
	case ClassType_FiniteArray:
		if (class_amount(source) != class_amount(target)) return 3;
		return match_classes(source_ptr, target_ptr);
	case ClassType_Tuple:
	case ClassType_ProcPointer:
		for (size_t i=0; i!=class_amount(source); i+=1){
			size_t err = match_classes(source_ptr, target_ptr);
			if (err != 0) return err; 
		}
		return 0;
	default: break;
	}
	assert(false && "missing case");
}


// does type checking with inferencing, implicit casting and constant propagation
// the 'var' is modified to match the 'expected_class_id'
// returns a compile time error information if such occured, else returns 0
static RegId match_argument(
	IrProcedure *proc,       // ir procedure in which the matching happens
	uint32_t *param_buffer,  // buffer for static parameters, that is filled when something
	size_t *param_index,     //   get infered
	const ClassNode *source, // source class
	const ClassNode *target, // target class
	RegId reg_id             // id of register that contains source's value
){
	IrNode reg_data = proc->regs[reg_id]; // Remember to not use it when register is fat.
	enum ClassType source_type = class_type(source);
	enum ClassType target_type = class_type(target);

	switch (source_type){
	// classes cannot be converted to anything
	case ClassType_Class:
		if (target_type != ClassType_Class && target_type != ClassType_Infered) return REG_MISMATCH;
		return REG_VOID;

	// if target is an enum it's value can be deduced
	case ClassType_GenericEnum:{
		if (target_type != ClassType_Enum) return REG_MISMATCH;
		// TODO: decide how to store enum's name and get it's value
		return REG_MISMATCH;
	}

	// can coerct to any unsigned or integer
	case ClassType_Number:
	case ClassType_Enum:
	case ClassType_Bool:
		if (target_type != ClassType_Number && target_type != ClassType_Integer) break;
		if (proc->regs[reg_id].data_type == IDT_B8+class_flags(target)) return reg_id;
		if (reg_data.type == IT_Data) return ir_add_register(proc, (IrNode){
			.type      = IT_Data,
			.data_type = IDT_B8 + class_flags(target),
			.u64       = reg_data.u64;
		});
		return ir_add_register(proc, (IrNode){
			.type      = IT_Bitcast,
			.data_type = IDT_B8 + class_flags(target),
			.unary.arg = reg_id
		});

	// can coerct to other integers
	case ClassType_Integer:
		if (target_type != ClassType_Integer) break;
		if (class_flags(source) == class_flags(target)) return reg_id;
		if (reg_data.type == IT_Data){
			// manual sign extension
			uint64_t sign_mask = 1ull << ((8ull << (reg_data.data_type - B8)) - 1);
			return ir_add_register(proc, (IrNode){
				.type      = IT_Data,
				.data_type = IDT_B8 + class_flags(target),
				.u64       = reg.u64 | ~(((reg.u64 & sign_mask) << 1) - 1);
			});
		}
		return ir_add_register(proc, (IrNode){
			.type      = class_flags(source) < class_flags(target) ? IT_SignExtend : IT_Bitcast,
			.data_type = IDT_B8 + class_flags(target),
			.unary.arg = reg_id
		});

	// 64 bit float can coerct to 32 bit float
	case ClassType_Float:
		if (target_type != ClassType_Float || class_flags(source)<=class_flags(target)) break;
		if (reg_data.type == IT_Data) return ir_add_register(proc, (IrNode){
			.type      = IT_Data,
			.data_type = IDT_F32,
			.f32       = (float)reg_data.f64;
		});
		return ir_add_register(proc, (IrNode){
			.type      = IT_Convert,
			.data_type = IDT_F32,
			.unary.arg = reg_id
		});
	
	// pointers can coerct to constant pointer and to void pointers
	case ClassType_Pointer:
		if (target_type != ClassType_Pointer) break;
		if ((class_flags(target) & ClassFlag_PossiblyConst) != 0){
			assert(*param_size != STATIC_PARAM_MAX_SIZE && "too complex static parameters");
			param_buffer[*param_size] = (class_flags(target) & ClassFlag_Const) != 0;
			*param_size += 1;
		} else if ((~class_flags(source) & class_flags(target) & ClassFlag_Const) != 0){
			return REG_MISMATCH;
		}
		if (class_type(source+1) == ClassType_Void) return reg_id;
		if (class_type(target+1) == ClassType_Void) return reg_id;
		return match_classes(param_buffer, param_size, source+1, target+1) ? REG_MISMATCH : reg_id;
	
	// spans can coerct to constant spans
	// additionally static spans can coert to fixed arrays
	case ClassType_Span:
		if (reg_data.type == IT_Data && target_type == ClassType_Array){
			if (match_classes(param_buffer, param_size, source+1, target+1)) return REG_MISMATCH;
			if (class_amount(target) == CLASS_MAX_AMOUNT){
				assert(*param_size != STATIC_PARAM_MAX_SIZE && "too complex static parameters");
				param_buffer[*param_size] = class_amount(source);
				*param_size += 1;
			} else (class_amount(source) != class_amount(target)) return REG_MISMATCH;
			return ir_add_register(proc, (IrNode){
				.type          = IT_Load,
				.data_type     = IT_Raw,
				// TODO: handle sizes
				.load.src_addr = ir_add_registe(proc, (IrNode){
					.type           = IT_Extract,
					.data_type      = IDT_B64,
					.extract.src    = reg_id,
					.extract.offset = 0
				}),
				.load.offset   = 0
			});
		}
		if (target_type != ClassType_Span) break;
		if ((class_flags(target) & ClassFlag_PossiblyConst) != 0){
			assert(*param_size != STATIC_PARAM_MAX_SIZE && "too complex static parameters");
			param_buffer[*param_size] = (class_flags(target) & ClassFlag_Const) != 0;
			*param_size += 1;
		} else if ((~class_flags(source) & class_flags(target) & ClassFlag_Const) != 0){
			return REG_MISMATCH;
		}
		return match_classes(param_buffer, param_size, source+1, target+1) ? REG_MISMATCH : reg_id;
	
	// buffers also can coerct to constant spans
	case ClassType_Buffer:{
		if (target_type != ClassType_Span || (class_flags(target) & ClassFlag_Const) == 0) break;
		if (match_classes(source+1, target+1)) return REG_MISMATCH;
		// do the premature optimization
		if (reg_data.type == IT_Pack && reg_data.size == 3) return ir_add_register(proc, (IrNode){
			.type            = IT_Pack,
			.data_type       = IT_Raw,
			.size            = 2,
			.pack.sources[0] = reg_data.pack.sources[0],
			.pack.sources[1] = ir_add_register(proc, (IrNode){
				.type      = IT_Bitcast,
				.data_type = IDT_B64,
				.unary.arg = reg_data.pack.sources[1]
			})
		});
		return ir_add_register(proc, (IrNode){
			.type            = IT_Pack,
			.data_type       = IT_Raw,
			.size            = 2,
			.pack.sources[0] = ir_add_register(proc, (IrNode){
				.type           = IT_Extract,
				.data_type      = IDT_Ptr,
				.extract.source = reg_id,
				.extract.offset = 0
			}),
			.pack.sources[1] = ir_add_register(proc, (IrNode){
				.type      = IT_Bitcast,
				.data_type = IDT_B64,
				.unary.arg = ir_add_register(proc, (IrNode){
					.type           = IT_Extract,
					.data_type      = IDT_B32,
					.extract.source = reg_id,
					.extract.offset = 8
				})
			})
		});
	}
	
	// arrays can coerct to constant spans and to unsized arrays
	case ClassType_Array:{
		if (target_type == ClassType_Array && class_amount(target) == CLASS_MAX_AMOUNT){
			assert(*param_size != STATIC_PARAM_MAX_SIZE && "too complex static parameters");
			param_buffer[*param_size] = class_amount(source);
			*param_size += 1;
		}
		if (target_type != ClassType_Span || (class_flags(target) & ClassFlag_Const) == 0) break;
		if (match_classes(source+1, target+1) != 0) break;
		return ir_add_register(proc, (IrNode){
			.type      = IT_Pack,
			.data_type = IT_Raw,
			.size      = 2,
			.pack.sources[0] = ir_add_register(proc, (IrNode){
				.type      = IT_Push,
				.data_type = IDT_Ptr,
				.push      = reg_id
			}),
			.pack.sources[1] = ir_add_register(proc, (IrNode){
				.type      = IT_Data,
				.data_type = IDT_B64,
				.u64       = class_amount(source)
			})
		});
	}
	
	// tuples can coerct to other tuples, arrays or structures
	case ClassType_Tuple:{
		size_t source_size = class_amount(source);
		// empty tuple can coerct to any type
		if (source_size == 0){
			// TODO: Return register with zero value of the target's size.
			// To do this the target's class must be known, and the "bytesize" operation is required.
			return REG_MISMATCH;
		}
		switch (target_type){
		case ClassType_Array:{
			size_t target_size = class_amount(target);
			if (target_size == CLASS_MAX_AMOUNT){
				assert(*param_size != STATIC_PARAM_MAX_SIZE && "too complex static parameters");
				param_buffer[*param_size] = class_amount(source);
				*param_size += 1;
			} else if (source_size != target_size) return REG_MISMATCH;
			size_t param_size_copy = *param_size;
			RegId first_reg = match_argument(
				proc,
				param_buffer, param_size,
				source+1, target+1,
				reg_id
			);
			if (param_size_copy != *param_size) return REG_MISMATCH;
			// handle singleton case
			if (source_size == 1) return first_reg;
			// create register that would contain the result, inside a temporary buffer
			size_t array_size = 1 + ((source_size-4)*sizeof(RegId)+sizeof(IrNode)-1)/sizeof(IrNode);
			IrNode buffer[2];
			IrNode *array_node = buffer;
			if (array_size > sizeof(buffer)){
				array_node = malloc(array_size*sizeof(IrNode));
				if (array_node == NULL) return REG_OOM;
			}
			*array_node = (IrNode){
				.type            = IT_Pack,
				.data_type       = IDT_Raw,
				.size            = source_size,
				.pack.sources[0] = first_reg
			};
			// match tuple fields agaist the first element's class
			for (size_t i=1; i!=source_size; i+=1){
				array_node->pack.sources[i] = match_argument(
					proc,
					param_buffer, &param_size_copy,
					source+1+i, target+1,
					reg_id
				);
				assert(param_size_copy == *param_size && "inference should not be happening in here");
			}
			RegId res_reg_id = ir_alloc_register(proc, array_size);
			memcpy(proc.regs+res_reg_id, array_node, array_size*sizeof(IrNode));
			if (array_node != buffer) free(array_node);
			return res_reg_id;
		}
		case ClassType_Struct:{
			/*
			if (class_amount(source) == ginfo->struct_info[class_index(target)].field_count){
				for (size_t i=0; i!=class_amount(source); i+=1){
					// TODO: match tuple field's class against struct filed's class
				}
				// TODO: match tuple field's class against struct field's class
			}
			*/
			break;
		}
		default: break;
		}
	}
	
	// procedures can be coerted to procedure pointers,
	// but only if procedure pointer is fully defined
	case ClassType_Procedure:{
		if (target_type == ClassType_Infered) return REG_VOID;
		if (target_type != ClassType_ProcPointer) break;
	}
	default:
		break;
	}

	// If source was a pointer it would have been handled before.
	if (target_type == ClassType_Pointer && (class_flags(target) & ClassFlag_Const) != 0){
		return ir_add_register(proc, (IrNode){
			.type      = IT_Push,
			.data_type = IT_Pointer,
			.push.src  = match_argument(
				proc,
				param_buffer, param_size,
				source, target+1,
				reg_id
			)
		});
	}
	return reg_id;
}



// NODE ARRAY CLASS
typedef struct{
	Node *ptr;
	uint32_t size;
	uint32_t capacity;
} NodeArray;


static bool NodeArray_reserve(NodeArray *arr, size_t required_space){
	assert(arr->size <= arr->capacity);
	if (arr->capacity < required_space){
		uint32_t new_capacity = arr->size!=0 ? 2*arr->capacity : 64;
		Node *new_ptr = realloc(arr->ptr, new_capacity*sizeof(Node));
		//if (new_ptr == NULL) return true;
		assert(new_ptr != NULL);
		arr->ptr = new_ptr;
		arr->capacity = new_capacity;
	}
	return false;
}

static Node *NodeArray_push(NodeArray *arr){
	assert(arr->size <= arr->capacity);
	if (arr->size == arr->capacity){
		uint32_t new_capacity = arr->size!=0 ? 2*arr->capacity : 64;
		Node *new_ptr = realloc(arr->ptr, new_capacity*sizeof(Node));
		//if (new_ptr == NULL) return NULL;
		assert(new_ptr != NULL);
		arr->ptr = new_ptr;
		arr->capacity = new_capacity;
	}
	Node *result = arr->ptr + arr->size;
	arr->size += 1;
	return result;
}


// VARIABLE STACK 

typedef struct VariableStack{
	ClassId *classes; // it also stores the metadata
	Value *values;
	uint32_t size;
	uint32_t capacity;
} VariableStack;








