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

#define FORMAL_ARGS0()
#define FORMAL_ARGS1(a) a arg1
#define FORMAL_ARGS2(a, b) \
    FORMAL_ARGS1(a), b arg2
#define FORMAL_ARGS3(a, b, c) \
    FORMAL_ARGS2(a, b), c arg3
#define FORMAL_ARGS4(a, b, c, d) \
    FORMAL_ARGS3(a, b, c), d arg4
#define FORMAL_ARGS5(a, b, c, d, e) \
    FORMAL_ARGS4(a, b, c, d), e arg5
#define FORMAL_ARGS6(a, b, c, d, e, f) \
    FORMAL_ARGS5(a, b, c, d, e), f arg6
#define FORMAL_ARGS(n, ...) FORMAL_ARGS##n(__VA_ARGS__)

#define ACTUAL_ARGS0
#define ACTUAL_ARGS1 arg1
#define ACTUAL_ARGS2 ACTUAL_ARGS1, arg2
#define ACTUAL_ARGS3 ACTUAL_ARGS2, arg3
#define ACTUAL_ARGS4 ACTUAL_ARGS3, arg4
#define ACTUAL_ARGS5 ACTUAL_ARGS4, arg5
#define ACTUAL_ARGS6 ACTUAL_ARGS5, arg6

#define STRUCT_ARGS0() int place_holder;
#define STRUCT_ARGS1(a) a arg1;
#define STRUCT_ARGS2(a, b) \
    STRUCT_ARGS1(a); \
    b arg2;
#define STRUCT_ARGS3(a, b, c) \
    STRUCT_ARGS2(a, b); \
    c arg3;
#define STRUCT_ARGS4(a, b, c, d) \
    STRUCT_ARGS3(a, b, c); \
    d arg4;
#define STRUCT_ARGS5(a, b, c, d, e) \
    STRUCT_ARGS4(a, b, c, d); \
    e arg5;
#define STRUCT_ARGS6(a, b, c, d, e, f) \
    STRUCT_ARGS5(a, b, c, d, e); \
    f arg6;
#define STRUCT_ARGS(arg_nr, ...) \
    STRUCT_ARGS##arg_nr(__VA_ARGS__)

#define SAVE_ARG0(p)
#define SAVE_ARG1(p) p.arg1 = arg1;
#define SAVE_ARG2(p) \
    p.arg2 = arg2; \
    SAVE_ARG1(p)
#define SAVE_ARG3(p) \
    p.arg3 = arg3; \
    SAVE_ARG2(p)
#define SAVE_ARG4(p) \
    p.arg4 = arg4; \
    SAVE_ARG3(p)
#define SAVE_ARG5(p) \
    p.arg5 = arg5; \
    SAVE_ARG4(p)
#define SAVE_ARG6(p) \
    p.arg6 = arg6; \
    SAVE_ARG5(p)
#define SAVE_ARG(p, n) SAVE_ARG##n(p)

#define VERIFY_ARG0(p) true
#define VERIFY_ARG1(p) (p.arg1 == arg1)
#define VERIFY_ARG2(p) \
    (p.arg2 == arg2) || \
    VERIFY_ARG1(p)
#define VERIFY_ARG3(p) \
    (p.arg3 == arg3) || \
    VERIFY_ARG2(p)
#define VERIFY_ARG4(p) \
    (p.arg4 == arg4) || \
    VERIFY_ARG3(p)
#define VERIFY_ARG5(p) \
    (p.arg5 == arg5) || \
    VERIFY_ARG4(p)
#define VERIFY_ARG6(p) \
    (p.arg6 == arg6) || \
    VERIFY_ARG5(p)
#define VERIFY_ARGS(p, n) VERIFY_ARG##n(p)

#define DEFINE_FUNCTION_STUB_CORE(name, return_type, arg_nr, ...) \
    struct name##_args { \
        STRUCT_ARGS(arg_nr, __VA_ARGS__)\
    };\
    struct name##_stub_obj { \
        struct stub_obj common_data; \
        struct { \
            STRUCT_ARGS(arg_nr, __VA_ARGS__); \
        } args[MAX_CALL_TIMES];\
    } name##_stub_obj; \
    static void name##_record(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->called = true; \
        SAVE_ARG(LAST_CALL_ARGS(name), arg_nr); \
    }\
    static void name##_restore(void) \
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memset(pobj, 0, sizeof(name##_stub_obj)); \
    } \
    static bool name##_verify_args(FORMAL_ARGS(arg_nr, __VA_ARGS__)) \
    {\
        return VERIFY_ARGS(LAST_CALL_ARGS(name), arg_nr); \
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
        name##_record(ACTUAL_ARGS##arg_nr); \
        return_type result = *(return_type*)(pobj->return_buffer); \
        if (pobj->side_effect != NULL) \
        { \
            pobj->side_effect(); \
        } \
        return result; \
    }

#define DEFINE_FUNCTION_STUB_NO_RETURN(name, return_type, arg_nr, ...) \
    DEFINE_FUNCTION_STUB_CORE(name, return_type, arg_nr, __VA_ARGS__)\
    return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->call_times ++; \
        name##_record(ACTUAL_ARGS##arg_nr); \
        if (pobj->side_effect != NULL) \
        { \
            pobj->side_effect(); \
        } \
    }

#define FETCH_ARG_OF(name, idx) LAST_CALL_ARGS(name).arg##idx

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
    (memcmp(compare, FETCH_ARG_OF(name, idx), sizeof(*(compare))))
#endif
