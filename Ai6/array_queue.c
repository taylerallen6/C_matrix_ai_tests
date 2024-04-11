#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ai6.h"


Array_queue* array_queue_create(int max_array_length, int max_queue_length)
{
    Array_queue* new_queue = malloc(sizeof(Array_queue));
    new_queue->max_array_length = max_array_length;
    new_queue->data = calloc(sizeof(float), new_queue->max_array_length);

    new_queue->max_queue_length = max_queue_length;
    new_queue->current_length = 0;
    new_queue->front = 0;

    return new_queue;
}

void array_queue_free(Array_queue* queue)
{
    free(queue->data);
    free(queue);
}

bool array_queue_max_length_reached(Array_queue* queue)
{
    bool is_queue_full = false;
    
    if (queue->front + queue->current_length == queue->max_array_length)
        is_queue_full = true;
    
    return (is_queue_full);
}

bool array_queue_is_full(Array_queue* queue)
{
    bool is_queue_full = false;
    
    if (queue->current_length == queue->max_queue_length)
        is_queue_full = true;
    
    return (is_queue_full);
}

bool array_queue_is_empty(Array_queue* queue)
{
    bool is_queue_empty = false;
    
    if (queue->current_length == 0)
        is_queue_empty = true;
    
    return (is_queue_empty);
}

int array_queue_dequeue(Array_queue* queue)
{
    int result;

    if (array_queue_is_empty(queue))
    {
        printf("Underflow \n");
        result = 666;
    }
    else
    {
        result = queue->data[queue->front];
        queue->front += 1;
        queue->current_length -= 1;
    }

    return result;
}

bool array_queue_enqueue(Array_queue* queue, int value)
{
    if (array_queue_max_length_reached(queue))
    {
        printf("Overflow \n");
        return false;
    }
    else
    {
        if (array_queue_is_full(queue))
            array_queue_dequeue(queue);
        
        int index = queue->front + queue->current_length;
        queue->data[index] = value;
        queue->current_length += 1;
    }

    return true;
}

void array_queue_assign_range(Array_queue* queue)
{
    for (int i = 0; i < queue->max_queue_length; i++)
        array_queue_enqueue(queue, i);
}

void array_queue_assign_reverse_range(Array_queue* queue)
{
    for (int i = queue->max_queue_length-1; i >= 0; i--)
        array_queue_enqueue(queue, i);
}

void array_queue_print(Array_queue* queue)
{
    for (int i = queue->front; i < queue->front + queue->current_length; i++)
    {
        printf("%d, ", queue->data[i]);
    }
    printf("\n");
}

void array_queue_print_all(Array_queue* queue)
{
    for (int i = 0; i < queue->max_array_length; i++)
    {
        printf("%d, ", queue->data[i]);
    }
    printf("\n");
}

int array_queue_show_space_left(Array_queue* queue)
{
    return queue->max_array_length - (queue->front + queue->current_length);
}

Array_queue* array_queue_recreate(Array_queue* queue, int new_max_array_length)
{
    if (new_max_array_length < queue->max_queue_length)
    {
        printf("WARNING: new_max_array_length: %d is less than current max_queue_length: %d. This with cut the queue short, causing loss of data.\n", new_max_array_length, queue->max_queue_length);
    }
    
    Array_queue* new_queue = array_queue_create(new_max_array_length, queue->max_queue_length);
    new_queue->current_length = queue->current_length;

    for (int i = 0; i < new_queue->current_length; i++)
        new_queue->data[i] = queue->data[i + queue->front];

    array_queue_free(queue);

    return new_queue;
}

Array_1d* array_queue_to_array_1d(Array_queue* queue)
{   
    Array_1d* arr = array_1d_create(queue->current_length);
    
    for (int i = 0; i < queue->current_length; i++)
    {   
        int queue_i = i + queue->front;
        arr->data[i] = queue->data[queue_i];
    }

    return arr;
}



// int main(void)
// {
//     Array_queue* queue1 = array_queue_create(20, 5);

//     for (int i = 0; i < 20; i++)
//     {
//         array_queue_enqueue(queue1, i);
//         printf("queue: ");
//         array_queue_print(queue1);

//         printf("queue all: ");
//         array_queue_print_all(queue1);
//     }

//     printf("\n");
//     printf("\n");

//     printf("free space: %d\n", array_queue_show_space_left(queue1));

//     queue1 = array_queue_recreate(queue1, queue1->max_array_length);
//     printf("queue recreated: ");
//     array_queue_print(queue1);
//     printf("queue all recreated: ");
//     array_queue_print_all(queue1);

//     printf("free space: %d\n", array_queue_show_space_left(queue1));

//     printf("\n");
//     printf("\n");

//     for (int i = 20; i < 30; i++)
//     {
//         array_queue_enqueue(queue1, i);
//         printf("queue: ");
//         array_queue_print(queue1);

//         printf("queue all: ");
//         array_queue_print_all(queue1);
//     }

//     printf("\n");
//     printf("\n");

//     printf("free space: %d\n", array_queue_show_space_left(queue1));

//     queue1 = array_queue_recreate(queue1, queue1->max_array_length + 5);
//     printf("queue recreated: ");
//     array_queue_print(queue1);
//     printf("queue all recreated: ");
//     array_queue_print_all(queue1);

//     printf("free space: %d\n", array_queue_show_space_left(queue1));

//     printf("\n");
//     printf("\n");

//     for (int i = 30; i < 40; i++)
//     {
//         array_queue_enqueue(queue1, i);
//         printf("queue: ");
//         array_queue_print(queue1);

//         printf("queue all: ");
//         array_queue_print_all(queue1);
//     }

//     // array_queue_print(queue1);
//     // printf("\n");

//     // int result;
//     // int current_length = queue1->current_length;
//     // for (int i = 0; i < current_length; i++)
//     // {
//     //     result = array_queue_dequeue(queue1);
//     //     printf("dequed_value: %d\n", result);

//     //     printf("queue: ");
//     //     array_queue_print(queue1);
//     //     printf("\n");
//     // }
    
//     // array_queue_print(queue1);

//     array_queue_free(queue1);

//     return 0;
// }