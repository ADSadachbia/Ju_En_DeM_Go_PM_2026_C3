#include <stdio.h>

/* Búsqueda secuencial en arreglos ordenados en forma creciente. */

#define MAX 100

/* Prototipos de funciones */
void Lectura(int *, int);
int Busca(int *, int, int);

int main(void)
{
    int RES, ELE, TAM, VEC[MAX];

    do
    {
        printf("Ingrese el tamaño del arreglo: ");
        scanf("%d", &TAM);
    }
    while (TAM > MAX || TAM < 1);   /* Verifica que el tamaño sea correcto */

    Lectura(VEC, TAM);
    printf("\nIngrese el elemento a buscar: ");
    scanf("%d", &ELE);

    RES = Busca(VEC, TAM, ELE);     /* Busca el elemento en el arreglo */

    if (RES)
        /* Si RES es verdadero (distinto de 0), se escribe la posición */
        printf("\nEl elemento se encuentra en la posición: %d", RES);
    else
        printf("\nEl elemento no se encuentra en el arreglo");
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

/* Localiza el elemento E en el arreglo A (ordenado en forma creciente).
   Si lo encuentra, regresa la posición correspondiente (base 1);
   en caso contrario, regresa 0. */
int Busca(int A[], int T, int E)
{
    int RES, I = 0, BAN = 0;
    while ((I < T) && (E >= A[I]) && !BAN)   /* Condición extra por el orden */
        if (A[I] == E)
            BAN++;
        else
            I++;
    if (BAN)
        RES = I + 1;    /* I+1 porque las posiciones comienzan en cero */
    else
        RES = BAN;
    return (RES);
}
