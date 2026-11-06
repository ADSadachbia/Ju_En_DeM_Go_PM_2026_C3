#include <stdio.h>
#include <stdlib.h>   /* Funciones de conversión: atoi, atof, strtod, atol, strtol */
#include <string.h>   /* Para strcspn */

int main(void)
{
    int i;
    double d;
    long l;
    char cad0[20], *cad1;

    /* atoi: cadena -> int */
    printf("\nIngrese una cadena de caracteres: ");
    fgets(cad0, sizeof(cad0), stdin);
    cad0[strcspn(cad0, "\n")] = '\0';
    i = atoi(cad0);
    printf("\n%s \t %d", cad0, i + 3);   /* i + 3 demuestra que ya es entero */

    /* atof: cadena -> double */
    printf("\nIngrese una cadena de caracteres: ");
    fgets(cad0, sizeof(cad0), stdin);
    cad0[strcspn(cad0, "\n")] = '\0';
    d = atof(cad0);
    printf("\n%s \t %.2lf ", cad0, d + 1.50);

    /* strtod: cadena -> double, y cad1 apunta al resto de la cadena */
    d = strtod(cad0, &cad1);
    printf("\n%s \t %.2lf", cad0, d + 1.50);
    puts(cad1);

    /* atol: cadena -> long */
    l = atol(cad0);
    printf("\n%s \t %ld ", cad0, l + 10);

    /* strtol: cadena -> long, base 0 acepta octal, decimal o hexadecimal */
    l = strtol(cad0, &cad1, 0);
    printf("\n%s \t %ld", cad0, l + 10);
    puts(cad1);

    return 0;
}
