#include <stdio.h>

/* Torres de Hanói.
El programa recibe como dato el número de discos e imprime la secuencia
exacta de movimientos para trasladarlos de la torre origen a la destino
utilizando una torre auxiliar. */

void hanoi(int n, char origen, char destino, char auxiliar);

int main(void)
{
    int discos;

    do
    {
        printf("Ingrese el número de discos (mayor a 0): ");
        scanf("%d", &discos);
    } while (discos < 1);

    printf("\nSecuencia de movimientos:\n\n");
    /* A = Torre Origen, C = Torre Destino, B = Torre Auxiliar */
    hanoi(discos, 'A', 'C', 'B');

    return 0;
}

void hanoi(int n, char origen, char destino, char auxiliar)
/* Función recursiva que resuelve el traslado de n discos. */
{
    if (n == 1)
    {
        /* Caso base: mover el único disco directamente al destino */
        printf("Mover disco 1 de la torre %c a la torre %c\n", origen, destino);
    }
    else
    {
        /* Paso 1: Mover n-1 discos de origen a auxiliar usando destino */
        hanoi(n - 1, origen, auxiliar, destino);

        /* Paso 2: Mover el disco más grande (n) de origen a destino */
        printf("Mover disco %d de la torre %c a la torre %c\n", n, origen, destino);

        /* Paso 3: Mover los n-1 discos de auxiliar a destino usando origen */
        hanoi(n - 1, auxiliar, destino, origen);
    }
}
