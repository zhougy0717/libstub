#ifndef HELPER_MACROS_H
#define HELPER_MACROS_H

// The most args a function stub can have is 6 as default.
// If you want to define more than 6 args, 
// Be sure you have added the necessary helper macros: 
//      -LASTx,
//      -FIRSTx
//      -FOR_EACHx
// Just follow the existing routines.
// BE NOTED!!!:
// Without these helpers, you will have a lot of compile errors
// in defining a stub with a too long arg list.
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

#endif
