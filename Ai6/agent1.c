#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "ai6.h"


typedef struct
{
    char data[100];
} Memory_name;

typedef struct
{
    int num_node_slots;
    int num_nodes;
    int recent_active_memory_queue_length;

    Array_list* node_list;
    Array_reverse_stack* available_slot_stack;

    Array_2d* connection_strength_weights;
    Array_2d* connection_order_weights;
    Array_2d* output_strength_weights;

    Array_queue* recent_active_memory_queue;

    Memory_name* memory_names;
} Agent;


Agent* agent_create(int num_node_slots)
{
    Agent* agent = malloc(sizeof(Agent));

    agent->num_node_slots = num_node_slots;
    agent->num_nodes = 0;
    agent->recent_active_memory_queue_length = 4;

    agent->node_list = array_list_create(num_node_slots);
    agent->available_slot_stack = array_reverse_stack_create(agent->node_list->max_length);
    array_reverse_stack_assign_reverse_range(agent->available_slot_stack);

    agent->connection_strength_weights = array_2d_create(num_node_slots, num_node_slots);
    agent->connection_order_weights = array_2d_create(num_node_slots, num_node_slots);
    // array_2d_set_all(agent->connection_order_weights, -1);
    agent->output_strength_weights = array_2d_create(num_node_slots, 128);
    array_2d_assign_random(agent->output_strength_weights);

    agent->recent_active_memory_queue = array_queue_create(10000, agent->recent_active_memory_queue_length);

    agent->memory_names = malloc(sizeof(Memory_name) * num_node_slots);

    return agent;
}

void agent_free(Agent* agent)
{
    array_list_free(agent->node_list);
    array_reverse_stack_free(agent->available_slot_stack);
    array_2d_free(agent->connection_strength_weights);
    array_2d_free(agent->connection_order_weights);
    array_2d_free(agent->output_strength_weights);
    array_queue_free(agent->recent_active_memory_queue);

    free(agent->memory_names);

    free(agent);
}

int agent_node_add(Agent* agent, char name[])
{   
    int node_index = array_reverse_stack_dequeue(agent->available_slot_stack);
    array_list_append(agent->node_list, node_index, node_index);
    array_2d_column_set(agent->connection_strength_weights, node_index, 0);
    array_2d_column_set(agent->connection_order_weights, node_index, -1);

    // strcpy(agent->memory_names[node_index].data, name);

    agent->num_nodes++;

    return node_index;
}

void agent_node_remove(Agent* agent, int node_index)
{
    array_list_remove(agent->node_list, node_index);
    array_reverse_stack_enqueue(agent->available_slot_stack, node_index);
    agent->num_nodes--;

    // array_2d_column_set(agent->connection_strength_weights, node_index, -1);
}

int agent_memory_create(Agent* agent, Array_1d* active_memories, int y)
{
    float update_amount = 1;

    char name[100] = "";
    for (int i = 0; i < active_memories->length-1; i++)
    {   
        int memory_i = active_memories->data[i];
        char* current_name = agent->memory_names[memory_i].data;
        // printf("%s=", current_name);
        strcat(name, current_name);
    }
    // printf("\n name: %s \n", name);

    int new_node_index = agent_node_add(agent, name);

    for (int i = active_memories->length - 4; i < active_memories->length - 1; i++)
    {
        int active_memory_index = active_memories->data[i];
        //// increase active memory toward new memory connection strength.
        agent->connection_strength_weights->data[active_memory_index][new_node_index] = update_amount;
        //// increase new memory toward active memory connection strength.
        // agent->connection_strength_weights->data[new_node_index][active_memory_index] = update_amount;

        agent->connection_order_weights->data[active_memory_index][new_node_index] = (float)i;
    }

    int most_recent_memory_index = active_memories->data[active_memories->length - 1];
    agent->connection_strength_weights->data[new_node_index][most_recent_memory_index] = update_amount;
    agent->output_strength_weights->data[new_node_index][y] = update_amount;

    return new_node_index;
}

