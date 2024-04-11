#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "../Stacks1/array_stack1.h"


typedef struct
{
    int* data;
    int max_length;
    int current_length;
    int last_item_index;
    int num_wasted_slots;
} Array_list;

Array_list* array_list_create(int max_length);
void array_list_free(Array_list* list);
void array_list_append(Array_list* list, int index, int value);
void array_list_remove(Array_list* list, int index);
void array_list_print(Array_list* list);
void array_list_print_all(Array_list* list);