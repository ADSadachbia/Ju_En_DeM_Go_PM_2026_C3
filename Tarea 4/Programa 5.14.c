#include <stdio.h>
#include <math.h>

/* Estadístico.
   Recibe como dato un arreglo unidimensional de enteros que contiene
   calificaciones y calcula la media, la varianza, la desviación estándar
   y la moda. */

#define MAX 100

/* Prototipos de funciones */
void Lectura(int *, int);
float Media(int *, int);
float Varianza(int *, int, float);
float Desviacion(float);
void Frecuencia(int *, int, int *);
int Moda(int *, int);

int main(void)
{
    int TAM, MOD, ALU[MAX], FRE[11] = {0};
    float MED, VAR, DES;

    do
    {
        printf("Ingrese el tamaño del arreglo: ");
        scanf("%d", &TAM);
    }
    while (TAM > MAX || TAM < 1);   /* Verifica que el tamaño sea correcto */

    Lectura(ALU, TAM);
    MED = Media(ALU, TAM);
    VAR = Varianza(ALU, TAM, MED);
    DES = Desviacion(VAR);
    Frecuencia(ALU, TAM, FRE);
    MOD = Moda(FRE, 11);

    printf("\nMedia: %.2f", MED);
    printf("\nVarianza: %.2f", VAR);
    printf("\nDesviación: %.2f", DES);
    printf("\nModa: %d", MOD);
    printf("\n");

    return 0;
}

/* Lee un arreglo de T elementos enteros (calificaciones de 0 a 10) */
void Lectura(int A[], int T)
{
    int I;
    for (I = 0; I < T; I++)
    {
        printf("Ingrese el elemento %d: ", I + 1);
        scanf("%d", &A[I]);
    }
}

/* Calcula la media */
float Media(int A[], int T)
{
    int I;
    float SUM = 0.0;
    for (I = 0; I < T; I++)
        SUM += A[I];
    return (SUM / T);
}

/* Calcula la varianza */
float Varianza(int A[], int T, float M)
{
    int I;
    float SUM = 0.0;
    for (I = 0; I < T; I++)
        SUM += (A[I] - M) * (A[I] - M);
    return (SUM / T);
}

/* Calcula la desviación estándar */
float Desviacion(float V)
{
    return (sqrt(V));
}

/* Calcula la frecuencia de cada calificación (0 a 10) */
void Frecuencia(int A[], int P, int B[])
{
    int I;
    for (I = 0; I < P; I++)
        if (A[I] >= 0 && A[I] <= 10)    /* Valida la calificación */
            B[A[I]]++;
}

/* Calcula la moda: la calificación con la frecuencia más alta
   (la primera, si hay empate) */
int Moda(int A[], int T)
{
    int I, MOD = 0, VAL = A[0];
    for (I = 1; I < T; I++)
        if (A[I] > VAL)
        {
            VAL = A[I];
            MOD = I;
        }
    return (MOD);
}
