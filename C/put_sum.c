#include <stdio.h>

int put_sum(int* a, int* b, int* c);

int main()
{
    int a, b, c, sum;
    printf("Enter a: ");
    scanf("%d", &a);

    printf("Enter b: ");
    scanf("%d", &b);

    printf("Enter c: ");
    scanf("%d", &c);

    printf("'a' before changes: %d\n'b' before changes: %d\n'c' before changes: %d\n\n", a, b, c);
    sum = put_sum(&a, &b, &c);
    printf("Sum: %d\n'a' after changes: %d\n'b' after changes: %d\n'c' after changes: %d\n", sum, a, b, c);
    
    return 0;
}

int put_sum(int* a, int* b, int* c)
{
    int sum;
    sum = *a + *b + *c;

    *a = sum;
    *b = sum;
    *c = sum;
    return sum;
}
