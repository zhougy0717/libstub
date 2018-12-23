#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "stub.h"
int sub(int a, int b)
{
    return a-b;
}

DEFINE_FUNCTION_STUB(func, int, 2, int, char);

START_TEST(test_stub_called) {
    struct stub_obj* func_stub = STUB(func);
    func(0, 0);
    ck_assert(CALLED(func));
}
END_TEST
START_TEST(test_stub_restore) {
    struct stub_obj* func_stub = STUB(func);
    func(0, 0);
    RESTORE(func);
    ck_assert(!CALLED(func));
    func(0, 0);
    RESTORE(func);
    ck_assert(!CALLED(func));
}
END_TEST
START_TEST(test_extract_args) {
    struct stub_obj* func_stub = STUB(func);
    func(1, 3);
    ck_assert_int_eq(1, FETCH_ARG_OF(func, 1, func_stub));
    ck_assert_int_eq(3, FETCH_ARG_OF(func, 2, func_stub));
}
END_TEST
START_TEST(test_returns) {
    struct stub_obj* func_stub = STUB(func);
    SET_RETURN_OF(func, 123);
    ck_assert_int_eq(123, func(0,1));
    ck_assert_int_eq(123, func(2,3));
}
END_TEST

START_TEST(test_call_times) {
    struct stub_obj* func_stub = STUB(func);
    func(1,2);
    ck_assert_int_eq(1, CALL_TIMES(func));

    func(2,3);
    ck_assert_int_eq(2, CALL_TIMES(func));
}
END_TEST
Suite * make_sub_suite(void) {
    Suite *s = suite_create("stub testing");       // 建立Suite
    TCase *tc_sub = tcase_create("test function stubs");  // 建立测试用例集
    suite_add_tcase(s, tc_sub);           // 将测试用例加到Suite中
    tcase_add_test(tc_sub, test_stub_called);
    tcase_add_test(tc_sub, test_stub_restore);
    tcase_add_test(tc_sub, test_extract_args);
    tcase_add_test(tc_sub, test_call_times);

    return s;
}

int main(void) {
    int n, n1;
    SRunner *sr, *sr1;
    sr = srunner_create(make_sub_suite()); // 将Suite加入到SRunner
    srunner_run_all(sr, CK_NORMAL);
    n = srunner_ntests_failed(sr);         // 运行所有测试用例
    srunner_free(sr);
    return (n == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
