# libstub
A stub library for unit testing in pure C code. With `libstub` you can easily create your own stub functions. And `libstub` provides bunch of spy functionalities such as in the modern mock libraries, such as `sinon` in javascript or `Mockito` in Java and etc.

`libstub` is created because I'm an embedded engineer that working as a TDD developer. But I don't find easy usable stub framework for pure `C` environment. 

> googletest and googlemock once were a good option. But they are built for being used with C/C++, hence the targeted compiler is g++, not gcc. So I have to spend a lot of effort in fixing building errors in stubbing functions in my existing code. Actually not mine, it was passed to me.

Because of the language limitation of pure  `C`, it's not so semantic to do with mock. So `libstub` only focus in stubbing. No mocking but a little bit faking. If you are confusing with the terminology outline here (they are always mentioned as 'test doubles'), just google it. And it might be very helpful to look at googletest offical page.

## How to use it

It's very easy to include libstub in you own project. The only thing you need is to include 1 header file. Use this. Make sure `libstub` is in your include directory.

```c
#include "stub.h"
```



## Feature List

`libstub` supports:

- Definine a stub function using a defined macro. Because it's written in the evil macros, you have to use a little bit different macros for function with return value and no return value.
- Set the arbitrary return value of the stubbed function.
- Set side effect of the funciton. A function for bringing side effect always looks like `void (*side_effect)(void)`.
- Set the fake function. The fake function has the same signature of the stubbed function.
- You have a bunch of choices to examine your function invocation. Here is the list:
  - Check if it's called and how many times it's get invoked.
  - Check if it's called with specified arg list.
  - Restore the function to default behaviour - Do nothing and return 0 if return value is needed.
  - Get the arglist of the last call and also get the arglist from a historical call. The default arg log number is 5. You can easily modify it to a customized value by a macro in your source code. 
  - You can verify the content of the pointer arg.
- Support to stub function having up to 6 args.  

## API

All the API's provided by `libstub` is written in macros. So be careful to pass in your args.

**DEFINE_FUNCTION_STUB & DEFINE_FUNCTION_STUB_NO_RETURN**

These 2 macros define the function prototypes and a lot of helper functions and data structure for being used in future.

Args:

- function name
- return type
- how many args in the arg list

Example:

- `DEFINE_FUNCTION_STUB(func, int, 2, int, char)`

  - it defines something like this `int func(int, char)`

- `DEFINE_FUNCTION_STUB_NO_RETURN(func, void, 1, int)`

  - it defines something like `void func(int)`

**CALLED**

This one checks if the function has been called.

Example:

- `CALLED(func)`

**CALL_TIMES**

This one returns how many times the function get invoked.

Example:

- `CALL_TIMES(func)`

**CALL_WITH_ARGS**

This one will check a whole arg list.

```c
func(1, 2, 4);
CALL_WITH_ARGS(1, 2, 4); // true
CALL_WITH_ARGS(1, 2, 3); // false
```



**FETCH_ARG_OF**

Get the arg with index from the arg list.

Args:

- function name
- arg index, the index starts from 1.

Example:

- `FETCH_ARG_OF(func, 1)`

  - This one returns the first arg from the last call.
  - If you pass the index out of the arg list, you will have compile error.

**FETCH_ARG_FROM_CALL**

Get arg from history calls.

Args:

- Function name
- Call index. The index starts from 1. The most arg logs is pre-defined by a macro. If you pass a index value bigger than that, you won't have compile error. `libstub` will follow a cycle rule to loop back to the beginning of the arg buffer. So be careful of what you send down.
- Arg index

Example:

- if the current setting for log number is as default value, which is 5. And then if you have below snippet.

```C
func(1,2) // x
func(1,2) // 1 <===== This is the oldes invocation we can touch.
func(1,2) // 2
func(1,2) // 3
func(1,2) // 4
func(1,2) // 5 <===== Pass index as 5 to access.
FETCH_ARG_FROM_CALL(func, 1, 1) // The oldest one
FETCH_ARG_FROM_CALL(func, 5, 1) // The last one
FETCH_ARG_FROM_CALL(func, 6, 1) // No error but access to the oldest one. 
```

**VERIFY_POINTER_ARG**

Verify the content of the pointer arg passed in. It's implemented by `memcmp`. Before invoking `memcmp`, this macro also check if the function is called and if the points are NULL.

Args:

- A pointer to the expected content.
- function name
- Arg index, starts from 1

Example:

```c

struct {
    int a;
    char b;
} to_compare = {1, 2};
VERIFY_POINTER_ARG(&to_compare, func, 1); // <==== You will get asserted here. Because no call happens yet.
func(NULL);
VERIFY_POINTER_ARG(&to_compare, func, 1); // <==== Still asserted. Because the arg is a NULL pointer.
func(&to_compare);
VERIFY_POINTER_ARG(&to_compare, func, 1); // Finally a smile face.
```

## Extend the arg list

 `libstub` supports to stub function contains up to 6 args. If you want to stub function having even longer arg list. You should be careful here. 

**It's always not a good idea to have args more than 5 in a function.** The longer the arg list is, the more vulnerable the function is. It may be very easy to have to be changed in future because of any minor things changed. 

If you are dealing with some existing code, I just want to say good luck. The good news is, with `libstub`, it's not so difficult to extend the arg list beyond 6.

Look at `helper_macros.h`. You can extend the arg list longer than 6. However keep in mind, having a too long arg list is not a good idea.
