#include <stdio.h>

int get_and_zero(int* n);

int main()
{
    int number, result;
    
    printf("Enter number: ");
    scanf("%d", &number);

    result = get_and_zero(&number);
    printf("New result: %d\nPrevious number: %d\n", result, number);
    return 0;
}

int get_and_zero(int* n)
{
    int temp;
    temp = *n;
    *n = 0; 
    return temp;
}
