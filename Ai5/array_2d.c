#include <stdio.h>
#include <stdlib.h>
#include "array_1d.h"


typedef struct
{
  float** data;
  int dim1;
  int dim2;
  int length; 
} Array_2d;


Array_2d* array_2d_create(int dim1, int dim2)
{
    Array_2d* arr = malloc(sizeof(Array_2d));
    arr->length = dim1 * dim2;
    arr->dim1 = dim1;
    arr->dim2 = dim2;

    arr->data = malloc(sizeof(float*) * arr->dim1);

    for (int i = 0; i < dim1; i++)
        arr->data[i] = calloc(sizeof(float), arr->dim2);

    return arr;
}

void array_2d_free(Array_2d* arr)
{
    for (int i = 0; i < arr->dim1; i++)
        free(arr->data[i]);
    
    free(arr->data);
    free(arr);
}

void array_2d_increase_dim1(Array_2d* arr, int num_new_rows)
{
    arr->dim1 += num_new_rows;

    arr->data = realloc(arr->data, sizeof(float*) * arr->dim1);

    for (int i = arr->dim1 - num_new_rows; i < arr->dim1; i++)
        arr->data[i] = calloc(sizeof(float), arr->dim2);
}

void array_2d_increase_dim2(Array_2d* arr, int num_new_columns)
{
    arr->dim2 += num_new_columns;

    for (int i = 0; i < arr->dim1; i++)
        arr->data[i] = realloc(arr->data[i], sizeof(float) * arr->dim2);
}

void array_2d_assign_range(Array_2d* arr)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] = (float)(i1 * arr->dim2 + i2);
        }   
    }
}

void array_2d_assign_random(Array_2d* arr)
{
    float rand_value;

    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            rand_value = (float)rand() / (float)RAND_MAX;
            arr->data[i1][i2] = rand_value;
        }   
    }
}

void array_2d_set_all(Array_2d* arr, float value)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] = value;
        }   
    }
}


void array_2d_print(Array_2d* arr)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        printf("{ ");
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
        printf("%.2f, ", arr->data[i1][i2]);
        }
        printf("}\n");
    }
}

Array_1d* array_2d_row_get(Array_2d* arr, int row)
{
    Array_1d* new_arr = array_1d_create(arr->dim2);

    for (int i = 0; i < arr->dim2; i++)
        new_arr->data[i] = arr->data[row][i];

    return new_arr;
}

Array_1d* array_2d_column_get(Array_2d* arr, int column)
{
    Array_1d* new_arr = array_1d_create(arr->dim1);

    for (int i = 0; i < arr->dim1; i++)
        new_arr->data[i] = arr->data[i][column];

    return new_arr;
}

Array_2d* array_2d_rows_get(Array_2d* arr, Array_1d* rows)
{
    Array_2d* new_arr_2d = array_2d_create(rows->length, arr->dim2);

    for (int i1 = 0; i1 < rows->length; i1++)
    {   
        int arr_i1 = (int)rows->data[i1];
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            new_arr_2d->data[i1][i2] = arr->data[arr_i1][i2];
        }
    }
    
    return new_arr_2d;
}

void array_2d_row_set(Array_2d* arr, int row, int new_value)
{
    for (int i = 0; i < arr->dim2; i++)
        arr->data[row][i] = new_value;
}

void array_2d_column_set(Array_2d* arr, int column, int new_value)
{
    for (int i = 0; i < arr->dim1; i++)
        arr->data[i][column] = new_value;
}


void array_2d_scalar_add(Array_2d* arr, float x)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] += x;
        }   
    }
}

void array_2d_scalar_subtract(Array_2d* arr, float x)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] -= x;
        }   
    }
}

void array_2d_scalar_multiply(Array_2d* arr, float x)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] *= x;
        }   
    }
}

void array_2d_scalar_divide(Array_2d* arr, float x)
{
    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            arr->data[i1][i2] /= x;
        }   
    }
}

Array_2d* array_2d_vector_muliply(Array_2d* arr1, Array_2d* arr2)
{
    Array_2d* new_arr = array_2d_create(arr1->dim1, arr1->dim2);

    for (int i1 = 0; i1 < arr1->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr1->dim2; i2++)
        {
            new_arr->data[i1][i2] = arr1->data[i1][i2] * arr2->data[i1][i2];
        }   
    }

    return new_arr;
}

