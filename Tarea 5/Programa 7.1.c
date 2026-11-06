#include <stdio.h>

/* Limpia el búfer de entrada (reemplazo de fflush(stdin) en Linux) */
void limpiar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    char p1, p2, p3 = '$';  /* p3 se inicializa con el símbolo $ */

    printf("\nIngrese un caracter: ");
    p1 = getchar();         /* Lee un caracter */
    putchar(p1);            /* Escribe un caracter */
    printf("\n");

    /* Si el usuario escribió más de un caracter o solo se leyó 1 y quedó el Enter,
       se limpia el búfer. Solo si p1 no fue el Enter mismo. */
    if (p1 != '\n')
        limpiar_buffer();

    printf("\nEl caracter p3 es: ");
    putchar(p3);
    printf("\n");

    printf("\nIngrese otro caracter: ");
    scanf("%c", &p2);       /* scanf con %c para leer un caracter */
    printf("%c", p2);       /* printf con %c para escribir un caracter */
    printf("\n");

    return 0;
}