Array_1d* agent_active_memories_argsort(Array_1d* rows)
{
    Array_1d* index_arr = array_1d_create(rows->length);
    array_1d_assign_range(index_arr);
    quick_arg_sort(rows->data, index_arr->data, rows->length);

    return index_arr;
}

float absolute_value(float x)
{
    return (float)fabs((double)x);
}

float agent_decrease_strength_by_order_difference(float strength_weight, float order_weight, float current_index_order, float order_effect_divider)
{
    float y;

    if (order_weight == -1)
        return y = strength_weight;
    
    float order_difference = absolute_value(order_weight - current_index_order);
    float bad_order_multiplier = order_difference / (order_effect_divider - 1);
    y = strength_weight * (1 - bad_order_multiplier);

    return y;
}

int agent_next_node_get(Agent* agent, Array_1d* sorted_active_memories, Array_1d* active_memories_sorted_index)
{
    Array_2d* filtered_connection_strength_weights = array_2d_rows_get(agent->connection_strength_weights, sorted_active_memories);
    Array_2d* filtered_connection_order_weights = array_2d_rows_get(agent->connection_order_weights, sorted_active_memories);

    Array_2d* new_filtered_connection_weights = array_2d_create(filtered_connection_strength_weights->dim1, filtered_connection_strength_weights->dim2);

    int num_columns = agent->node_list->current_length + agent->node_list->num_wasted_slots;

    for (int i1 = new_filtered_connection_weights->dim1-4; i1 < new_filtered_connection_weights->dim1; i1++)
    {
        float current_index_order = active_memories_sorted_index->data[i1];

        for (int i2 = 0; i2 < num_columns; i2++)
        {
            float strength_weight = filtered_connection_strength_weights->data[i1][i2];
            float order_weight = filtered_connection_order_weights->data[i1][i2];
            new_filtered_connection_weights->data[i1][i2] = agent_decrease_strength_by_order_difference(strength_weight, order_weight, current_index_order, (float)agent->recent_active_memory_queue_length);
        }
    }
    
    //// free created arrays that are no longer used
    array_2d_free(filtered_connection_strength_weights);
    array_2d_free(filtered_connection_order_weights);
    
    Array_1d* column;
    float sum_value;
    float max_value = -1000000000000.0;
    int max_index = 0;
    for (int i = 0; i < num_columns; i++)
    {   
        if (agent->node_list->data[i] == -1)
            continue;

        column = array_2d_column_get(new_filtered_connection_weights, i);
        sum_value = array_1d_sum(column);

        if (sum_value > max_value)
        {
            max_value = sum_value;
            max_index = i;
        }

        array_1d_free(column);
    }
    
    //// free created arrays that are no longer used
    array_2d_free(new_filtered_connection_weights);

    return max_index;
}

int agent_output_get(Agent* agent, Array_1d* active_memories)
{
    Array_2d* filtered_output_strength_weights = array_2d_rows_get(agent->output_strength_weights, active_memories);
    int num_columns = filtered_output_strength_weights->dim2;
    // printf("num_columns: %i", num_columns);

    Array_1d* column;
    float sum_value;
    float max_value = -1000000000000.0;
    int max_index = 0;
    for (int i = 0; i < num_columns; i++)
    {
        column = array_2d_column_get(filtered_output_strength_weights, i);
        sum_value = array_1d_sum(column);

        if (sum_value > max_value)
        {
            max_value = sum_value;
            max_index = i;
        }

        array_1d_free(column);
    }
    // printf("max_index: %d\n", max_index);
    // printf("max_value: %f\n", max_value);

    //// free created arrays that are no longer used
    array_2d_free(filtered_output_strength_weights);
    int max_output_index = max_index;

    return max_index;
}

