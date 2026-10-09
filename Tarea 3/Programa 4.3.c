#include <stdio.h>

/* Conflicto de variables con el mismo nombre. */

void f1(void);   /* Prototipo de función. */

int K = 5;       /* Variable global. */

int main(void)
{
    int I;

    for (I = 1; I <= 3; I++)
        f1();

    printf("\n");
    return 0;
}

void f1(void)
{
    int *globalK = &K;   /* Apunta a la K global (la local aún no existe). */
    int K = 2;           /* Variable local. */

    K += K;
    printf("\n\nEl valor de la variable local es: %d", K);

    *globalK = *globalK + K;   /* Uso de ambas variables. */
    printf("\nEl valor de la variable global es: %d", *globalK);
}
