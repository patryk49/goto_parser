#pragma once

#include "utils.h"

#include "node_types.h"



#define MAX_PARAM_COUNT 8



typedef uint8_t ClassType;
enum ClassType{
	// These are defined globally and are specified insde ClassId if necessary
	// Also their values must be known at compile time
	ClassType_Void,
	ClassType_Class,
	ClassType_InProgress, // type is specified before the assignment symbol
	ClassType_Infered,    // type is infered from value
	ClassType_Error,
	ClassType_GenericEnum,

	// These are defined globally and are specified insde ClassId if necessary
	ClassType_Number,
	ClassType_Integer,
	ClassType_Bool,
	ClassType_Character,
	ClassType_Float,

	// Place for adding costum non parametrized classes
	
	// Those are specifield by external structure indexed by .index filed of ClassId
	// and are unique, wchich means that can be compared by pool and index
	ClassType_Tuple,       // tuple is a struct without names
	ClassType_Struct,
	ClassType_Procedure,   // specific procedure
	ClassType_Enum,
	ClassType_Module,
	ClassType_NumberRange,

	// Those are specifield by external structure indexed by .index filed of ClassId
	// Also the cannot be used recursively.
	ClassType_FixedArray,
	ClassType_FiniteArray,
	ClassType_ProcPointer
};



typedef uint16_t VarFlag;
enum VarFlag{
	VarFlag_Static       = 1 <<  0, // is a compile time variable
	VarFlag_Known        = 1 <<  1, // is known at compile time
	VarFlag_Lvalue       = 1 <<  2, // is assignable
	VarFlag_Constant     = 1 <<  3, // is non mutable
	VarFlag_ThreadLocal  = 1 <<  4, // is thread local
	VarFlag_Multiuse     = 1 <<  5,
	VarFlag_NeedsAddress = 1 <<  6,
	VarFlag_7            = 1 <<  7, // known value does not use pointer indirection
	VarFlag_8            = 1 <<  8, // known value does not use pointer indirection
	VarFlag_9            = 1 <<  9, // known value does not use pointer indirection
	VarFlag_10           = 1 << 10, // known value does not use pointer indirection
	VarFlag_11           = 1 << 11, // known value does not use pointer indirection
	VarFlag_12           = 1 << 12, // known value does not use pointer indirection
	VarFlag_13           = 1 << 13, // known value does not use pointer indirection
	VarFlag_14           = 1 << 14, // known value does not use pointer indirection
	VarFlag_15           = 1 << 15  // known value does not use pointer indirection
};

typedef struct VarFlags{
	VarFlag flags;
	uint8_t alignment;
	uint8_t bit_set;     // maybe use it for knowing which fields/indexes are known at compile time
	uint32_t name_index; // if its 0 variable has no name
} VarFlags;


typedef struct ClassId{
	ClassType type;        // class type
	uint8_t   prefixes[3]; // 6*4 bits that represent [restrict | constant | /span/pointer//0]
	uint32_t  index;       // class index
} ClassId;


static bool is_simple_class_type(ClassType t){
	return ClassType_Number <= t && t <= ClassType_Float;
}

static bool is_simple_class(ClassId class_id){
	if (class_id.prefixes[0] != 0) return false;
	if (class_id.prefixes[1] != 0) return false;
	if (class_id.prefixes[2] != 0) return false;
	return is_simple_class_type(class_id.type);
}





typedef struct DefinitionPath{
	uint16_t module_id;
	uint16_t symbol_id;
	uint32_t pos;
} DefinitonPath;





typedef union Value{
	const char *debug_text;
	ClassId     class_id;
	void       *data;
	void       *pointer;
	char        chars[8];
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
	NodeFlag_4                = 1 << 4,
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
	ClassId c;
	VarFlags f;
	Value v;
} Variable;






typedef struct ModulePool{
	size_t var_size;
	size_t var_capacity;
	ClassId *var_classes;
	union{
		Node **var_expressions;
		Value *var_values;
	};
	VariableName *var_names;

	// TODO: add additional actions, like run directives, asserts, module parameters
} ModulePool;


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


// TEMPORARY MUDULE POOL
ModulePool module_states[32];


static bool class_compare(ClassId lhs, ClassId rhs){
	if (lhs.type != rhs.type) return false;
	if (lhs.prefixes != rhs.prefixes) return false;
	if (lhs.type < ClassType_FixedArray)
		return lhs.index==rhs.index;
	// Handle non unique classes that canoot be compared just by ClassId.
	assert(false && "Tried to compare non existant, non uniue class.");
}





// Calling conventions for procedures
typedef uint8_t CallingConvention;
enum CallingConvention{
	CC_Default = 0,
	CC_C
};



// structures that containt information abount non unique classes
typedef struct ArrayClassInfo{
	ClassId class_id;
	size_t elem_count;
} ArrayClassInfo;


typedef struct TupleClassInfo{
	size_t field_count;
	ClassId class_ids[];
} TupleClassInfo;


typedef struct ProcPointerClassInfo{
	uint8_t arg_count;
	CallingConvention cc;
	ClassId result_class_id;
	ClassId arg_class_ids[MAX_PARAM_COUNT];
} ProcPointerClassInfo;


typedef struct ProcedureClassInfo{
	uint8_t arg_count;
	uint8_t default_count;

	CallingConvention cc;
	
	uint8_t is_arg_static;
	uint8_t is_arg_optionally_static;

	ClassId result_class_id;
	ClassId arg_class_ids[MAX_PARAM_COUNT];

	Node *body;
	void *implementations;

	Node *default_expressions[];
} ProcedureClassInfo;

typedef struct FieldHashElement{
	uint32_t name_index;
	uint16_t name_hash;
	uint16_t field_index;
} FieldHashElement;

typedef struct StructClassInfo{
	uint16_t field_count;
	uint16_t static_count;

	uint16_t bytesize;
	uint16_t alignment;

	uint32_t fields_ht_cap;
	uint32_t statics_ht_cap;

	FieldHashElement *fileds_ht;
	FieldHashElement *statics_ht;

	// holds both static and on static fields (non static first)
	Variable fields[12];
} StructClassInfo;





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

