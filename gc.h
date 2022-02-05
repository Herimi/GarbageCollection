//
// Created by Herimi
//

#ifndef DEFINE_GC_H
#define DEFINE_GC_H

#include "type.h"
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include "variable_table.h"

#define OUT_OF_MEMORY_EXCEPTION 1000000

/**
 * GC模块
 */
typedef struct GcAble {
    // 标记算法中的标记
    bool mark;
    // 对象指针
    void *objectPoint;
} GcAble;
bool GC_MARK = true;
// 创建GC列表（此时的值为0，所以所有的deleteAble都默认为false）
static GcAble GC_ARR[1024] = {0};

// 请求GC_ARR分配一块空间
void *gc_alloc(Class *class) {
    for (int i = 0; i < SIZEOF_ARR(GC_ARR); ++i) {
        GcAble gcAble = GC_ARR[i];
        // 找到可用空间，就开始分配对象
        if (gcAble.objectPoint == NULL) {
            size_t size = class->size;
            void *object = calloc(1, size);
            gcAble.objectPoint = object;
            GC_ARR[i] = gcAble;
            return object;
        }
    }
    return NULL;
}

// 扫描到的所有对象都会走进这个方法
void mark_handler(Class *class, void *objectPoint) {
    if (class == NULL || objectPoint == NULL) {
        return;
    }
    for (int i = 0; i < SIZEOF_ARR(GC_ARR); ++i) {
        GcAble gcAble = GC_ARR[i];
        // 找到对应的对象进行标记，被标记过的无序GC
        if (objectPoint == gcAble.objectPoint) {
            // 如果被标记过了，则表示有循环依赖则，需要跳出这个循环依赖
            if (gcAble.mark == GC_MARK) {
                return;
            }
            gcAble.mark = GC_MARK;
            GC_ARR[i] = gcAble;
            break;
        }
    }
    // 第0个永远是class类型的所以不gc
    for (int i = 1; i < class->fieldSum; ++i) {
        Field *field = class->fields + i;
        // 类别
        Class *fieldClass = field->class;
        if (fieldClass == NULL) {
            continue;
        }
        // 指针
        void *fieldObjectPoint = *((void **) (objectPoint + field->offset));
        if (fieldObjectPoint == NULL) {
            continue;
        }
        // 递归处理（深度优先）
        mark_handler(fieldClass, fieldObjectPoint);
    }
}

bool gc_mark() {
    scan_variable_table(&mark_handler);
    // 保证每次标记的标记量都不一样
    GC_MARK = !GC_MARK;
    return true;
}

bool gc_delete() {
    bool flag = false;
    for (int i = 0; i < SIZEOF_ARR(GC_ARR); ++i) {
        GcAble gcAble = GC_ARR[i];
        // 判断是否需要gc
        void *point = gcAble.objectPoint;
        if (point != NULL && gcAble.mark == GC_MARK) {
            flag = true;
            free(point);
            gcAble.objectPoint = NULL;
            gcAble.mark = !GC_MARK;
            GC_ARR[i] = gcAble;
        }
    }
    // 如果一个都没删除，则表示没有空间了，需要抛出异常
    if (!flag) {
        errno = OUT_OF_MEMORY_EXCEPTION;
    }
    return flag;
}

void out_of_memory_exception_handler(int sign) {
    perror("out of memory, could no create an object");
    abort();
}

void gc() {
    // 查找变量表
    // 利用可达性分析算法进行标记
    // 如果gc失败了，则抛出异常
    gc_mark();
    gc_delete();
    if (errno == OUT_OF_MEMORY_EXCEPTION) {
        out_of_memory_exception_handler(OUT_OF_MEMORY_EXCEPTION);
    }
}

#define NEW(TYPE) new_instance(GET_CLASS(TYPE));

typedef struct Object {
    Class *class;
} Object;

void *new_instance(Class *class) {
    void *object = gc_alloc(class);
    // 没有找到则开始进行GC
    if (object == NULL) {
        gc();
        object = gc_alloc(class);
    }
    ((Object *) object)->class = class;
    // 添加变量，做可达性分析扫描
    add_var(class, object);
    return object;
}

#endif //DEFINE_GC_H
