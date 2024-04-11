#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void swap_float(float* x, float* y)
{
    float temp = *x;
    *x = *y;
    *y = temp;
}


int quick_arg_sort_partition(float* values_array, float* index_array, int low, int high)
{
    int pivot_index = low + (rand() % (high - low));
    
    if (pivot_index != high)
        swap_float(&values_array[pivot_index], &values_array[high]);
        swap_float(&index_array[pivot_index], &index_array[high]);
    
    float pivot_value = values_array[high];
    int i = low; 
    
    for (int j = low; j < high; j++)
    {
        if (values_array[j] <= pivot_value)
        {
            swap_float(&values_array[i], &values_array[j]);
            swap_float(&index_array[i], &index_array[j]);
            i++;
        }
    }
    
    swap_float(&values_array[i], &values_array[high]);
    swap_float(&index_array[i], &index_array[high]);
    
    return i;
}

void quick_arg_sort_recursion(float* values_array, float* index_array, int low, int high)
{
    if (low < high)
    {
        int pivot_index = quick_arg_sort_partition(values_array, index_array, low, high);
        quick_arg_sort_recursion(values_array, index_array, low, pivot_index - 1);
        quick_arg_sort_recursion(values_array, index_array, pivot_index + 1, high);
    }
}

void quick_arg_sort(float* values_array, float* index_array, int length)
{
    srand(time(NULL));
    quick_arg_sort_recursion(values_array, index_array, 0, length - 1);
}