#include "gc.h"

STRUCT(Test, int a;int b;)
STRUCT(Test1, Test *a;struct Test2 *b;)
STRUCT(Test2, Test1 *a;Test *b;)
INIT_STRUCT(Test, FIELD(Test, NULL, a), FIELD(Test, NULL, b));
INIT_STRUCT(Test1, FIELD(Test1, Test, a), FIELD(Test1, Test2, b));
INIT_STRUCT(Test2, FIELD(Test2, Test1, a), FIELD(Test2, Test, b));
void test(Test *test_p, int size) METHOD_BODY(
        for (int i = 0; i < size; ++i) {
            Test1 *p = (Test1 *) NEW(Test1);
            p->a = test_p;
        }
)
int main() METHOD_BODY(
        Test *test_p = (Test *) NEW(Test);
        // 第一次gc
        test(test_p, 1023);
        Test1 *test1_p = (Test1 *) NEW(Test1);
        // 第二次gc
        test(test_p, 1022);
        Test2 *test2_p = (Test2 *) NEW(Test2);
        return 0;
)


