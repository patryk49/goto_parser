#pragma once 

#include "utils.h"

// LIST OF NODES WITH 1 DATA STORAGE

// Declare, Colon, DoubleColon, GetField, Trait, Identifier, Unresolved, String, EnumLiteral






/*
typedef struct ParameterNode{
	union{
		ClassId class_id; // when node coresponds to last parameter it holds implementation id
		ClassId *class_ids;
	};	
	Value *values;       // this thing also holds the next nodes,
								// 0th index is for runtime implementation when its possible
	uint16_t count;
	uint16_t capacity;
	
	uint16_t infered; // how many infered parameters it has
	
	// if the infered parameters is not 0 then the class_ids is used intead of class_id
} ParameterNode;

ParameterNode *get_nexts(ParameterNode *node) noexcept{
	return (ParameterNode *)(node->values + node->capacity) + 1;
}


// information about unresolved class
typedef struct UnresolvedClass{
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
} UnresolvedClass;


// Information about solid class
typedef struct ClassInfo{
	UnresolvedClass *unresolved;
	Variable *constants;
	Variable *fields;
	uint16_t constant_count;
	uint16_t field_count;
	
	uint16_t bytesize;
	uint16_t alignment;

	Procedure *operators;
} ClassInfo; // IN PRACTICE IT WOULD BE STORED AS STRUCT OF ARRAYS


	

static ClassId match_paramaters(
	const ClassId *input_classes,
	const Value *input_values,
	size_t input_count,
	UnresolvedClass *matcher
) noexcept{
	size_t param_count = matcher->param_count;
	[[unlikely]] if (input_count < matcher->min_param_count)
		RAISE_ERROR("too few parameters provided");
	[[unlikely]] if (input_count > param_count)
		RAISE_ERROR("too many parameters provided");

	size_t arg_index = 0;
	NextArgument:{
		arg_index += 1;
		if (arg_index == param_count) return curr_param->class_id;
		
		if (arg_index == input_count){
			// fill in the default value
			Variable default_value = evaluate_constant_expression(
				matcher->param_defaults[input_count],
				inputs // this could pull values from caller's side, so passing stack would be required
			);
			// PLACE FOR ERROR HANDLING
			inputs[input_count] = default_value;
			input_count += 1;
		}
	
		ClassId iclass_flags = classflags(inputs[arg_index].class_id);
		ClassId pclass_flags = classflags(curr_param->class_id);
		
		if (pclass_flags == 1 && iclass_flags == 0)
			RAISE_ERROR("provided parameter is not known at compile time");
		if (pclass_flags == 0 || iclass_flags == 0){
			curr_param = *(get_nexts(curr_param) - 1); // this is the index of runtime implementaion
			goto NextArgument;
		}
		
		if (curr_param->infered == 0) {
			// here class can be checked only once
			if ()
				RAISE_ERROR("provided parameter hast wrong class");
			
			// match more complex values
			size_t input_value_size = classes[(size_t)iclass_id].bytesize;
			for (size_t i=0; i!=curr_param->count; i+=1){
				if (memcmp(
					inputs[arg_index].value.ptr,
					curr_param->values[i].ptr,
					input_value_size
				)){
					curr_param = *(get_nexts(curr_param) + i);
					goto NextArgument;
				}
			}
		} else{
			// here class and value must be checked at each iteration
			size_t input_value_size = classes[(size_t)iclass_id].bytesize;
			for (size_t i=0; i!=curr_param->count; i+=1){
				if (
					iclass_id == curr_param->class_ids[i] &&
					memcmp(inputs[arg_index].value.ptr, curr_param->values[i].ptr, input_value_size)
				){
					curr_param = *(get_nexts(curr_param) + i);
					goto NextArgument;
				}
			}
		}
	}

 	// when neccesary, resize the table of next parameters
	if (curr_param->count == curr_param->capacity){
		size_t new_cap = 2 * curr_param->count;
		Variable *new_memory = (Variable *)alloc(
			al, new_cap*sizeof(Variable) + (new_cap+1)*sizeof(ParameterNode)
		).ptr;
		memcpy(new_memory, curr_param->values, curr_param->capacity*sizeof(Variable));
		memcpy(
			new_memory + 2*curr_param->capacity,
			get_nexts(curr_param) - 1,
			(curr_param->capacity + 1)* sizeof(ParameterNode)
		);
		curr_param->capacity *= 2;
	}

	for (;;){
		curr_param->values[curr_param->count] = inputs[arg_index].value;
		curr_param->count += 1;

		if (arg_index == param_count){
			ClassId new_implementation_id = make_class_implementation(matcher, inputs, al);
			get_nexts(curr_param)->class_id = new_implementation_id;	
			return new_implementation_id;
		}

		arg_index += 1;
		
		if (arg_index == input_count){
			// fill in the default value
			Variable default_value = evaluate_constant_expression(
				matcher->param_defaults[input_count],
				inputs // this could pull values from caller's side, so passing stack would be required
			);
			// PLACE FOR ERROR HANDLING
			inputs[input_count] = default_value;
			input_count += 1;
		}

		ParameterNode *new_param = alloc(al, sizeof(ParameterNode));
		new_param->values = (Variable *)alloc(al, sizeof(Variable) + sizeof(ParameterNode));
		new_param->class_id = evaluate_constant_expression(
			matcher->param_classes[arg_index],
			inputs
		);
		new_param->count = 0;
		new_param->capacity = 1;
		// TODO: handle infered values
		new_param->infered = 0;
		[[unlikely]] if (inputs[arg_index].class_id != new_param->class_id)
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



Variable evaluate_constant_expression(Node *expr, Span<Varaible> stack) noexcept{
	for (;;){
		Node curr = *expr;
		expr += 1;
		switch (curr.type){
		case Node::Call:
			Variable caller = match_parameters(
				&stack[stack.size - curr.count],
				curr.count,
				stack[stack.size - curr.count - 1].value
			);
			stack.size -= curr.count;
			if (caller.class_if == BasicCalss::Void){
				
			} else if (caller.class_id == BasicClass::ClassId){
				stack[stack.count-1] = caller;
			} else{
				stack[stack.cout-1] = unchecked_call(caller, &stack[stack.size]);
			}
			break;
		case Node::GetProcedure:
			Varaible caller = match_parameters(
				&stack[stack.size - curr.count],
				curr.count,
				stack[stack.size - curr.count - 1].value
			);
			stack.size -= curr.count;
			stack[stack.count-1] = caller;
		}
		case Node::Add:
			
	}
}
*/
