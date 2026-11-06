#include <stdio.h>

/* Archivos y caracteres. El programa escribe caracteres en un archivo. */
int main(void)
{
    int p1;
    FILE *ar;

    ar = fopen("arc.txt", "w");   /* Se abre arc.txt para escritura. */
    if (ar != NULL)
    {
        /* Se escribe en el archivo hasta detectar fin de línea o fin de entrada. */
        while ((p1 = getchar()) != '\n' && p1 != EOF)
            fputc(p1, ar);

        fclose(ar);               /* Se cierra el archivo. */
    }
    else
        printf("No se puede abrir el archivo\n");

    return 0;
}
