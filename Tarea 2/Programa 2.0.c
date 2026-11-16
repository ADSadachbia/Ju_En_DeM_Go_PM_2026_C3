#include <stdio.h>
#include <stdlib.h>
/* Carpeta donde estan los Programa X.c*/
#define CARPETA "/media/adsadachbia/A-Working/Code/Blocks/Clases/Tarea 2/TAREA2"

int main(void)
{
    /* Agrega aqui los archivos que quieras en el menu */
const char *archivos[] = {
    "Programa 2.1",
    "Programa 2.2",
    "Programa 2.3",
    "Programa 2.4",
    "Programa 2.5",
    "Programa 2.6",
    "Programa 2.7",
    "Programa 2.8",
    "Programa 2.9",
    "Programa 2.10",
    "Programa 2.11",
    "Programa 2.12",
    "Programa 2.13",
    "Programa 2.14",
    "Programa 2.15",
    "Programa 2.16"
};
    int total = sizeof(archivos) / sizeof(archivos[0]);
    int opcion, i;
    char comando[512];

    do
    {
        printf("\n===== MENU =====\n");
        for (i = 0; i < total; i++)
            printf("%d. %s\n", i + 1, archivos[i]);
        printf("0. Salir\n");
        printf("Elige una opcion: ");
        scanf("%d", &opcion);

        if (opcion >= 1 && opcion <= total)
        {
            /* Compila el .c elegido a un ejecutable temporal y lo corre */
            snprintf(comando, sizeof(comando),
                     "gcc \"%s/%s.c\" -o /tmp/prog_menu -lm && /tmp/prog_menu",
                     CARPETA, archivos[opcion - 1]);
            system(comando);
            printf("\n--- Fin de %s ---\n", archivos[opcion - 1]);
        }
        else if (opcion != 0)
            printf("\nOpcion no valida\n");

    } while (opcion != 0);

    return 0;
}
