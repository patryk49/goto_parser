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


void check_expression(GlobalInfo *ginfo){
	Variable *stack;
	size_t stack_size = 0;
	Node *ast;
	Node *it = ast;

	size_t infered_count = 0;
	size_t splat_variable_count = 0;

	for (;;){
		Node curr = *it;
		it += 1;
		switch (it->type){
		case Node_Unsigned:
			IrNode ir_node = {
				.type = IT_Data,
				.data_type = IDT_64,
				.flags = IFlag_Static,
				.u64 = it->data.u64
			};
			stack[stack_size] = (Variable){
				.class_id.type = Number,
				.class_id.bits = 64,
				.flags = VarFlag_Static,
				.node_index = 0 // TODO: ir node array
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

		case Node_Pointer:{
			if (stack[stack_size-1].c.type == ClassType_ClassId){	
				ClassId prefixed_class_id = add_class_prefix(
					stack[stack_size-1].v.class_id,
					ClassPrefix_Pointer // TODO: add constant pointer flag
				);
				break;
			}
			// here is the place for adding a "address of" ir node 
			break;
		}

		case Node_D_Class:
			stack[stack_size-1] = (Variable){
				.c.type     = ClassType_ClassId,
				.f.flags    = VarFlag_Static,
				.v.class_id = stack[stack_size-1].c
			};
			break;

		case Node_Colon:
		case Node_Constant:
		case Node_OptionalConstant:
			if (stack[stack_size-1].c.type != ClassType_ClassId){
				RETURN_ERROR(
					"type specification must be of type not a class_id",
					nodes[stack[stack_size].f.node_index].pos
				);
			stack[stack_size] = (Variable){
				.c.type     = ClassType_Decaration,
				.f.flags    = 0,
				.v.class_id = (ClassId){.type=ClassType_ProcPointer, .index=ginfo->proc_pointer_count}
			};


				

		case Node_ProcedureClass:{
			if (curr.count > MAX_PARAM_COUNT)
				RETURN_ERROR("procedure class has too many argumants", curr.pos);
			ProcPoinerClassInfo *new_pp = ginfo->proc_pointers + ginfo->proc_pointer_count;
			Variable *arg_base = stack - curr.count - 1;
			if (arg_base[curr.count].c.type != ClassType_ClassId){
				RETURN_ERROR(
					"return type specification class is not of type class_id",
					nodes[arg_base[i].f.node_index].pos
				);
			}
			*new_pp = (ProcPinterClassInfo){
				.arg_count = curr.count,
				.result_class_id = arg_base[curr.count].v.class_id
			};
			for (size_t i=0; i!=curr.count; i+=1){
				if (arg_base[i].c.type != ClassType_ClassId){
					RETURN_ERROR(
						"parameter class specification is not of type class_id",
						nodes[arg_base[i].f.node_index].pos
					);
				}
				new_pp->arg_class_ids[i] = arg_base[i].v.class_id;
			}
			stack_size -= curr.count;
			stack[stack_size] = (Variable){
				.c.type     = ClassType_ClassId,
				.f.flags    = VarFlag_Static,
				.v.class_id = (ClassId){.type=ClassType_ProcPointer, .index=ginfo->proc_pointer_count}
			};
			ginfo->proc_pointer_count += 1;
			break;
		}
		
		case Node_S_Procedure:{
			if (curr.count > MAX_PARAM_COUNT)
				RETURN_ERROR("procedure class has too many argumants", curr.pos);
			ProcedureClassInfo *new_proc = ginfo->procedures + ginfo->procedure_count;
			bool specified_return_class = (curr.flags & NodeFlag_HasReturnClass) != 0;
			Variable *arg_base = stack - curr.count - specified_return_class;
			*new_proc = (ProcedureClassInfo){
				.arg_count = curr.count,
				.body = it + 1,
			};
			for (size_t i=0; i!=curr.count; i+=1){
				if (arg_base[i].c.type == ClassType_ClassId){
					RETURN_ERROR(
						"parameter class specification is not of type class_id",
						nodes[arg_base[i].f.node_index].pos
					);
				}
				new_proc->arg_class_ids[i] = arg_base[i].v.class_id;
			
			}


			
			

			ginfo->proc_pointer_count += 1;
			break;
		}
		default:
		ErrorValue:
			stack[stack_size-1] = (Varaible){
				.c.type  = ClassType_Error,
				.c.flags,
				.v.debug_text = "error"
			};
		}
	}
}
