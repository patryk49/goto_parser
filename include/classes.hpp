#pragma once 

#include "parser.hpp"

struct BasicClass{
	enum : uint64_t{
		Void = 0,
		U8, U16, U32, U64,
		I8, I16, I32, I64,
		Bool, Char,
		F32, F64,
		Class,
		Parametrized, Deduced,
		UnresolvedProcedure,

		// LITERALS
		Object, Enum,
		Number, Character, String,
	};
};

// struct representing a variable
struct Variable{
	ClassId class_id;
	Value value;
};

// possible is_pointer implementation:
// is_pointer :: (T :: Class) => U64~T>>24 != 0 && U64~T>>28 & 2 == 0;

// How class is represented: 99998888777766665555444433332222111ppppBBBBBBBBxxxxxxxxxxxxxxxx
// pppp - nuber of prefixes
// 9999-1111 - type of n'th prefix,
//   bits represent ( --/restrict | mutable/-- | pointer/span/2d_span/something_else )
// BBBBBBBB - bits which represents parametrized intrinsic type (has different type table)
// xxxxxxxxxxxxxxxx - unique id representing instance of solid class
using ClassId = uint64_t;
constexpr size_t ClassSpecificationIdBits = 16;
constexpr size_t ClassInstanceIdBits = 24;

constexpr size_t MaxParamCount = 8;


bool is_basic(ClassId class_id) noexcept{
	return class_id <= (ClassId)BasicClass::String;
}

bool is_simple(ClassId class_id) noexcept{ // "simple" to compare
	return is_basic(class_id) || class_id >> ClassInstanceIdBits != 0;
}

bool is_intrinsic(ClassId class_id) noexcept{
	return (class_id >> ClassSpecificationIdBits) & 0xff != 0;
}



// List of intrinsic parametric types:
// fixed array, finite array, procedure pointer, delegate, module

// Information about solid class
struct ClassInfo{
	UnresolvedClass *unresolved;
	Variable *constants;
	Variable *fields;
	uint16_t constant_count;
	uint16_t field_count;
	
	uint16_t bytesize;
	uint16_t alignment;

	Procedure *operators;
}; // IN PRACTICE IT WOULD BE STORED AS STRUCT OF ARRAYS



struct ParameterNode{
	union{
		ClassId class_id; // when node coresponds to last parameter it holds implementation id
		ClassId *class_ids;
	};	
	Value *values;    // this thing also holds the next nodes
	uint16_t count;
	uint16_t capacity;
	
	uint16_t infered; // how many infered parameters it has
	
	// if the infered parameters is not 0 then the class_ids is used intead of class_id
};

ParameterNode *get_nexts(ParameterNode *node) noexcept{
	return (ParameterNode *)(node->values + node->capacity);
}


// information about unresolved class
struct UnresolvedClass{
	uint8_t param_count;
	uint8_t min_param_count;
	uint8_t infered_count;
	Node *param_classes[MaxParamCount];
	Node *param_defaults[MaxParamCount];
	ParameterNode implementations;

	uint32_t *param_names; // infered values are put after the param_names
	Node *fields;
	// implementations is tree structure that holds arguments in binary format
	// default arguments will be always put at frst index at instatiotion time

	// Parameters to class must be trivially comparable
};


