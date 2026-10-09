#include <stdio.h>

/* Arreglo sin elementos repetidos.
   Recibe como dato un arreglo unidimensional desordenado de N elementos
   y obtiene ese mismo arreglo pero sin los elementos repetidos. */

#define MAX 100

/* Prototipos de funciones */
void Lectura(int *, int);
void Imprime(int *, int);
void Elimina(int *, int *);
/* En Elimina, el segundo parámetro es por referencia porque el tamaño
   del arreglo puede disminuir. */

int main(void)
{
    int TAM, ARRE[MAX];

    /* do-while para verificar que el tamaño ingresado sea correcto */
    do
    {
        printf("Ingrese el tamaño del arreglo: ");
        scanf("%d", &TAM);
    }
    while (TAM > MAX || TAM < 1);

    Lectura(ARRE, TAM);
    Elimina(ARRE, &TAM);   /* El tamaño se pasa por referencia */
    Imprime(ARRE, TAM);
    printf("\n");

    return 0;
}

/* Lee un arreglo de T elementos enteros */
void Lectura(int A[], int T)
{
    int I;
    printf("\n");
    for (I = 0; I < T; I++)
    {
        printf("Ingrese el elemento %d: ", I + 1);
        scanf("%d", &A[I]);
    }
}

/* Imprime un arreglo de T elementos enteros, sin repeticiones */
void Imprime(int A[], int T)
{
    int I;
    printf("\nArreglo sin elementos repetidos:");
    for (I = 0; I < T; I++)
        printf("\nA[%d]: %d", I + 1, A[I]);
}

/* Elimina los elementos repetidos, conservando la primera aparición */
void Elimina(int A[], int *T)
{
    int I, J, K;
    for (I = 0; I < *T - 1; I++)
    {
        J = I + 1;
        while (J < *T)
        {
            if (A[I] == A[J])
            {
                /* Se recorren los elementos a la izquierda para borrar A[J] */
                for (K = J; K < *T - 1; K++)
                    A[K] = A[K + 1];
                (*T)--;     /* El arreglo disminuye de tamaño */
            }
            else
                J++;        /* Solo avanza si no se eliminó nada */
        }
    }
}
