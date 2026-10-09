#include <stdio.h>

/* Producto de vectores.
   Calcula el producto elemento a elemento de dos vectores y almacena
   el resultado en otro arreglo unidimensional. */

#define MAX 10  /* Tamaño de los arreglos */

/* Prototipos de funciones */
void Lectura(int VEC[], int T);
void Imprime(int VEC[], int T);
void Producto(int *X, int *Y, int *Z, int T);

int main(void)
{
    int VE1[MAX], VE2[MAX], VE3[MAX];

    Lectura(VE1, MAX);
    Lectura(VE2, MAX);
    Producto(VE1, VE2, VE3, MAX);

    printf("\nProducto de los Vectores");
    Imprime(VE3, MAX);
    printf("\n");

    return 0;
}

/* Lee un arreglo de T enteros */
void Lectura(int VEC[], int T)
{
    int I;
    printf("\n");
    for (I = 0; I < T; I++)
    {
        printf("Ingrese el elemento %d: ", I + 1);
        scanf("%d", &VEC[I]);
    }
}

/* Imprime un arreglo de T enteros */
void Imprime(int VEC[], int T)
{
    int I;
    for (I = 0; I < T; I++)
        printf("\nVEC[%d]: %d", I + 1, VEC[I]);
}

/* Z[i] = X[i] * Y[i] */
void Producto(int *X, int *Y, int *Z, int T)
{
    int I;
    for (I = 0; I < T; I++)
        Z[I] = X[I] * Y[I];
}
