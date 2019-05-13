#ifndef EXTERN_STUB_H
#define EXTERN_STUB_H

#include "stub.h"

#define IMPORT_FUNCTION_STUB_CORE(name, return_type, arg_nr, ...) \
    struct name##_args { \
        STRUCT_ARGS(arg_nr, __VA_ARGS__)\
    };\
    struct __##name##_stub_obj { \
        struct stub_obj common_data; \
        struct name##_args args[MAX_CALL_ARG_LOGS];\
        return_type (*fake)(FORMAL_ARGS(arg_nr, __VA_ARGS__)); \
    }; \
    extern struct __##name##_stub_obj name##_stub_obj; \
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

#define IMPORT_FUNCTION_STUB(name, return_type, arg_nr, ...) \
    IMPORT_FUNCTION_STUB_CORE(name, return_type, arg_nr, __VA_ARGS__)\
    static void name##_returns(return_type value) \
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memcpy(pobj->return_buffer, &value, sizeof(value)); \
    }\
    extern return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__));

#define IMPORT_FUNCTION_STUB_NO_RETURN(name, return_type, arg_nr, ...) \
    IMPORT_FUNCTION_STUB_CORE(name, return_type, arg_nr, __VA_ARGS__)\
    extern return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__));

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
