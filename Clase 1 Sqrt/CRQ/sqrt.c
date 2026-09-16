#include <stdio.h>

int main()
{
    float x = 0.0;
    float b = 0.0;

    printf("Ingrese el numero\n");
    scanf ("%f",&x);
    b = x;
    while (! (b == (x/b)))
    {
     b = 0.5 * ((x/b)+b);

    }
    printf("\nValor Raiz %f",b);
    return 0;
}