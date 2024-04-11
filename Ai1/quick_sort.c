#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void swap(float *x, float *y)
{
    float temp = *x;
    *x = *y;
    *y = temp;
}

int partition(float* array, int low, int high)
{
    int pivot_index = low + (rand() % (high - low));
    
    if (pivot_index != high)
        swap(&array[pivot_index], &array[high]);
    
    float pivot_value = array[high];
    int i = low; 
    
    for (int j = low; j < high; j++)
    {
        if (array[j] <= pivot_value)
        {
            swap(&array[i], &array[j]);
            i++;
        }
    }
    
    swap(&array[i], &array[high]);
    
    return i;
}

void quicksort_recursion(float* array, int low, int high)
{
    if (low < high)
    {
        int pivot_index = partition(array, low, high);
        quicksort_recursion(array, low, pivot_index - 1);
        quicksort_recursion(array, pivot_index + 1, high);
    }
}

void quicksort(float* array, int length)
{
    srand(time(NULL));
    quicksort_recursion(array, 0, length - 1);
}