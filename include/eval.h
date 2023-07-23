#pragma once

#include "classes.h"

// NOTE: when error is found fast forward throught nodes
// till the end of statement or till the #try node is found

Variable error_variable(const char *text, ){
	Variable res = {0};
	res.c.type = ClassType_Error;
}


Variable match_procedure(Variable args, size_t arg_count, ProcedureClassInfo *info){


}


void check_expression(ModulePool *m, ){
	Variable *stack;
	size_t stack_size = 0;
	Node *ast;
	Node *it = ast;

	for (;;){
		Node curr = *it;
		it += 1;
		switch (it->type){
		case Node_Unsigned:
			stack[stack_size] = (Variable){
				.c.type  = Number,
				.f.flags = VarFlag_Static,
				.v       = it->data
			};
			stack_size += 1;
			it += 1;
			break;

		case Node_Add:{
			assert(stack_size >= 2);
			stack_size -= 1;
			Variable lhs = stack[stack_size-1];
			Variable rhs = stack[stack_size-2];
			if (
				lhs.c.type == ClassType_Number && lhs.c.prefixes == 0 &&
				rhs.c.type == ClassType_Number && rhs.c.prefixes == 0
			){
				stack[stack_size-1] = (Varaible){
					.c.type  = ClassType_Number,
					.c.flags = (lhs.f.flags & rhs.f.flags & (VarFlag_Static|VarFlag_Known)),
					.v.u64 = lhs.data.u64 + rhs.data.u64
				};
			}
			assert(false && "addition not suported for this type");
			goto ErrorValue;
		}
		case Node_Call:{
			stack_size -= curr.count;
			if ((curr.flags & NodeFlag_DirectIdentifier) != 0){
				// check if the first argument implements static procedure with matching name
				
			}
			Variable proc = stack[stack_size-1];
			Variable *args = &stack[stack_size];
			if (proc.type != )
		}
		default:
		ErrorValue:
			stack[stack_size-1] = (Varaible){
				.c.type  = ClassType_Error,
				.c.flags,
				.v.debug_text = "eror"
			};
		}
	}
}
