#ifndef STUB_H
#define STUB_H

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

struct stub_obj_data {
    bool called;
    unsigned char return_buffer[16];
};
struct stub_obj {
    struct stub_obj_data data;
    bool called;
    unsigned char return_buffer[16];
};

#define FORMAL_ARGS1(a) \
    a arg1
#define FORMAL_ARGS2(a, b) \
    FORMAL_ARGS1(a), b arg2
#define FORMAL_ARGS(n, ...) FORMAL_ARGS##n(__VA_ARGS__)

#define ACTUAL_ARGS1 arg1
#define ACTUAL_ARGS2 arg1, arg2

#define STRUCT_ARGS1(a) a arg1
#define STRUCT_ARGS2(a, b) \
    STRUCT_ARGS1(a); \
    b arg2;
#define STRUCT_ARGS(arg_nr, ...) \
    STRUCT_ARGS##arg_nr(__VA_ARGS__)

#define SAVE_ARG1(p) p.arg1 = arg1;
#define SAVE_ARG2(p) \
    p.arg2 = arg2; \
    SAVE_ARG1(p)
#define SAVE_ARG(p, n) SAVE_ARG##n(p)

#define DEFINE_FUNCTION_STUB(name, return_type, arg_nr, ...) \
    struct name##_args { \
        STRUCT_ARGS(arg_nr, __VA_ARGS__)\
    };\
    struct name##_stub_obj { \
        struct stub_obj common; \
        struct __args { \
            STRUCT_ARGS(arg_nr, __VA_ARGS__); \
        } args;\
    } name##_stub_obj; \
    static void name##_record(int args, FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        pobj->called = true; \
        SAVE_ARG(name##_stub_obj.args, arg_nr); \
    }\
    static void name##_restore(void) \
    { \
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memset(pobj, 0, sizeof(name##_stub_obj)); \
    } \
    static void name##_returns(return_type value) \
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        memcpy(pobj->return_buffer, &value, sizeof(value)); \
    }\
    return_type name(FORMAL_ARGS(arg_nr, __VA_ARGS__))\
    {\
        struct stub_obj* pobj = (struct stub_obj*)(&name##_stub_obj); \
        name##_record(arg_nr, ACTUAL_ARGS##arg_nr); \
        return_type result = *(return_type*)(pobj->return_buffer); \
        return result; \
    }
#define STUB(name) ((struct stub_obj*)&name##_stub_obj)

#define FETCH_ARG_OF(name, idx, stub_obj) \
    name##_stub_obj.args.arg##idx

#define SET_RETURN_OF(name, value) \
    name##_returns(value);

#define RESTORE(name) name##_restore()
#endif
