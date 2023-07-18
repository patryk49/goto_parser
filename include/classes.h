#pragma once

#include "utils.h"

#include "node_types.h"





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
	ClassType_Polymorphic, // polymorphic procedure

	// These are defined globally and are specified insde ClassId if necessary
	ClassType_Number,
	ClassType_Integer,
	ClassType_Bool,
	ClassType_Character,
	ClassType_Float,

	// Place for adding costum non parametrized classes
	
	// Those are specifield by external structure indexed by .index filed of ClassId
	// and are unique, wchich means that can be compared by pool and index
	ClassType_Tuple,     // tuple is a struct without names
	ClassType_Struct,
	ClassType_Procedure, // specific procedure
	ClassType_Enum,
	ClassType_Module,
	ClassType_NumberRange,

	// Those are specifield by external structure indexed by .index filed of ClassId
	// Also the cannot be used recursively.
	ClassType_FixedArray,
	ClassType_FiniteArray,
	ClassType_ProcPointer,
	
};


typedef uint16_t ValueFlags;
enum ValueFlags{
	ClassFlags_Static       = 1 <<  0, // is known at compile time
	ClassFlags_Known        = 1 <<  1, // it's value is known at compile time
	ClassFlags_Lvalue       = 1 <<  2, // is assignable
	ClassFlags_Constant     = 1 <<  3, // is non mutable
	ClassFlags_ThreadLocal  = 1 <<  4, // is thread local
	ClassFlags_Multiuse     = 1 <<  5,
	ClassFlags_NeedsAddress = 1 <<  6,
	ClassFlags_7            = 1 <<  7, // known value does not use pointer indirection
	ClassFlags_8            = 1 <<  8, // known value does not use pointer indirection
	ClassFlags_9            = 1 <<  9, // known value does not use pointer indirection
	ClassFlags_10           = 1 << 10, // known value does not use pointer indirection
	ClassFlags_11           = 1 << 11, // known value does not use pointer indirection
	ClassFlags_12           = 1 << 12, // known value does not use pointer indirection
	ClassFlags_13           = 1 << 13, // known value does not use pointer indirection
	ClassFlags_14           = 1 << 14, // known value does not use pointer indirection
	ClassFlags_15           = 1 << 15  // known value does not use pointer indirection
};


typedef struct ClassId{
	ValueFlags flags;    // flags that dont change the class id
	uint8_t    more_flags;
	ClassType  type;     // class type
	uint16_t   pool;     // describes in what module class is defined
	uint32_t   prefixes; // 8*4 bits that represent [restrict | constant | /span/pointer//0]
	uint32_t   index;    // index in modules pool, each type has its own pool
} ClassId;





typedef struct DefinitionPath{
	uint16_t module_id;
	uint16_t symbol_id;
	uint32_t pos;
} DefinitonPath;





typedef union Value{
	const char *debug_text;
	ClassId     class_id;
	void       *pointer;
	char        chars[8];
	uint64_t    u64;
	int64_t     i64;
	float       f32;
	double      f64;
	uint32_t    rune;
	bool        boolean;
} Value;




typedef struct NodeScopeInfo{
	uint32_t index;
	uint32_t position;
} NodeScopeInfo;

typedef struct Node{ union{
	struct{
		NodeType type;
		union{
			bool broadcast;
			bool infered;
			bool has_return_type;
		};
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
	
	size_t procedure_count;
	struct PocedureInfo *procedures;

	// ClassInfo for non unique classes
	struct ArrayClassInfo       *fixed_array_classes;
	struct ArrayClassInfo       *finite_array_classes;
	struct ProcPointerClassInfo *proc_pointer_classes;
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
		return lhs.pool==rhs.pool && lhs.index==rhs.index;
	// Handle non unique classes that canoot be compared just by ClassId.
	ModulePool *lm = module_states + lhs.pool;
	ModulePool *rm = module_states + rhs.pool;
	uint32_t *lsize;
	uint32_t *rsize;
	// IDEA: Maybe turn this thing into lookup table.
	switch (lhs.type){
	case ClassType_FixedArray:
		lsize = (uint32_t *)lm->fixed_array_classes + lhs.index;
		rsize = (uint32_t *)rm->fixed_array_classes + rhs.index;
		break;
	case ClassType_FiniteArray:
		lsize = (uint32_t *)lm->finite_array_classes + lhs.index;
		rsize = (uint32_t *)rm->finite_array_classes + rhs.index;
		break;
	case ClassType_ProcPointer:
		lsize = (uint32_t *)lm->proc_pointer_classes + lhs.index;
		rsize = (uint32_t *)rm->proc_pointer_classes + rhs.index;
		break;
	default:
		assert(false && "Tried to compare non existant, non uniue class.");
	}
	if (*lsize != *rsize) return false;
	return memcmp(lsize, rsize, *lsize);
}





// structures that containt information abount non unique classes
typedef struct ArrayClassInfo{
	uint32_t size;
	uint32_t el_count;
	ClassId el_type[];
} ArrayClassInfo;

typedef struct TupleClassInfo{
	uint32_t size;
	uint32_t el_count;
	ClassId el_type[];
} TupleClassInfo;

#define MAX_PARAM_COUNT 8

typedef struct ProcedureInfo{
	uint8_t arg_count;

	ClassId args[MAX_PARAM_COUNT];
	struct Node *defaults[MAX_PARAM_COUNT];
	struct Node *body;
	
	size_t save_count;
	uint8_t *saves;
} ProcedureInfo;

typedef struct ProcPointerClassInfo{
	uint8_t arg_count;
	uint8_t save_count;
	ClassId args[MAX_PARAM_COUNT];
	
	struct Node *defaults[MAX_PARAM_COUNT];
	
	struct Bytecode *saves;
	ClassId *result_classes;
	
	struct Node *body;
} ProcPointerClassInfo;




// possible is_pointer implementation:
// is_pointer :: (T :: Class) => U64~T>>24 != 0 && U64~T>>28 & 2 == 0;


typedef struct TupleInfo{
	ClassId *field_classes;

	uint16_t field_count;
	
	uint16_t bytesize;
	uint16_t alignment;

	uint16_t *offsets;
} TupleInfo;


typedef struct StructInfo{
	ClassId *field_classes;
	ClassId *constant_classes;
	
	char *field_names;
	char *constant_names;

	uint8_t *constant_values;

	uint16_t field_count;
	uint16_t constant_count;
	
	uint16_t bytesize;
	uint16_t alignment;

	uint16_t *offsets;

	// TODO: add [field_name    -> field_index   ] map
	// TODO: add [constant_name -> constant_index] map
} StructInfo;






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

