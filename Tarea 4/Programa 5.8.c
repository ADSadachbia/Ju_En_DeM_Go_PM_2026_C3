#include <stdio.h>

/* Primos.
   Almacena en un arreglo unidimensional los primeros 100 números primos. */

#define TAM 100

/* Prototipos de funciones */
void Imprime(int *, int);
void Primo(int, int *);

int main(void)
{
    int P[TAM] = {1, 2};
    int FLA, J = 2, PRI = 3;

    while (J < TAM)
    {
        FLA = 1;
        Primo(PRI, &FLA);   /* Determina si PRI es primo */
        if (FLA)            /* Si FLA es 1, entonces PRI es primo */
        {
            P[J] = PRI;
            J++;
        }
        PRI += 2;           /* Solo se revisan los impares */
    }
    Imprime(P, TAM);
    printf("\n");

    return 0;
}

/* Determina si A es primo; en ese caso el valor de *B no se altera */
void Primo(int A, int *B)
{
    int DI = 3;
    while (*B && (DI < (A / 2)))
    {
        if ((A % DI) == 0)
            *B = 0;
        DI++;
    }
}

/* Imprime el arreglo de números primos */
void Imprime(int Primos[], int T)
{
    int I;
    for (I = 0; I < T; I++)
        printf("\nPrimos[%d]: %d", I, Primos[I]);
}
