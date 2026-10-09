#include <stdio.h>

void trueque(int *x, int *y)
{
    int tem;

    tem = *x;
    *x = *y;
    *y = tem;
}

int suma(int x)
{
    return (x + x);
}

int main(void)
{
    int A = 3, B = 8;

    printf("\nAntes del trueque: A = %d, B = %d", A, B);
    trueque(&A, &B);
    printf("\nDespués del trueque: A = %d, B = %d", A, B);

    printf("\nsuma(%d) = %d", A, suma(A));

    printf("\n");
    return 0;
}
