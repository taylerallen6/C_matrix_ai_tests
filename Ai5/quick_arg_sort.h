#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void swap_float(float* x, float* y);
int quick_arg_sort_partition(float* values_array, float* index_array, int low, int high);
void quick_arg_sort_recursion(float* values_array, float* index_array, int low, int high);
void quick_arg_sort(float* values_array, float* index_array, int length);