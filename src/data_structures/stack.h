#pragma once

#include "typedefs.h"

typedef struct stack stack_;

stack_ *stack_create(u32 type_size);
void stack_close(stack_ *stack);
void stack_push(stack_ *stack, void *elem);
void stack_pop(stack_ *stack);
stack_ **stack_get_top(stack_ *stack);

u32 stack_get_count(stack_ *stack);