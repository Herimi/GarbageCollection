//
// Created by Herimi
//

#ifndef DEFINE_STACK_H
#define DEFINE_STACK_H

#include <stddef.h>
#include <stdlib.h>

typedef struct StackNode {
    struct StackNode *next;
    void *element;
} StackNode;

typedef struct Stack {
    StackNode *head;
    int size;
} Stack;

Stack *init_stack() {
    Stack *stack = calloc(1, sizeof(Stack));
    return stack;
}

// 头插法
Stack *push(Stack *stack, void *element) {
    StackNode *node = calloc(1, sizeof(StackNode));
    node->element = element;
    node->next = stack->head;
    stack->head = node;
    stack->size = stack->size + 1;
    return stack;
}

void *pop(Stack *stack) {
    StackNode *node = stack->head;
    if (node == NULL) {
        return NULL;
    }
    stack->head = node->next;
    // 删除原有的地址
    void *element = node->element;
//    free(node->element);
    free(node);
    stack->size = stack->size - 1;
    return element;
}

// 遍历
void forEach(Stack *stack, void (*handler)(void *)) {
    StackNode *node = stack->head;
    while (node != NULL) {
        handler(node->element);
        node = node->next;
    }
}

Stack *clear_stack(Stack *stack) {
    while (stack->size > 0) {
        pop(stack);
    }
    return stack;
}

void destroy_stack(Stack *stack) {
    stack = clear_stack(stack);
    free(stack);
}

#endif //DEFINE_STACK_H
