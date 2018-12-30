#ifndef STUB_H
#define STUB_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

struct stub_obj {
    bool called;
    int call_times;
    unsigned char return_buffer[16];
    void (*side_effect)(void);
};

#define MAX_CALL_TIMES 5
#define LAST_CALL_ARGS(name) \
    name##_stub_obj.args[name##_stub_obj.common_data.call_times % MAX_CALL_TIMES]

#define LAST1(a, ...) a
#define LAST2(a, ...) LAST1(__VA_ARGS__)
#define LAST3(a, ...) LAST2(__VA_ARGS__)
#define LAST4(a, ...) LAST3(__VA_ARGS__)
#define LAST5(a, ...) LAST4(__VA_ARGS__)
#define LAST6(a, ...) LAST5(__VA_ARGS__)

#define FIRST1(a, ...)                  a
#define FIRST2(a, b, ...)               a, b
#define FIRST3(a, b, c, ...)            a, b, c
#define FIRST4(a, b, c, d, ...)         a, b, c, d
#define FIRST5(a, b, c, d, e, ...)      a, b, c, d, e
#define FIRST6(a, b, c, d, e, f, ...)   a, b, c, d, e, f

#define FOR_EACH0(MAKE, MAKE0, MAKE1, ...) \
    MAKE0()
#define FOR_EACH1(MAKE, MAKE0, MAKE1, a, ...) \
    MAKE1(a)
#define FOR_EACH2(MAKE, MAKE0, MAKE1, ...) \
    FOR_EACH1(MAKE, MAKE0, MAKE1, FIRST1(__VA_ARGS__)) MAKE(LAST2(__VA_ARGS__), 2)
#define FOR_EACH3(MAKE, MAKE0, MAKE1, ...) \
    FOR_EACH2(MAKE, MAKE0, MAKE1, FIRST2(__VA_ARGS__)) MAKE(LAST3(__VA_ARGS__), 3)
#define FOR_EACH4(MAKE, MAKE0, MAKE1, ...) \
    FOR_EACH3(MAKE, MAKE0, MAKE1, FIRST3(__VA_ARGS__)) MAKE(LAST4(__VA_ARGS__), 4)
#define FOR_EACH5(MAKE, MAKE0, MAKE1, ...) \
    FOR_EACH4(MAKE, MAKE0, MAKE1, FIRST4(__VA_ARGS__)) MAKE(LAST5(__VA_ARGS__), 5)
#define FOR_EACH6(MAKE, MAKE0, MAKE1, ...) \
    FOR_EACH5(MAKE, MAKE0, MAKE1, FIRST5(__VA_ARGS__)) MAKE(LAST6(__VA_ARGS__), 6)
#define FOR_EACH(nr, MAKE, MAKE0, ...) FOR_EACH##nr(MAKE, MAKE0, __VA_ARGS__)

#define MAKE_FORMAL_ARG0()          void
#define MAKE_FORMAL_ARG1(a)         a arg1
#define MAKE_FORMAL_ARG(a, n)       ,a arg##n

#define MAKE_ACTUAL_ARG0()
#define MAKE_ACTUAL_ARG1(a)         arg1
#define MAKE_ACTUAL_ARG(a, n)       ,arg##n

#define MAKE_STRUCT_ARG0()          int place_holder;
#define MAKE_STRUCT_ARG1(a)         a arg1;
#define MAKE_STRUCT_ARG(a, n)       a arg##n;

#define MAKE_SAVE_ARG0(a)
#define MAKE_SAVE_ARG1(a)           pargs->arg1 = arg1;
#define MAKE_SAVE_ARG(a, n)         pargs->arg##n = arg##n;

#define MAKE_VERIFY_ARG0(a)         true
#define MAKE_VERIFY_ARG1(a)         (pargs->arg1 == arg1)
#define MAKE_VERIFY_ARG(a, n)       && (pargs->arg##n = arg##n)

#define FORMAL_ARGS(nr, ...) \
    FOR_EACH##nr(MAKE_FORMAL_ARG, MAKE_FORMAL_ARG0, MAKE_FORMAL_ARG1, __VA_ARGS__)
