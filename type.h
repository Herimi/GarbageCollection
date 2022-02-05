//
// Created by Herimi
//

#ifndef DEFINE_TYPE_H
#define DEFINE_TYPE_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/**
 * @param TYPE 结构体类型
 * @param MEMBER_TYPE 结构体成员类别
 * @param MEMBER_NAME 结构体成员名称
 */
#define FIELD(TYPE, MEMBER_TYPE, MEMBER_NAME) {NULL, offsetof(TYPE, MEMBER_NAME), &init_class_##MEMBER_TYPE}
#define SIZEOF_ARR(ARR) (sizeof(ARR) / sizeof(ARR[0]))
#define FIELDS(args...) {args}
#define STRUCT(TYPE, X) \
typedef struct TYPE {\
Class *class;\
X                       \
} TYPE;                 \
Class* init_class_##TYPE();

#define INIT_STRUCT(TYPE, ARGS...) \
static bool class_##TYPE##_init = false; \
static Field class_##TYPE##_fields[] = FIELDS(FIELD(Test, NULL, class), ARGS); \
static Class class_##TYPE = {#TYPE, sizeof(TYPE), SIZEOF_ARR(class_##TYPE##_fields), class_##TYPE##_fields}; \
Class* init_class_##TYPE() {                            \
    if (!class_##TYPE##_init) {                     \
        class_##TYPE##_init = true;                 \
        init_class_field(&class_##TYPE);            \
    }                                               \
    return &class_##TYPE;                           \
}

#define GET_CLASS(TYPE) init_class_##TYPE()

typedef struct Field {
    struct Class *class;
    int offset;

    // 用于促使华class字段的方法
    struct Class *(*get_field_class)();
} Field;
typedef struct Class {
    char *id;
    // 一个对象的大小
    size_t size;
    // 字段数量
    size_t fieldSum;
    // 各个字段
    struct Field *fields;
} Class;
// 用户默认为空的
static bool class_NULL_init = false;
static Class class_NULL = {0};

// 用于默认为空的
Class *init_class_NULL() {
    if (!class_NULL_init) {
        class_NULL_init = true;
    }
    return NULL;
}

// 初始化class中各个属性的class
void *init_class_field(Class *class) {
    // 第0个永远是class类型的所以循环
    for (int i = 1; i < class->fieldSum; ++i) {
        Field *field = class->fields + i;
        if (field->class == NULL) {
            Class *(*get_field_class)() = field->get_field_class;
            if (get_field_class != NULL) {
                field->class = get_field_class();
            }
        }
    }
}

#endif //DEFINE_TYPE_H
