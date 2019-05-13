#include "../stub.h"
#define A(a, ...) int a; B(__VA_ARGS__)
void main()
{
    func(1,3);
    printf("arg1 %d\n", FETCH_ARG(func, 2, func_stub));
}
