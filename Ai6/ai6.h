#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


/* array_1d */
typedef struct
{
  float* data;
  int length; 
} Array_1d;

Array_1d* array_1d_create(int length);
void array_1d_free(Array_1d* arr);
void array_1d_assign_range(Array_1d* arr);
void array_1d_assign_random(Array_1d* arr);
float array_1d_get(Array_1d* arr, int index);
void array_1d_set(Array_1d* arr, int index, float new_value);
void array_1d_resize(Array_1d* arr, int new_length);
void array_1d_print(Array_1d* arr);

void array_1d_scalar_add(Array_1d* arr, float x);
void array_1d_scalar_subtract(Array_1d* arr, float x);
void array_1d_scalar_multiply(Array_1d* arr, float x);
void array_1d_scalar_divide(Array_1d* arr, float x);
Array_1d* array_1d_vector_muliply(Array_1d* arr1, Array_1d* arr2);
Array_1d* array_1d_vector_divide(Array_1d* arr1, Array_1d* arr2);
float array_1d_sum(Array_1d* arr);


/* array_2d */
typedef struct
{
  float** data;
  int dim1;
  int dim2;
  int length; 
} Array_2d;

Array_2d* array_2d_create(int dim1, int dim2);
void array_2d_free(Array_2d* arr);
void array_2d_increase_dim1(Array_2d* arr, int num_new_rows);
void array_2d_increase_dim2(Array_2d* arr, int num_new_columns);
void array_2d_assign_range(Array_2d* arr);
void array_2d_assign_random(Array_2d* arr);
void array_2d_set_all(Array_2d* arr, float value);

void array_2d_print(Array_2d* arr);
Array_1d* array_2d_row_get(Array_2d* arr, int row);
Array_1d* array_2d_column_get(Array_2d* arr, int column);
Array_2d* array_2d_rows_get(Array_2d* arr, Array_1d* rows);
void array_2d_row_set(Array_2d* arr, int row, int new_value);
void array_2d_column_set(Array_2d* arr, int column, int new_value);

void array_2d_scalar_add(Array_2d* arr, float x);
void array_2d_scalar_subtract(Array_2d* arr, float x);
void array_2d_scalar_multiply(Array_2d* arr, float x);
void array_2d_scalar_divide(Array_2d* arr, float x);
Array_2d* array_2d_vector_muliply(Array_2d* arr1, Array_2d* arr2);
Array_2d* array_2d_vector_divide(Array_2d* arr1, Array_2d* arr2);
float array_2d_sum(Array_2d* arr);


/* array_list */
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


/* array_queue */
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


/* array_reverse_stack*/
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


/* quick_sort */
void swap(float* x, float* y);
int partition(float* array, int low, int high);
void quicksort_recursion(float* array, int low, int high);
void quicksort(float* array, int length);


/* quick_arg_sort */
void swap_float(float* x, float* y);
int quick_arg_sort_partition(float* values_array, float* index_array, int low, int high);
void quick_arg_sort_recursion(float* values_array, float* index_array, int low, int high);
void quick_arg_sort(float* values_array, float* index_array, int length);