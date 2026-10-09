#include <stdio.h>

/* Búsqueda binaria. */

#define MAX 100

/* Prototipos de funciones */
void Lectura(int *, int);
int Binaria(int *, int, int);

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

    RES = Binaria(VEC, TAM, ELE);   /* Busca el elemento en el arreglo */

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

/* Búsqueda binaria del elemento E en el arreglo A de T elementos
   (ordenado en forma creciente). Si lo encuentra, regresa la posición
   correspondiente (base 1); en caso contrario, regresa 0. */
int Binaria(int A[], int T, int E)
{
    int IZQ = 0, CEN, DER = T - 1, BAN = 0;

    while ((IZQ <= DER) && (!BAN))
    {
        CEN = (IZQ + DER) / 2;
        if (E == A[CEN])
            BAN = CEN + 1;      /* Posición base 1, así nunca vale 0 si se encontró */
        else if (E > A[CEN])
            IZQ = CEN + 1;
        else
            DER = CEN - 1;
    }
    return (BAN);
}