void agent_output_weights_correct(Agent* agent, Array_1d* active_memories, int y, int y_pred)
{
    // array_1d_print(active_memories);
    // printf("%i-%i\n", x, y_pred+95);
    printf("%c", y_pred);
    float update_amount;

    bool y_vs_y_pred = (y == y_pred);
    // printf("%d", y_vs_y_pred);

    if (y == y_pred)
        update_amount = 0;
    else
        update_amount = -1;

    int active_memory_index;
    for (int i = 0; i < active_memories->length; i++)
    {
        active_memory_index = (int)active_memories->data[i];
        // printf("%d", active_memory_index);
        agent->output_strength_weights->data[active_memory_index][y_pred] += update_amount;
        agent->output_strength_weights->data[active_memory_index][y] += 1;
    }
}

// void agent_connections_update(Agent* agent, Array_1d* active_memories)
// {
//     for (int i1 = 0; i1 < active_memories->length; i1++)
//     {
//         for (int i2 = i1; i2 < active_memories->length; i2++)
//         {
//             if (i1 == i2)
//                 continue;

//             int connection_i1 = active_memories->data[i1];
//             int connection_i2 = active_memories->data[i2];

//             agent->connection_strength_weights->data[connection_i1][connection_i2] += 100;
//             // agent->connection_strength_weights->data[connection_i2][connection_i1] += 10;

//             //// store order of recent memory.
//             agent->connection_order_weights->data[connection_i1][connection_i2] = (float)i1;
//         }
//     }
// }


