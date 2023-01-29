#pragma once 

#include "classes.hpp"

// Information about procedure
struct ProcedureInfo{
	UnresolvedClass *unresolved;
	Variable *constants;
	Variable *fields;
	uint16_t constant_count;
	uint16_t field_count;
	
	uint16_t bytesize;
	uint16_t alignment;

	Procedure *operators;

	bool is_inline;
	bool is_const;
}; // IN PRACTICE IT WOULD BE STORED AS STRUCT OF ARRAYS


struct ParameterNode{
	ClassId class_id; // when node coresponds to last parameter it holds implementation id
	Value *values;    // this thing also holds the next nodes
	uint32_t count;
	uint32_t capacity;
};

ParameterNode *get_nexts(ParameterNode *node) noexcept{
	return (ParameterNode *)(node->values + node->capacity);
}

enum class ParamType : uint8_t{
	Var, Const, Auto
};


// information about unresolved class
struct UnresolvedProcedure{
	uint8_t param_count;
	uint8_t min_param_count;
	bool is_inline;
	Node *param_classes[MaxParamCount];
	Node *param_defaults[MaxParamCount];
	ParamType param_type[MaxParamCount];
	ParameterNode implementations;

	uint32_t *param_names;
	Node *return_class;
	Node *body;

	// implementations is tree structure that holds arguments in binary format
	// default arguments will be always put at frst index at instatiotion time

	// Parameters to class must be trivially comparable
};

proc :: (A :: ?TA/Array, B :: TA = TA~123) -> Int {
	
}




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
		if (iclass_id != curr_param->class_id)
			RAISE_ERROR("parameter cannot be matched, because it's class dont match");
		
		if (iclass_id <= (ClassId)BasicClass::String || (iclass_id>>ClassInstanceIdBits) != 0){
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
			if ((iclass_id >> ClassInstanceIdBits) & 1 == 1){
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
		
		[[unlikely]] if (input[arg_index].class_id != new_param->class_id)
			RAISE_ERROR("arguments's class does not match parameter's class");
		
		*(get_nexts(curr_param) + curr_param->count - 1) = new_param;
		curr_param = new_param;
	}
}



// number of params must match the total number of unresolved class's parameters
static ClassId make_class_implementation(
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

using ParamIndex = uint32_t;
Variable evauate_constant_expression(Varaible *stack, Node *expr) noexcept{


}


using ParamIndex = uint32_t;

Variable get_parameter(ClassID class_id, ParamIndex index) noexcept{
	Varaible result = {BasicClass::Class, .class_id = class_id};
	for (size_t i=1; i<index&0xff; i+=1){
		if (result.class_id == BasicClass::Class){
			// normal class
			if (result.class_id >> == 0){
				result = classes[result.class_id].constants[(index >> i*4) & 0xff];
			} else{

			}
		} else{
			
		}
	}
	return result;
}

ParamIndex get_param_index(Node *expression) noexcept{
	for 
}


bool match(ClassId arg_class_id, Value *class_value) noexcept{
	// HOW MATCHING POLYMORPHIC ARGUMENT WOULD WORK:
	// A - caller's class (always solid)
	// T - calee's class  (not always solid)
	//
	// 1. determine syntax tree location of unresolved values in T
	// 2. try to get values in those locations from A,
	//    if it fails match cannot be found
	// 3. substitute found values into T and evaluate T
	//    if evaluated t was not previously defined match cannot be found
	// 4. compare A and T
	//
	// as class polymorphism can be only 5 levels deep and ther can be max 16 arguments,
	// the synttax tree location can be described by 32 bit number:
	// ________55554444333322221111nnnn
	// nnnn - total depth
	// 1111-5555 - argument index at depth n
	uint32_t deduced[16];
	size_t deduced_count = 0;
	Value *it = class_value;
	for (;;){
		if (it->class_id == BasicClass::Deduced){
			deduced[deduced_count] = 0;
			deduced_count += 1;
		} else if (it->class_id == BasicClass::Parametrized){
			


		}
		
	}
	
}

ProcedureInfo *match_operator(ClassId arg_class, uint8_t operator_id){
	if ((arg_class >> 13) & 1 == 1){

	} else{
		if (classes[arg_type].operators == nullptr) goto DefaultImplementation;
		return classes[arg_type].operators[operator_id];
	}
DefaultImplementation:
	// WITHOUT ANY REQUIREMENTS:
	// assign, equals, not equals,
	
	// WITH REQUIREMENTS
	// assigns, comparisons
}


// modules are shared among all files
// module names with parameters are kept in compile time global hash table
//
// bytecode is generated along evaluation of the syntax tree



void evaluate_module(Span<Node> nodes){
	Span<Variable> globals;
	Span<Variable> locals;
}

array ptr int span hash int string

void evaluate_procedure(Node *nodes){
	Node *it = nodes;
	alignas(uint64_t) uint8_t memory[4096];
	size_t memory_ptr = 0;
	Array<Variable> stack = {};
	Array<Bytenode> bytecode = {};

	for (;;){
		Node curr = *it;
		it += 1;

		switch (curr.type){
		case Node::StringLiteral:
			push_value(stack, Variable{BasicClass::String, .value=curr.index}, al);
			break;
		case Node::Call:{
			Variable arg_value = stack[stack.size-curr.count-1];
			if (arg_value.class_id == BasicClass::Unresolved){
				// 1. check how many parameters it takes
				// 2. find existing or generate new implementation based on parameters
				// 3. insert it in there
				UnresolvedClass *arg_class = unresolved_classes.ptr + arg_value.value;
				[[unlikely]] if (curr.count < arg_class->param_count)
					THROW_ERROR("not enough parameters for class");
				[[unlikely]] if (curr.count > arg_class->param_count)
					THROW_ERROR("too many parameters for class");
				uint8_t *params = beg(memory) + memory_ptr;
				ClassId solid_class = match_parameters(
					stack.ptr + stack.size - curr.count,
					nodes.ptr + arg_class->params_index,
					arg_class,
					curr.count
				);
				stack.ptr -= curr.count;
				stack[stack.size-1].class_id = BasicClass::Class;
				stack[stack.size-1].value.class_id = solid_class;
			} else{
				// typecheck procedure call
				if ((arg_value.class_id >> 13) & 1 != 1){
					// handle things other than procedures
				}

				if (arg_value.value == UINT32_MAX){
					// handle procedure pointer
				}

				match_arguments(
					arg_value.value,
					stack.ptr - curr.count,
				);
			}
		}
		default:
			break;
		}
	}
}
