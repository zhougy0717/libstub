#ifndef STUB_H
#define STUB_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "helper_macros.h"

#define VERSION     "v1.0"
struct stub_obj {
    bool called;
    int call_times;
    unsigned char return_buffer[16];
    void (*side_effect)(void);
};

#ifndef MAX_CALL_ARG_LOGS
#define MAX_CALL_ARG_LOGS 5
#endif

#define MAKE_FORMAL_ARG0()
#define MAKE_FORMAL_ARG1(a)         a arg1
#define MAKE_FORMAL_ARG(a, n)       ,a arg##n

#define MAKE_ACTUAL_ARG0()
#define MAKE_ACTUAL_ARG1(a)         arg1
#define MAKE_ACTUAL_ARG(a, n)       ,arg##n

#define MAKE_STRUCT_ARG0()
#define MAKE_STRUCT_ARG1(a)         a arg1;
#define MAKE_STRUCT_ARG(a, n)       a arg##n;

#define MAKE_SAVE_ARG0(a)
#define MAKE_SAVE_ARG1(a)           memcpy(&pargs->arg1, &arg1, sizeof(arg1));
#define MAKE_SAVE_ARG(a, n)         memcpy(&pargs->arg1, &arg1, sizeof(arg1));pargs->arg##n = arg##n;

#define MAKE_VERIFY_ARG0(a)         true
#define MAKE_VERIFY_ARG1(a)         (0 == memcmp(&pargs->arg1, &arg1, sizeof(arg1)))
#define MAKE_VERIFY_ARG(a, n)       && (0 == memcmp(&pargs->arg##n, &arg##n, sizeof(arg##n)))

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
    struct __##name##_stub_obj { \
        struct stub_obj common_data; \
        struct name##_args args[MAX_CALL_ARG_LOGS];\
        return_type (*fake)(FORMAL_ARGS(arg_nr, __VA_ARGS__)); \
    } name##_stub_obj; \
    static void name##_record(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->called = true; \
        int call_times = name##_stub_obj.common_data.call_times; \
        struct name##_args* pargs = &name##_stub_obj.args[call_times % MAX_CALL_ARG_LOGS]; \
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
        struct name##_args* pargs = &name##_stub_obj.args[call_times % MAX_CALL_ARG_LOGS]; \
        return VERIFY_ARGS(arg_nr, __VA_ARGS__); \
    } \
    static void name##_set_fake(return_type (*fake)(FORMAL_ARGS(arg_nr, __VA_ARGS__))) \
    { \
        name##_stub_obj.fake = fake; \
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
        if (name##_stub_obj.fake != NULL) \
        { \
            return name##_stub_obj.fake(ACTUAL_ARGS(arg_nr, __VA_ARGS__)); \
        } \
        return_type result = *(return_type*)(pobj->return_buffer); \
        return result; \
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
        if (name##_stub_obj.fake != NULL) \
        { \
            return name##_stub_obj.fake(ACTUAL_ARGS(arg_nr, __VA_ARGS__)); \
        } \
    }

#define FETCH_ARG_OF(name, idx) \
    name##_stub_obj.args[name##_stub_obj.common_data.call_times % MAX_CALL_ARG_LOGS].arg##idx

#define SET_RETURN_OF(name, value) name##_returns(value);
#define SET_FAKE_OF(name, pfunc) name##_set_fake(pfunc);

#define RESTORE(name) name##_restore()
#define CALLED(name) name##_stub_obj.common_data.called
#define CALL_TIMES(name) name##_stub_obj.common_data.call_times
#define SET_SIDE_EFFECT_OF(name, func) name##_stub_obj.common_data.side_effect = func
#define CALL_WITH_ARGS(name, ...) name##_verify_args(__VA_ARGS__)
#define FETCH_ARG_FROM_CALL(name, call, idx) \
    { \
        assert(idx <= MAX_CALL_ARG_LOGS); \
        name##_stub_obj.args[call % MAX_CALL_ARG_LOGS].arg##idx; \
    }
#define VERIFY_POINTER_ARG(compare, name, idx) \
{ \
    assert(CALLED(name)); \
    assert(compare != NULL); \
    assert(FETCH_ARG_OF(name, idx) != NULL); \
    memcmp(compare, FETCH_ARG_OF(name, idx), sizeof(*(compare))); \
}
#endif
