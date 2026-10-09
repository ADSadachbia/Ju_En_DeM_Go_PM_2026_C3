#include <stdio.h>

/* Frecuencia de calificaciones.
   Recibe las calificaciones (0 a 5) de un grupo de 50 alumnos, obtiene la
   frecuencia de cada calificación y escribe cuál es la frecuencia más alta. */

#define TAM 50

/* Prototipos de funciones */
void Lectura(int *, int);
void Frecuencia(int *, int, int *, int);
void Impresion(int *, int);
void Mayor(int *, int);

int main(void)
{
    int CAL[TAM], FRE[6] = {0};   /* Arreglos de calificaciones y frecuencias */

    Lectura(CAL, TAM);
    Frecuencia(CAL, TAM, FRE, 6);

    printf("\nFrecuencia de Calificaciones\n");
    Impresion(FRE, 6);
    Mayor(FRE, 6);

    return 0;
}

/* Lee el arreglo de calificaciones */
void Lectura(int VEC[], int T)
{
    int I;
    for (I = 0; I < T; I++)
    {
        printf("Ingrese la calificacion -0:5- del alumno %d: ", I + 1);
        scanf("%d", &VEC[I]);
    }
}

/* Imprime el arreglo de frecuencias */
void Impresion(int VEC[], int T)
{
    int I;
    for (I = 0; I < T; I++)
        printf("\nFrecuencia de la calificacion %d: %d", I, VEC[I]);
}

/* Cuenta cuántas veces aparece cada calificación */
void Frecuencia(int A[], int T, int B[], int M)
{
    int I;
    for (I = 0; I < T; I++)
        if ((A[I] >= 0) && (A[I] < M))   /* Valida que la calificación sea correcta */
            B[A[I]]++;                   /* Incrementa la frecuencia de esa calificación */
}

/* Obtiene la primera ocurrencia de la frecuencia más alta */
void Mayor(int *X, int T)
{
    int I, MFRE = 0, MVAL = X[0];

    for (I = 1; I < T; I++)
        if (X[I] > MVAL)
        {
            MVAL = X[I];
            MFRE = I;
        }

    printf("\n\nLa frecuencia mas alta es %d y corresponde a la calificacion %d\n",
           MVAL, MFRE);
}
