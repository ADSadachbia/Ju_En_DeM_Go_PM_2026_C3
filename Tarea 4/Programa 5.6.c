#include <stdio.h>

/* Suma-cuadrados.
   Calcula la suma del cuadrado de los elementos de un arreglo
   unidimensional de 100 elementos de tipo real. */

#define MAX 100  /* Espacio máximo que ocupará el arreglo */

/* Prototipos de funciones */
void Lectura(float *, int);
double Suma(float *, int);

int main(void)
{
    float VEC[MAX];
    double RES;

    Lectura(VEC, MAX);
    RES = Suma(VEC, MAX);   /* Se almacena el resultado en RES */

    printf("\n\nSuma del arreglo: %.2f", RES);
    printf("\n");

    return 0;
}

/* Lee un arreglo de T elementos reales */
void Lectura(float A[], int T)
{
    int I;
    for (I = 0; I < T; I++)
    {
        printf("Ingrese el elemento %d: ", I + 1);
        scanf("%f", &A[I]);
    }
}

/* Suma el cuadrado de los componentes del arreglo */
double Suma(float A[], int T)
{
    int I;
    double AUX = 0.0;
    for (I = 0; I < T; I++)
        AUX += A[I] * A[I];
    return AUX;
}
