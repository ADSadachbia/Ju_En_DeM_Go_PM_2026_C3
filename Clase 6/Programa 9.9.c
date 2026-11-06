#include <stdio.h>

/* Alumnos.
   El programa lee bloques (variables de tipo estructura alumno)
   de un archivo de acceso directo. */
typedef struct { /* Declaración de la estructura alumno. */
    int matricula;
    char nombre[20];
    int carrera;
    float promedio;
} alumno;

void lee(FILE *); /* Prototipo de función. */

int main(void)
{
    FILE *ar;

    if ((ar = fopen("ad1.dat", "r")) != NULL)
    {
        lee(ar);
        fclose(ar);
    }
    else
        printf("\nEl archivo no se puede abrir\n");

    return 0;
}

void lee(FILE *ap)
/* Lee bloques de un archivo de acceso directo. */
{
    alumno alu;

    /* fread devuelve el número de elementos leídos: 1 si hubo registro,
       0 si se llegó al fin del archivo. */
    while (fread(&alu, sizeof(alumno), 1, ap) == 1)
    {
        printf("\nMatrícula: %d", alu.matricula);
        printf("\tCarrera: %d", alu.carrera);
        printf("\tPromedio: %f\t", alu.promedio);
        puts(alu.nombre);
        fread(&alu, sizeof(alumno), 1, ap);
    }
}
