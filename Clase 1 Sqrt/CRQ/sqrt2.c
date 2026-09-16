

#include <stdio.h>
#include <math.h>

int main()
{
    float x = 0.0;
    float b = 0.0;

    printf("Ingrese el numero\n");
    scanf("%f", &x);

    b = x;  // valor inicial distinto de cero (asumiendo x != 0)

    while (fabs(b - (x/b)) > 0.0001)
    {
        b = 0.5 * ((x/b) + b);
    }

    printf("\nValor Raiz %f", b);
    return 0;
}//pruebagit