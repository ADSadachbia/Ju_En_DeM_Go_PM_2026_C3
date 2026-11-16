#include <stdio.h>
#include <stdlib.h>

/* Lectura de archivo de texto con memoria dinámica.
El programa lee un archivo de tamaño desconocido carácter por carácter,
reservando espacio inicial con malloc() y ampliándolo con realloc(). */

char *leer_archivo(FILE *ar);

int main(void)
{
    FILE *ar;
    char *contenido;

    ar = fopen("texto.txt", "r");
    if (ar != NULL)
    {
        contenido = leer_archivo(ar);
        fclose(ar);

        if (contenido != NULL)
        {
            printf("\n--- Contenido del archivo ---\n\n");
            printf("%s\n", contenido);
            
            /* Liberar la memoria dinámica al terminar */
            free(contenido);
        }
        else
            printf("\nError: No se pudo asignar memoria.\n");
    }
    else
        printf("\nNo se puede abrir el archivo texto.txt\n");

    return 0;
}

char *leer_archivo(FILE *ar)
/* Lee el archivo carácter por carácter y redimensiona el bloque de memoria. */
{
    int capacidad = 10; /* Tamaño inicial pequeño para demostrar realloc */
    int longitud = 0;
    int ch;
    char *buffer, *temp;

    /* 1. Reserva inicial de memoria con malloc() */
    buffer = (char *)malloc(capacidad * sizeof(char));
    if (buffer == NULL)
        return NULL;

    /* 2. Lectura hasta el fin de archivo (EOF) */
    while ((ch = fgetc(ar)) != EOF)
    {
        /* Si alcanzamos el límite (dejando 1 espacio para '\0'), ampliamos */
        if (longitud + 1 >= capacidad)
        {
            capacidad *= 2; /* Duplicamos la capacidad para mayor eficiencia */
            temp = (char *)realloc(buffer, capacidad * sizeof(char));
            if (temp == NULL)
            {
                free(buffer); /* Evitar fuga de memoria si realloc falla */
                return NULL;
            }
            buffer = temp;
        }
        buffer[longitud++] = (char)ch;
    }

    /* 3. Ajustar la memoria al tamaño exacto utilizado más el carácter nulo */
    temp = (char *)realloc(buffer, (longitud + 1) * sizeof(char));
    if (temp != NULL)
        buffer = temp;

    buffer[longitud] = '\0'; /* Terminación de la cadena */
    return buffer;
}
