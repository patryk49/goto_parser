#pragma once

#include "utils.h"

#include "node_types.h"
#include "bytecode.h"



#define MAX_PARAM_COUNT 8



typedef uint8_t ClassType;
enum ClassType{
	// These are defined globally and are specified insde ClassId if necessary
	// Also their values must be known at compile time
	ClassType_Void,
	ClassType_Class,
	ClassType_InProgress, // type is specified before the assignment symbol
	ClassType_UnknownClass, // type is infered from value
	ClassType_Error,
	ClassType_GenericEnum,

	// These are defined globally and are specified insde ClassId if necessary
	ClassType_Number,
	ClassType_Integer,
	ClassType_Bool,
	ClassType_Character,
	ClassType_Float,

	// Things that use are not really classes but they uses the same data structure
	ClassType_InferedSize, // size that was infered 

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
	ClassType_ProcPointer,
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
	uint8_t bitset;      // maybe use it for knowing which fields/indexes are known at compile time
	uint32_t name_index; // if its 0 variable has no name
} VarFlags;



typedef uint8_t ClassPrefix;
enum ClassPrefix{
	// Tags
	ClassPrefix_None              =  0,
	ClassPrefix_Pointer           =  1,
	ClassPrefix_Span              =  2,
	ClassPrefix_3                 =  3,
	ClassPrefix_4                 =  4,
	ClassPrefix_5                 =  5,
	ClassPrefix_6                 =  6,
	ClassPrefix_7                 =  7,
	ClassPrefix_8                 =  8,
	ClassPrefix_9                 =  9,
	ClassPrefix_10                = 10,
	ClassPrefix_11                = 11,
	ClassPrefix_12                = 12,
	ClassPrefix_13                = 13,
	ClassPrefix_14                = 14,
	ClassPrefix_UnsizedFixedArray = 15,

	// Flags
	ClassPrefixFlag_Constant = 1 << 4,
	ClassPrefixFlag_Restrict = 1 << 5,
	ClassPrefixFlag_3        = 1 << 6,
	ClassPrefixFlag_4        = 1 << 7
};



typedef struct ClassId{
	ClassType   type;         // class type
	ClassPrefix prefixes[11]; // prefix is a parametrized class that doesn't require much space
	uint32_t    index;        // class index
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

static ClassId remove_class_prefix(ClassId class_id){
	memmove(class_id.prefixes, class_id.prefixes+1, sizeof(class_id.prefixes)-1);
	class_id.prefixes[sizeof(class_id.prefixes)-1] = 0;
	return class_id;
}





typedef struct DefinitionPath{
	uint16_t module_id;
	uint16_t symbol_id;
	uint32_t pos;
} DefinitonPath;





typedef union Value{
	const char *debug_text;
	ClassId     class_id;
	PhiLabel    ir;
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
	ClassId c;
	VarFlags f;
	Value v;
} Variable;






// structures that containt information abount non unique classes
typedef struct ArrayClassInfo{
	uint32_t size;
	ClassId class_id;
} ArrayClassInfo;


typedef struct TupleClassInfo{
	uint32_t field_count;
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





typedef struct GlobalInfo{
	// unique classes's info
	TupleClassInfo  *tuples;
	StructClassInfo *structs;
	
	size_t struct_count;
	size_t tuple_count;

	// non unique classes's info
	ArrayClassInfo  *fixed_arrays;
	ArrayClassInfo  *finite_arrays;

	size_t fixed_array_count;
	size_t finite_array_count;

	// TODO: add additional actions, like run directives, asserts, module parameters
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




static bool class_compare(const GlobalInfo *ginfo, ClassId lhs, ClassId rhs){
	if (lhs.type != rhs.type) return false;
	if (memcmp(lhs.prefixes, rhs.prefixes, sizeof(lhs.prefixes)) == 0) return false;
	if (lhs.type < ClassType_FixedArray) return lhs.index == rhs.index;
	// Handle non unique classes that canoot be compared just by ClassId.
	// Comparisons are tail recursive.
	if (lhs.index == rhs.index) return true; // early escape
	if (lhs.type == ClassType_FixedArray){
		ArrayClassInfo lhs_info = ginfo->fixed_arrays[lhs.index];
		ArrayClassInfo rhs_info = ginfo->fixed_arrays[rhs.index];
		if (lhs_info.size != rhs_info.size) return false;
		return class_compare(ginfo, lhs_info.class_id, rhs_info.class_id);
	}
	if (lhs.type == ClassType_FiniteArray){
		ArrayClassInfo lhs_info = ginfo->finite_arrays[lhs.index];
		ArrayClassInfo rhs_info = ginfo->finite_arrays[rhs.index];
		if (lhs_info.size != rhs_info.size) return false;
		return class_compare(ginfo, lhs_info.class_id, rhs_info.class_id);
	}
	assert(false && "Tried to compare non existant, non uniue class.");
}


static bool infrencing_class_compare(
	const GlobalInfo *ginfo,
	ClassId lhs,
	ClassId rhs,
	ClassId **infered
){
	if (rhs.type == ClassType_UnknownClass){
		**infered = lhs;
		*infered += 1;
		return true;
	}
	if (memcmp(lhs.prefixes, rhs.prefixes, sizeof(lhs.prefixes)) == 0) return false;
	if (lhs.type==ClassType_FixedArray && rhs.prefixes[0]==ClassPrefix_UnsizedFixedArray){
		**infered = (ClassId){
			.type  = ClassType_InferedSize,
			.index = ginfo->fixed_arrays[lhs.index].size
		};
		*infered += 1;
		ArrayClassInfo lhs_info = ginfo->fixed_arrays[lhs.index];
		return infrencing_class_compare(ginfo, lhs_info.class_id, remove_class_prefix(rhs), infered);
	}
	if (lhs.type != rhs.type) return false;
	if (lhs.type < ClassType_FixedArray) return lhs.index == rhs.index;
	// Handle non unique classes that canoot be compared just by ClassId.
	if (lhs.index == rhs.index) return true; // early escape
	if (lhs.type == ClassType_FixedArray){
		ArrayClassInfo lhs_info = ginfo->fixed_arrays[lhs.index];
		ArrayClassInfo rhs_info = ginfo->fixed_arrays[rhs.index];
		if (lhs_info.size != rhs_info.size) return false;
		return infrencing_class_compare(ginfo, lhs_info.class_id, rhs_info.class_id, infered);
	}
	if (lhs.type == ClassType_FiniteArray){
		ArrayClassInfo lhs_info = ginfo->finite_arrays[lhs.index];
		ArrayClassInfo rhs_info = ginfo->finite_arrays[rhs.index];
		if (lhs_info.size != rhs_info.size) return false;
		return infrencing_class_compare(ginfo, lhs_info.class_id, rhs_info.class_id, infered);

	}
	assert(false && "Tried to compare non existant, non uniue class.");
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








