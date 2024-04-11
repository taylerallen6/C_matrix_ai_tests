
typedef struct
{
  float* data;
  int length; 
} Array_1d;



Array_1d* array_1d_create(int length);
void array_1d_assign_range(Array_1d* arr);
void array_1d_assign_random(Array_1d* arr);
float array_1d_get(Array_1d* arr, int index);
void array_1d_set(Array_1d* arr, int index, float new_value);
void array_1d_resize(Array_1d* arr, int new_length);
void array_1d_print(Array_1d* arr);
void array_1d_free(Array_1d* arr);

void array_1d_scalar_add(Array_1d* arr, float x);
void array_1d_scalar_subtract(Array_1d* arr, float x);
void array_1d_scalar_multiply(Array_1d* arr, float x);
void array_1d_scalar_divide(Array_1d* arr, float x);
Array_1d* array_1d_vector_muliply(Array_1d* arr1, Array_1d* arr2);
Array_1d* array_1d_vector_divide(Array_1d* arr1, Array_1d* arr2);
float array_1d_sum(Array_1d* arr);