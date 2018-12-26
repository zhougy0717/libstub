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
    func(0, 0);
    ck_assert(CALLED(func));
}
END_TEST
START_TEST(test_stub_restore) {
    func(0, 0);
    RESTORE(func);
    ck_assert(!CALLED(func));
    func(0, 0);
    RESTORE(func);
    ck_assert(!CALLED(func));
}
END_TEST
START_TEST(test_extract_args) {
    func(1, 3);
    ck_assert_int_eq(1, FETCH_ARG_OF(func, 1));
    ck_assert_int_eq(3, FETCH_ARG_OF(func, 2));
}
END_TEST
START_TEST(test_returns) {
    SET_RETURN_OF(func, 123);
    ck_assert_int_eq(123, func(0,1));
    ck_assert_int_eq(123, func(2,3));
}
END_TEST

START_TEST(test_call_times) {
    func(1,2);
    ck_assert_int_eq(1, CALL_TIMES(func));

    func(2,3);
    ck_assert_int_eq(2, CALL_TIMES(func));
}
END_TEST

static bool side_effect_invoked = false;
static void side_effect(void)
{
    side_effect_invoked = true;
}
START_TEST(test_set_side_effect) {
    SET_SIDE_EFFECT_OF(func, side_effect);
    func(1, 2);
    ck_assert(side_effect_invoked);
}
END_TEST
START_TEST(test_call_with_args) {
    func(10, 20);
    ck_assert(CALL_WITH_ARGS(func, 10, 20));
    ck_assert(!CALL_WITH_ARGS(func, 20, 30));
    // ck_assert(CALL_WITH_ARGS(20)); <----- compile error
}
END_TEST
START_TEST(test_fetch_arg_of_call_time) {
    func(10, 20);
    func(40, 30);
    ck_assert_int_eq(40, FETCH_ARG_FROM_CALL(func, 2, 1));
    // ck_assert_int_eq(40, FETCH_ARG_FROM_CALL(func, MAX_CALL_TIMES, 1)); <----- You will be asserted here.
}
END_TEST

// libstub only supports to stub a function with up to 6 args.
DEFINE_FUNCTION_STUB(long_arglist_func, int, 6, int, char, int, int, int, int);

START_TEST(test_long_arg_list) {
    long_arglist_func(0,0,0,0,0,0);
    ck_assert(CALLED(long_arglist_func));
}
END_TEST

DEFINE_FUNCTION_STUB(no_arg_func, int, 0)
START_TEST(test_no_arg) {
    no_arg_func();
    ck_assert(CALLED(no_arg_func));
}
END_TEST

DEFINE_FUNCTION_STUB_NO_RETURN(void_func, void, 1, int);
START_TEST(test_void_func) {
    void_func(5);
    ck_assert(CALLED(void_func));
    ck_assert_int_eq(5, FETCH_ARG_OF(void_func, 1));
    ck_assert_int_eq(5, FETCH_ARG_FROM_CALL(void_func, 1, 1));
}
END_TEST

struct args{
    int a;
    int b;
};
DEFINE_FUNCTION_STUB(func_with_pointer_arg, int, 1, struct args*);
START_TEST(test_verify_pointer_arg) {
    struct args c = {12345, 6789};
    struct args d = {12345, 6789};
    func_with_pointer_arg(&c);
    ck_assert_int_eq(0, VERIFY_POINTER_ARG(&d, func_with_pointer_arg, 1));
}
END_TEST

DEFINE_FUNCTION_STUB(func_with_void_pointer_arg, int, 1, void*);
START_TEST(test_verify_void_pointer_arg) {
    struct args c = {12345, 6789};
    struct args d = {12345, 6789};
    func_with_void_pointer_arg(&c);
    ck_assert_int_eq(0, VERIFY_POINTER_ARG(&d, func_with_void_pointer_arg, 1));
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
    tcase_add_test(tc_sub, test_set_side_effect);
    tcase_add_test(tc_sub, test_call_with_args);
    tcase_add_test(tc_sub, test_fetch_arg_of_call_time);
    tcase_add_test(tc_sub, test_long_arg_list);
    tcase_add_test(tc_sub, test_no_arg);
    tcase_add_test(tc_sub, test_void_func);
    tcase_add_test(tc_sub, test_verify_pointer_arg);
    tcase_add_test(tc_sub, test_verify_void_pointer_arg);

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
