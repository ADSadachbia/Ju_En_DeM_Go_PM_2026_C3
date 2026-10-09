#include <stdio.h>

/* Ordenación por inserción directa. */

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

/* Ordena el arreglo A por el método de inserción directa */
void Ordena(int A[], int T)
{
    int AUX, L, I;
    for (I = 1; I < T; I++)
    {
        AUX = A[I];             /* Elemento a insertar */
        L = I - 1;
        while ((L >= 0) && (AUX < A[L]))
        {
            A[L + 1] = A[L];    /* Se recorre a la derecha el elemento mayor */
            L--;
        }
        A[L + 1] = AUX;         /* Se inserta en su posición */
    }
}
