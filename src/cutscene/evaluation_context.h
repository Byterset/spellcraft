#ifndef __CUTSCENE_EVALUATION_CONTEXT_H__
#define __CUTSCENE_EVALUATION_CONTEXT_H__

#define MAX_STACK_SIZE  128

#include <stdint.h>

enum data_type {
    DATA_TYPE_NULL,
    DATA_TYPE_S8,
    DATA_TYPE_S16,
    DATA_TYPE_S32,

    DATA_TYPE_BOOL,

    DATA_TYPE_F32,
};

struct evaluation_context {
    int stack[MAX_STACK_SIZE];
    uint16_t current_stack;
    void* local_varaibles;
    void* global_variables;
};

void evaluation_context_init(struct evaluation_context* context);

void evaluation_context_push(struct evaluation_context* context, int value);
int evaluation_context_pop(struct evaluation_context* context);

int evaluation_context_load(void* data, enum data_type data_type, int word_offset);

#endif