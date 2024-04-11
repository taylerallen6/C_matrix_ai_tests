#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
    int* data;
    int total_length;
    int current_length;
    int head;
} Array_reverse_stack;


Array_reverse_stack* array_reverse_stack_create(int total_length);
void array_reverse_stack_free(Array_reverse_stack* stack);
bool array_reverse_stack_is_full(Array_reverse_stack* stack);
bool array_reverse_stack_is_empty(Array_reverse_stack* stack);
bool array_reverse_stack_enqueue(Array_reverse_stack* stack, int value);
int array_reverse_stack_dequeue(Array_reverse_stack* stack);
void array_reverse_stack_assign_range(Array_reverse_stack* stack);
void array_reverse_stack_assign_reverse_range(Array_reverse_stack* stack);
void array_reverse_stack_print(Array_reverse_stack* stack);
void array_reverse_stack_increase_total_length(Array_reverse_stack* stack, int increase_amount);
void array_reverse_stack_increase_total_length_refill_reverse_range(Array_reverse_stack* stack, int increase_amount);