Array_2d* array_2d_vector_divide(Array_2d* arr1, Array_2d* arr2)
{
    Array_2d* new_arr = array_2d_create(arr1->dim1, arr1->dim2);

    for (int i1 = 0; i1 < arr1->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr1->dim2; i2++)
        {
            new_arr->data[i1][i2] = arr1->data[i1][i2] / arr2->data[i1][i2];
        }   
    }
    
    return new_arr;
}

float array_2d_sum(Array_2d* arr)
{
    float sum_value = 0;

    for (int i1 = 0; i1 < arr->dim1; i1++)
    {
        for (int i2 = 0; i2 < arr->dim2; i2++)
        {
            sum_value += arr->data[i1][i2];
        }   
    }

    return sum_value;
}


// int main(void)
// {
//     Array_2d* arr1 = array_2d_create(4,5);
//     array_2d_print(arr1);
//     printf("\n");

//     array_2d_assign_range(arr1);
//     array_2d_print(arr1);
//     printf("\n");

//     // array_2d_increase_dim1(arr1, 2);
//     // array_2d_assign_range(arr1);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_increase_dim2(arr1, 2);
//     // array_2d_assign_range(arr1);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_scalar_add(arr1, 4.0);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_scalar_subtract(arr1, 3.0);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_scalar_multiply(arr1, 5.0);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_scalar_divide(arr1, 3.0);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // array_2d_assign_random(arr1);
//     // array_2d_print(arr1);
//     // printf("\n");

//     // Array_1d* arr_1d_1 = array_2d_row_get(arr1, 2);
//     // Array_1d* arr_1d_2 = array_2d_column_get(arr1, 2);

//     // array_1d_print(arr_1d_1);
//     // array_1d_print(arr_1d_2);
//     // printf("\n");

//     // array_1d_free(arr_1d_1);
//     // array_1d_free(arr_1d_2);


//     Array_1d* rows = array_1d_create(3);
//     array_1d_set(rows, 0, (float)0);
//     array_1d_set(rows, 1, (float)2);
//     array_1d_set(rows, 2, (float)1);
//     array_1d_print(rows);

//     Array_2d* new_arr_2d = array_2d_rows_get(arr1, rows);
//     array_2d_print(new_arr_2d);

//     array_2d_free(new_arr_2d);
//     array_1d_free(rows);



//     // array_2d_column_set(arr1, 2, 0);
//     // array_2d_row_set(arr1, 2, 0);
//     // array_2d_print(arr1);
//     // printf("\n");



//     // Array_2d* arr2 = array_2d_create(2,3);
//     // array_2d_assign_random(arr2);

//     // Array_2d* arr3 = array_2d_vector_muliply(arr1, arr2);

//     // printf("Matrix Multiply:\n");
//     // printf("array 1:\n");
//     // array_2d_print(arr1);
//     // printf("\n");
//     // printf("array 2:\n");
//     // array_2d_print(arr2);
//     // printf("\n");
//     // printf("results:\n");
//     // array_2d_print(arr3);


//     // printf("\n");
//     // array_2d_assign_random(arr2);

//     // arr3 = array_2d_vector_divide(arr1, arr2);

//     // printf("Matrix Divide:\n");
//     // printf("array 1:\n");
//     // array_2d_print(arr1);
//     // printf("\n");
//     // printf("array 2:\n");
//     // array_2d_print(arr2);
//     // printf("\n");
//     // printf("results:\n");
//     // array_2d_print(arr3);

//     // printf("\n");

//     // array_2d_assign_range(arr1);
//     // printf("array 1:\n");
//     // array_2d_print(arr1);
//     // float sum = array_2d_sum(arr1);
//     // printf("sum: %f\n", sum);

//     // Array_1d* arr_1d = array_1d_create(5);
//     // array_1d_assign_random(arr_1d);
//     // array_1d_print(arr_1d);



//     //   array_1d_free(arr_1d);
//     array_2d_free(arr1);
//     // array_2d_free(arr2);
//     // array_2d_free(arr3);


//     return 0;
// }