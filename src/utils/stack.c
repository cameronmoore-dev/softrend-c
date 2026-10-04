#include "utils/stack.h"

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "defines.h"

typedef struct stack
{
    u32 count;
    u32 type_size;
} stack_;

stack_ *stack_create(u32 type_size)
{
    stack_ *s = malloc(sizeof(stack_));
    s->count = 0;
    s->type_size = type_size;
    return s;
}

void stack_close(stack_ *stack)
{
    stack_ **top = stack_get_top(stack);

    // The beginning of the stacked elements.
    // Starts sizeof(stack_) after the base stack_ pointer
    stack_ **bot = (stack_ **)(stack + 1); 
    
    while (top != bot)
    {
        // Free the pointer to the element's data
        free(*top);
        stack->count--;
        top = stack_get_top(stack);
    }

    free(stack);
}

void stack_push(stack_ *stack, void *elem)
{
    u32 buf_size = sizeof(stack_) + sizeof(uintptr_t) * ++stack->count;
    void *tmp = realloc(stack, buf_size);
    if (tmp)
    {
        stack = tmp;

        // The head of the stack 
        // points to a pointer that points to the element's data
        stack_ **new_top = stack_get_top(stack);

        // Create enough space on the heap for a copy of the element
        void *e = malloc(stack->type_size);
        
        // Copy the element's data into the new allocation
        memcpy(e, elem, stack->type_size);

        // Set the pointer of the newly allocated element 
        // as the value that the top of the stack points to
        *new_top = e;
    }
}

void stack_pop(stack_ *stack)
{
    if (stack->count > 0)
    {
        u32 buf_size = sizeof(stack_) + sizeof(uintptr_t) * --stack->count;
        void *tmp = realloc(stack, buf_size);
        if (tmp)
        {
            stack = tmp;
        }
    }
}

stack_ **stack_get_top(stack_ *stack)
{
    if (stack->count < 1)
    {
        return NULL;
    }

    return (stack_ **)((stack + 1) + ((stack->count - 1) * sizeof(uintptr_t)));
}

u32 stack_get_count(stack_ *stack)
{
    return stack->count;
}