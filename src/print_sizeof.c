
#include <stdio.h>

int main()
{
#define PRINT_SIZE(type) printf("sizeof(" #type "): %I64u\n", sizeof(type))
    PRINT_SIZE(float);
    PRINT_SIZE(double);
    PRINT_SIZE(long double);
    
    PRINT_SIZE(char);
    PRINT_SIZE(short);
    PRINT_SIZE(int);
    PRINT_SIZE(long int);
    PRINT_SIZE(long long int);
}