static ClassId match_paramaters(
	const Variable *inputs,
	size_t input_count,
	UnresolvedClass *matcher
) noexcept{
	size_t param_count = matcher->param_count;
	[[unlikely]] if (input_count < matcher->min_param_count)
		RAISE_ERROR("too few parameters proveded");
	[[unlikely]] if (input_count > param_count)
		RAISE_ERROR("too many parameters proveded");
	
	// prepare the input
	Variable input[MaxParamCount];
	memcpy(input, inputs, input_count*sizeof(Variable));
	
	// fill in the default values
	while (input_count != matcher->param_count){
		Variable default_class_id = evaluate_constant_expression(
			matcher->param_classes[input_count],
			input
		);
		[[unlikely]] if (default_class_id.class_id != BasicClass::Class)
			RAISE_ERROR("the thing provieded inside parameter's class declaration is not a class");
		Variable default_value = evaluate_constant_expression(
			matcher->param_classes[input_count],
			input
		)
		[[unlikely]] if (default_value.class_id != default_class_id.value.class_id)
			RAISE_ERROR("parameter's default value doesn't match it's class");

		input[input_count].class_id = default_class_id.value.class_id;
		input[input_count].value = default_value.value;
		input_count += 1;
	}

	ParameterNode *curr_param = &matcher->implementations;
	size_t arg_index = (size_t)-1;

	NextArgument:{
		arg_index += 1;
		if (arg_index == input_count) return curr_param->class_id;
	
		ClassId iclass_id = input[arg_index].class_id;
		if (curr_param->infered == 0){
			// here class can be checked only once
			if (iclass_id != curr_param->class_id)
				RAISE_ERROR("parameter cannot be matched, because it's class dont match");
			
			if (is_simple(iclass_id)){
				// match very simple values
				for (size_t i=0; i!=curr_param->count; i+=1){
					if (input[arg_index].value.u64 == curr_param->values[i].u64){
						curr_param = *(get_nexts(curr_param) + i);
						goto NextArgument;
					}
				}
			} else{
				// match more complex values
				size_t input_value_size;
				if (is_intrinsic(iclass_id)){
					puts("NOT IMPLEMNETED"); exit(1);
					//input_value_size = intrinsic_classes[(size_t)iclass_id].bytesize;
				} else{
					input_value_size = classes[(size_t)iclass_id].bytesize;
				}
				for (size_t i=0; i!=curr_param->count; i+=1){
					if (memcmp(input[arg_index].value.ptr, curr_param->values[i].ptr, input_value_size)){
						curr_param = *(get_nexts(curr_param) + i);
						goto NextArgument;
					}
				}
			}
		} else{
			// here class and value must be checked at each iteration
			if (is_simple(iclass_id)){
				// match very simple values
				for (size_t i=0; i!=curr_param->count; i+=1){
					if (
						iclass_id == curr_param->class_ids[i] &&
						input[arg_index].value.u64 == curr_param->values[i].u64
					){
						curr_param = *(get_nexts(curr_param) + i);
						goto NextArgument;
					}
				}
			} else{
				// match more complex values
				size_t input_value_size;
				if (is_intrinsic(iclass_id)){
					puts("NOT IMPLEMNETED"); exit(1);
					//input_value_size = intrinsic_classes[(size_t)iclass_id].bytesize;
				} else{
					input_value_size = classes[(size_t)iclass_id].bytesize;
				}
				for (size_t i=0; i!=curr_param->count; i+=1){
					if (
						iclass_id == curr_param->class_ids[i] &&
						memcmp(input[arg_index].value.ptr, curr_param->values[i].ptr, input_value_size)
					){
						curr_param = *(get_nexts(curr_param) + i);
						goto NextArgument;
					}
				}
			}
		}
	}

 	// when neccesary, resize the table of next parameters
	if (curr_param->count == curr_param->capacity){
		Value *new_memory = (Value *)alloc(
			al, 2 * curr_param->count * (sizeof(Value) + sizeof(ParameterNode))
		);
		memcpy(new_memory, curr_param->values, curr_param->capacity*sizeof(Value));
		memcpy(
			new_memory + 2*curr_param->capacity,
			get_nexts(curr_param),
			curr_param->capacity * sizeof(ParameterNode)
		);
		curr_param->capacity *= 2;
	}

	for (;;){
		curr_param->values[curr_param->count] = input[arg_index].value;
		curr_param->count += 1;

		if (arg_index == input_count){
			ClassId new_implementation_id = make_class_implementation(matcher, input, al);
			get_nexts(curr_param)->class_id = new_implementation_id;	
			return new_implementation_id;
		}

		arg_index += 1;

		ParameterNode *new_param = alloc(al, sizeof(ParameterNode));
		new_param->values = (Value *)alloc(al, sizeof(Value) + sizeof(ParameterNode));
		new_param->class_id = evaluate_constant_expression(
			matcher->param_classes[arg_index],
			input
		);
		new_param->count = 0;
		new_param->capacity = 1;
		// TODO: handle infered values
		new_param->infered = 0;
		
		[[unlikely]] if (input[arg_index].class_id != new_param->class_id)
			RAISE_ERROR("arguments's class does not match parameter's class");
		
		*(get_nexts(curr_param) + curr_param->count - 1) = new_param;
		curr_param = new_param;
	}
}



// number of params must match the total number of unresolved class's parameters
ClassId make_class_implementation(
	UnresolvedClass *unresolved,
	Variable *params,
	Allocator al
) noexcept{
	ClassInfo new_class = {};
	new_class.unresolved = matcher;
	new_class.constant_count = matcher->param_count;
	new_class.constants = (Variable *)alloc(al, new_class.constant_count*sizeof(Variable)).ptr;

	for (size_t i=0; i!=new_class.constant_count; i+=1){
		new_class.constants[i].class_id = (params+i)->class_id;
		new_class.constants[i].value.u64 = (params+i)->value.u64;
		// TODO: ADD PARAMETER'S NAME TO new_class
	}

	// TODO: CHECK AND ADD FIELDS TO new_class
	Node *node = matcher->fields;

	push_value(classes, new_class, class_allocator);
	return classes.size - 1;
}