#define ACTUAL_ARGS(nr, ...) \
    FOR_EACH##nr(MAKE_ACTUAL_ARG, MAKE_ACTUAL_ARG0, MAKE_ACTUAL_ARG1, __VA_ARGS__)
#define STRUCT_ARGS(nr, ...) \
    FOR_EACH##nr(MAKE_STRUCT_ARG, MAKE_STRUCT_ARG0, MAKE_STRUCT_ARG1, __VA_ARGS__)
#define SAVE_ARGS(nr, name, ...) \
    FOR_EACH##nr(MAKE_SAVE_ARG, MAKE_SAVE_ARG0, MAKE_SAVE_ARG1, __VA_ARGS__)
#define VERIFY_ARGS(nr, name, ...) \
    FOR_EACH##nr(MAKE_VERIFY_ARG, MAKE_VERIFY_ARG0, MAKE_VERIFY_ARG1, __VA_ARGS__)


#define DEFINE_FUNCTION_STUB_CORE(name, return_type, arg_nr, ...) \
    struct name##_args { \
        STRUCT_ARGS(arg_nr, __VA_ARGS__)\
    };\
    struct name##_stub_obj { \
        struct stub_obj common_data; \
        struct name##_args args[MAX_CALL_TIMES];\
    } name##_stub_obj; \
    static void name##_record(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->called = true; \
        int call_times = name##_stub_obj.common_data.call_times; \
        struct name##_args* pargs = &name##_stub_obj.args[call_times % MAX_CALL_TIMES]; \
        SAVE_ARGS(arg_nr, __VA_ARGS__); \
    }\
    static void name##_restore(void) \
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memset(pobj, 0, sizeof(name##_stub_obj)); \
    } \
    static bool name##_verify_args(FORMAL_ARGS(arg_nr, __VA_ARGS__)) \
    {\
        int call_times = name##_stub_obj.common_data.call_times; \
        struct name##_args* pargs = &name##_stub_obj.args[call_times % MAX_CALL_TIMES]; \
        return VERIFY_ARGS(arg_nr, __VA_ARGS__); \
    }

#define DEFINE_FUNCTION_STUB(name, return_type, arg_nr, ...) \
    DEFINE_FUNCTION_STUB_CORE(name, return_type, arg_nr, __VA_ARGS__)\
    static void name##_returns(return_type value) \
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memcpy(pobj->return_buffer, &value, sizeof(value)); \
    }\
    return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->call_times ++; \
        name##_record(ACTUAL_ARGS(arg_nr, __VA_ARGS__)); \
        if (pobj->side_effect != NULL) \
        { \
            pobj->side_effect(); \
        } \
        return (return_type)(*(return_type*)(pobj->return_buffer)); \
    }

#define DEFINE_FUNCTION_STUB_NO_RETURN(name, return_type, arg_nr, ...) \
    DEFINE_FUNCTION_STUB_CORE(name, return_type, arg_nr, __VA_ARGS__)\
    return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->call_times ++; \
        name##_record(ACTUAL_ARGS(arg_nr, __VA_ARGS__)); \
        if (pobj->side_effect != NULL) \
        { \
            pobj->side_effect(); \
        } \
    }

#define FETCH_ARG_OF(name, idx) \
    name##_stub_obj.args[name##_stub_obj.common_data.call_times % MAX_CALL_TIMES].arg##idx

#define SET_RETURN_OF(name, value) name##_returns(value);

#define RESTORE(name) name##_restore()
#define CALLED(name) name##_stub_obj.common_data.called
#define CALL_TIMES(name) name##_stub_obj.common_data.call_times
#define SET_SIDE_EFFECT_OF(name, func) name##_stub_obj.common_data.side_effect = func
#define CALL_WITH_ARGS(name, ...) name##_verify_args(__VA_ARGS__)
#define FETCH_ARG_FROM_CALL(name, call, idx) \
    { \
        assert(idx <= MAX_CALL_TIMES); \
        name##_stub_obj.args[call % MAX_CALL_TIMES].arg##idx; \
    }
#define VERIFY_POINTER_ARG(compare, name, idx) \
{ \
    assert(CALLED(name)); \
    assert(compare != NULL); \
    memcmp(compare, FETCH_ARG_OF(name, idx), sizeof(*(compare))); \
}
#endif
