#include <stdio.h>

/* Archivos con variables enteras y reales.
   El programa lee información de los alumnos de una escuela,
   almacenada en un archivo. Utiliza una función para realizar
   la lectura, pero el archivo se abre y cierra desde el programa principal.
*/

void promedio(FILE *);   /* Prototipo de función. */

void main(void)
{
    FILE *ar;

    if ((ar = fopen("arc9.txt", "r")) != NULL)
    {
        promedio(ar);

        /* Se llama a la función promedio.
           Observe la forma como se pasa el archivo como parámetro.
        */

        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}

void promedio(FILE *ar1)
/* Observa la forma como se recibe el archivo. */

/* Esta función lee los datos de los alumnos desde un archivo,
   e imprime tanto la matrícula como el promedio de cada alumno.
*/
{
    int i, j, n, 