int main()
{
    Agent* agent1 = agent_create(10000);
    
    // array_2d_print(agent1->output_strength_weights);

    for (int i = 0; i < 128; i++)
    {
        char name[100];
        name[0] = i;

        agent_node_add(agent1, name);
    }
        

    // char xs[] = "abcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcdeabcde";
    // char xs[] = "acebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebacebaceb";
    // char xs[] = "cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats cats ";
    // char xs[] = "cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog cat dog ";
    // char xs[] = "cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs cats dogs ";
    // char xs[] = "cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car cats car ";
    char xs[] = "cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog cats dog car dog ";
    // char xs[] = "the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. the dog eats food. ";
    // char xs[] = "Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. ";
    // char xs[] = "Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. ";
    // char xs[] = "Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. ";

    // char xs[] = "Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead.";
    
    // char xs[] = "Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead. Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead.Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead.Once upon a time, there was a little mouse named Max. Max was curious and brave, and he wanted to explore the world beyond his tiny mouse hole. One day, Max packed a small bag with some cheese and a few crumbs, and he set out on an adventure. He scampered through fields and forests, and he saw many amazing things along the way. Max saw a big red apple hanging from a tree, and he climbed up to take a bite. He saw a river with fish jumping in the water, and he took a quick dip to cool off. He saw a big, fluffy cloud in the sky, and he imagined it was a giant pillow. As Max journeyed further and further from home, he met new friends like a friendly rabbit who showed him how to find the sweetest carrots, and a wise old owl who taught him how to navigate by the stars. Max also had to watch out for animals that wanted to eat him, like a sneaky cat and a hungry hawk. But Max was quick and clever, and he always found a way to escape danger. Finally, after many weeks of traveling, Max came to a beautiful meadow filled with flowers and tall grass. In the distance, he could see a mountain range with snow-capped peaks. Max felt a sense of awe and wonder at the sight. He realized that even though he was small, he could achieve big things if he had the courage to follow his dreams. And so, Max sat down in the meadow and enjoyed a delicious meal of cheese and crumbs. He looked up at the sky and smiled, feeling grateful for all the adventures he had experienced. As the sun began to set, Max realized it was time to return home. He packed up his bag and started the journey back to his tiny mouse hole. Along the way, Max felt proud of himself for being brave and exploring the world. He knew that there would always be more adventures to come, and he couldn't wait to see what was next. And so, Max returned home safely, with a heart full of excitement and joy for all the possibilities that lay ahead.";



    // char xs[] = "Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once Once upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon upon a a a a a a a a a a a a a a a a a a a a time, time, time, time, time, time, time, time, time, time, time, time, time, time, time, time, time, time, there there there there there there there there there there there there there there there there there there was a was a was a was a was a was a was a was a was a was a was a was a was a was a was a was a was a was a little little little little little little little little little little little little little little little little mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse mouse named named named named named named named named named named named named named named named named named named named named Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Max. Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, Once upon a time, a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was a time, there was was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse was a little mouse mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. mouse named Max. Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Max. Once upon Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. Once upon a time, there was a little mouse named Max. ";

    // printf("xs: %s\n", xs);
    int xs_length = strlen(xs);
    printf("xs_length: %d\n", xs_length);

    //// CONVERT STRINGS TO LOWERCASE FOR SIMPLICITYthe dog eats food. 
    for (int i = 0; xs[i]; i++)
        xs[i] = tolower(xs[i]);

    printf("xs: %s\n", xs);
    printf("\n");

    printf("os: ");
    bool output_updating = false;
    int y_pred;
    Array_1d* active_memories;
    Array_1d* sorted_active_memories;
    Array_1d* active_memories_sorted_index;
    for (int i = 0; xs[i] != 0; i++)
    {
        int x = xs[i];
        // printf("%c", x);

        // printf("y: %c\n", x);
        
        if (output_updating == true)
        {
            //// UPDATE OUTPUT CONNECTIONS
            agent_output_weights_correct(agent1, active_memories, x, y_pred);
            
            //// FREE ARRAYS BEFORE USE IN THIS NEXT ITERATION
            array_1d_free(active_memories);
            array_1d_free(sorted_active_memories);
        }


        array_queue_enqueue(agent1->recent_active_memory_queue, x);

        //// CHECK IF RECENT ACTIVE MEMORY QUEUE IS READY
        if (array_queue_is_full(agent1->recent_active_memory_queue) == false)
        {
            printf("_");
            continue;
        }
        
        //// GET RECENT ACTIVE MEMORIES
        active_memories = array_queue_to_array_1d(agent1->recent_active_memory_queue);
        sorted_active_memories = array_queue_to_array_1d(agent1->recent_active_memory_queue);
        active_memories_sorted_index = agent_active_memories_argsort(sorted_active_memories);

        // printf("active_memories: { ");
        // for (int i = 0; i < active_memories->length; i++)
        // {
        //     int memory_i = active_memories->data[i];
        //     printf("(%s), ", agent1->memory_names[memory_i].data);
        // }
        // printf("}\n");

        // array_1d_print(active_memories);

        //// TRIGGER NEXT ACTIVE NODE
        int max_node_index = agent_next_node_get(agent1, sorted_active_memories, active_memories_sorted_index);
        array_queue_enqueue(agent1->recent_active_memory_queue, max_node_index);

        active_memories = array_queue_to_array_1d(agent1->recent_active_memory_queue);

        // printf("active_memories: { ");
        // for (int i = 0; i < active_memories->length; i++)
        // {
        //     int memory_i = active_memories->data[i];
        //     printf("(%s), ", agent1->memory_names[memory_i].data);
        // }
        // printf("}\n");

        //// GET OUTPUT
        int max_output_index = agent_output_get(agent1, active_memories);

        // agent_connections_update(agent1, active_memories);
        
        if (i < 100000)
        {
            //// CREATE NEW MEMORY
            int new_memory_index = agent_memory_create(agent1, active_memories, x);
            // array_queue_enqueue(agent1->recent_active_memory_queue, new_memory_index);
        }
        

        
        array_1d_free(active_memories_sorted_index);

        // if (max_node_index < 128)
        //     printf("%c", max_node_index);
        // else
        //     printf("-");

        // printf("M(%i)", max_node_index);
        // printf("%c", max_output_index);

        // printf("x: %c\n", x);
        // printf("o: %c\n", max_output_index);

        output_updating = true;
        y_pred = max_output_index;

    }
    printf("\n");

    // array_2d_print(agent1->output_strength_weights);
    // array_2d_print(agent1->connection_strength_weights);


    array_1d_free(active_memories);
    array_1d_free(sorted_active_memories);
    agent_free(agent1);


    return 0;
}