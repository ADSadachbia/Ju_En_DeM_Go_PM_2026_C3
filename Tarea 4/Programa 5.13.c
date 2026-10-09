#include <stdio.h>

/* Ordenación por selección directa. */

#define MAX 100

/* Prototipos de funciones */
void Lectura(int *, int);
void Ordena(int *, int);
void Imprime(int *, int);

int main(void)
{
    int TAM, VEC[MAX];

    do
    {
        printf("Ingrese el tamaño del arreglo: ");
        scanf("%d", &TAM);
    }
    while (TAM > MAX || TAM < 1);   /* Verifica que el tamaño sea correcto */

    Lectura(VEC, TAM);
    Ordena(VEC, TAM);
    Imprime(VEC, TAM);
    printf("\n");

    return 0;
}

/* Lee un arreglo de T elementos enteros */
void Lectura(int A[], int T)
{
    int I;
    for (I = 0; I < T; I++)
    {
        printf("Ingrese el elemento %d: ", I + 1);
        scanf("%d", &A[I]);
    }
}

/* Escribe un arreglo ordenado de T elementos enteros */
void Imprime(int A[], int T)
{
    int I;
    for (I = 0; I < T; I++)
        printf("\nA[%d]: %d", I, A[I]);
}

/* Ordena el arreglo A por el método de selección directa */
void Ordena(int A[], int T)
{
    int I, J, MEN, L;
    for (I = 0; I < (T - 1); I++)
    {
        MEN = A[I];                 /* Mínimo provisional */
        L = I;                      /* Posición del mínimo */
        for (J = (I + 1); J < T; J++)
            if (A[J] < MEN)
            {
                MEN = A[J];
                L = J;
            }
        A[L] = A[I];                /* El elemento de la posición I pasa a la posición L */
        A[I] = MEN;                 /* El mínimo se coloca en la posición I */
    }
}
