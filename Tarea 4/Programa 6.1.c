#include <stdio.h>

/* Diagonal principal.
   Recibe como dato una matriz cuadrada de tipo entero y escribe
   la diagonal principal. */

#define TAM 10

/* Prototipos de funciones */
void Lectura(int [][TAM], int);
void Imprime(int [][TAM], int);
/* Siempre es necesario declarar el número de columnas; si no, el
   compilador marca un error de sintaxis. */

int main(void)
{
    int MAT[TAM][TAM];

    Lectura(MAT, TAM);
    Imprime(MAT, TAM);
    printf("\n");

    return 0;
}

/* Lee una matriz cuadrada de F filas y F columnas */
void Lectura(int A[][TAM], int F)
{
    int I, J;
    for (I = 0; I < F; I++)
        for (J = 0; J < F; J++)
        {
            printf("Ingrese el elemento %d %d: ", I + 1, J + 1);
            scanf("%d", &A[I][J]);
        }
}

/* Escribe la diagonal principal de una matriz cuadrada de F filas y columnas */
void Imprime(int A[][TAM], int F)
{
    int I, J;
    for (I = 0; I < F; I++)
        for (J = 0; J < F; J++)     /* El libro usa J < TAM; aquí da lo mismo */
            if (I == J)
                printf("\nDiagonal %d %d: %d ", I, J, A[I][J]);
}
