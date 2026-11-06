
#include <stdio.h>

/* Archivos y caracteres. El programa lee caracteres de un archivo. */
int main(void)
{
    int p1;
    FILE *ar;

    /* Abrir el archivo y verificar que se abrió, en una sola instrucción. */
    if ((ar = fopen("arc.txt", "w")) != NULL)
    {
        /* Se leen caracteres mientras no se llegue al fin del archivo. */
        while ((p1 = fgetc(ar)) != EOF)
            putchar(p1);

        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo\n");

    return 0;
}
