#include "gc.h"

/**
 * 结构体示例 STRUCT(结构体名称, 结构体内容)
 *
 * 所有结构体定义完成后必须定义结构体初始化方法
 * INIT_STRUCT(结构体名称, FIELD(结构体名称, 第一个属性类别, 第一个属性名称), FIELD(结构体名称, 第2个属性类别, 第2个属性名称), .....);
 */
// 下面是例子
/**
 * 定义所有结构体
 */
STRUCT(Test, int a;int b;)
STRUCT(Test1, Test *a;struct Test2 *b;)
STRUCT(Test2, Test1 *a;Test *b;)

/**
 * 定义所有结构体的初始化方法
 */
INIT_STRUCT(Test, FIELD(Test, NULL, a), FIELD(Test, NULL, b));
INIT_STRUCT(Test1, FIELD(Test1, Test, a), FIELD(Test1, Test2, b));
INIT_STRUCT(Test2, FIELD(Test2, Test1, a), FIELD(Test2, Test, b));


/**
 * 所有的方法都必须写成这种格式
 *
 * 返回值 方法名(参数...) METHOD_BODY(
 *      方法中的内功
 * )
 */
void test(Test *test_p, int size) METHOD_BODY(
        for (int i = 0; i < size; ++i) {
            Test1 *p = (Test1 *) NEW(Test1);
            p->a = test_p;
        }
)
int main() METHOD_BODY(
        /**
         * 使用 NEW(结构体名称) 创建对象，他的返回值是一个 void* 指针类型，需要自己强转成需要的类型
         */
        Test *test_p = (Test *) NEW(Test);
        // 第一次gc
        test(test_p, 1023);
        Test1 *test1_p = (Test1 *) NEW(Test1);
        // 第二次gc
        test(test_p, 1022);
        Test2 *test2_p = (Test2 *) NEW(Test2);
        return 0;
)


