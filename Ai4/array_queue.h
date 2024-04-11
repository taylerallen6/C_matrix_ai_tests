#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
    int* data;
    int max_array_length;
    int max_queue_length;
    int current_length;
    int front;
} Array_queue;


Array_queue* array_queue_create(int max_array_length, int max_queue_length);
void array_queue_free(Array_queue* queue);
bool array_queue_max_length_reached(Array_queue* queue);
bool array_queue_is_full(Array_queue* queue);
bool array_queue_is_empty(Array_queue* queue);
int array_queue_dequeue(Array_queue* queue);
bool array_queue_enqueue(Array_queue* queue, int value);
void array_queue_assign_range(Array_queue* queue);
void array_queue_assign_reverse_range(Array_queue* queue);
void array_queue_print(Array_queue* queue);
void array_queue_print_all(Array_queue* queue);
int array_queue_show_space_left(Array_queue* queue);
Array_queue* array_queue_recreate(Array_queue* queue, int new_max_array_length);
Array_1d* array_queue_to_array_1d(Array_queue* queue);