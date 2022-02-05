//
// Created by Herimi
//

#ifndef DEFINE_VARIABLE_TABLE_H
#define DEFINE_VARIABLE_TABLE_H

#include "stack.h"

#define METHOD_BODY(X) \
{                      \
push_method_var_table(); \
X \
pop_method_var_table();\
}
// 变量表中数据
typedef struct Var {
    Class *class;
    // 对象指针
    void *objectPoint;
} Var;
// 方法的变量表
typedef struct MethodVarTable {
    Var *vars;
    // 逻辑大小
    int varSum;
    // 实际大小
    int size;
} MethodVarTable;
// 创建局部变量表
static Stack VARIABLE_TABLE = {0};

// 插入变量表
void push_method_var_table() {
    MethodVarTable *methodVarTable = calloc(1, sizeof(MethodVarTable));
    push(&VARIABLE_TABLE, methodVarTable);
}

// 删除变量表
void pop_method_var_table() {
    MethodVarTable *methodVarTable = (MethodVarTable *) pop(&VARIABLE_TABLE);
    Var *vars = methodVarTable->vars;
    free(vars);
    free(methodVarTable);
}

// 变量表添加一个变量
void add_var(Class *class, void *objectPoint) {
    MethodVarTable *methodVarTable = (MethodVarTable *) VARIABLE_TABLE.head->element;
    // 容量不够的时候进行扩容
    int size = methodVarTable->size;
    int varSum = methodVarTable->varSum;
    if (size <= varSum) {
        Var *vars = methodVarTable->vars;
        int oldSize = size;
        size = (size << 1) + 16;
        vars = realloc(vars, sizeof(Var) * size);
        int oldOffset = sizeof(Var) * oldSize;
        memset(vars + oldSize, 0, sizeof(Var) * size - oldOffset);
        methodVarTable->size = size;
        methodVarTable->vars = vars;
    }
    // 正常添加
    Var *vars = methodVarTable->vars;
    Var *var = vars + varSum;
    var->class = class;
    var->objectPoint = objectPoint;
    varSum = varSum + 1;
    methodVarTable->varSum = varSum;
}

void (*variable_table_object_handler)(Class *, void *) = NULL;

void scan_handler(void *point) {
    MethodVarTable *methodVarTable = (MethodVarTable *) point;
    Var *vars = methodVarTable->vars;
    for (int i = 0; i < methodVarTable->varSum; ++i) {
        Var *var = vars + i;
        (*variable_table_object_handler)(var->class, var->objectPoint);
    }
}

// 扫表变量表
void scan_variable_table(void (*object_handler)(Class *, void *)) {
    variable_table_object_handler = object_handler;
    forEach(&VARIABLE_TABLE, &scan_handler);
}

#endif //DEFINE_VARIABLE_TABLE_H
