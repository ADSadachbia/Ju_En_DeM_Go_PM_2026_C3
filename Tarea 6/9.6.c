#include <stdio.h>

/* Archivos con variables enteras y reales.
   El programa lee los datos de un grupo de alumnos
   almacenados en un archivo.
*/

void main(void)
{
    int i, j, n, mat;
    float cal;

    FILE *ar;

    if ((ar = fopen("arc8.txt", "r")) != NULL)
    {
        fscanf(ar, "%d", &n);

        /* Se lee el número de alumnos del archivo. */

        for (i = 0; i < n; i++)
        {
            fscanf(ar, "%d", &mat);

            printf("\nMatrícula: %d", mat);

            /* Se lee la matrícula del alumno. */

            for (j = 0; j < 5; j++)
            {
                fscanf(ar, "%f", &cal);

                printf("\t%.2f", cal);

                /* Se leen las calificaciones
                   del alumno.
                */
            }
        }

        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}