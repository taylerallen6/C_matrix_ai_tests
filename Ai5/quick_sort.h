#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void swap(float* x, float* y);
int partition(float* array, int low, int high);
void quicksort_recursion(float* array, int low, int high);
void quicksort(float* array, int length);