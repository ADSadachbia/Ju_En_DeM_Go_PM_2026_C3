#include <stdio.h>
#include <string.h>

/* Funciones para el manejo de cadenas de caracteres de la biblioteca stdio.h. */

/* Vacía el búfer de entrada (reemplaza a fflush(stdin)). */
void limpiarBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    char *cad0 = "Buenos días";
    char cad1[20] = "Hola";
    char cad2[] = "México";
    char cad3[] = {'B', 'i', 'e', 'n', 'v', 'e', 'n', 'i', 'd', 'o', '\0'};
    char cad4[20], cad5[20], cad6[20];
    int p;
    int i = 0;

    printf("\nLa cadena cad0 es: ");
    puts(cad0);

    printf("\nLa cadena cad1 es: ");
    printf("%s\n", cad1);

    printf("\nLa cadena cad2 es: ");
    puts(cad2);

    printf("\nLa cadena cad3 es: ");
    puts(cad3);

    /* fgets en lugar de gets: limita la lectura al tamaño del arreglo */
    printf("\nIngrese una línea de texto —se lee con fgets—: \n");
    fgets(cad4, sizeof(cad4), stdin);
    cad4[strcspn(cad4, "\n")] = '\0';   /* quita el '\n' que fgets conserva */
    printf("\nLa cadena cad4 es: ");
    puts(cad4);

    /* scanf solo lee la primera palabra; %19s evita desbordar cad5 */
    printf("\nIngrese una línea de texto —se lee con scanf—: \n");
    scanf("%19s", cad5);
    printf("\nLa cadena cad5 es: ");
    printf("%s\n", cad5);
    limpiarBuffer();

    printf("\nIngrese una línea de texto —se lee cada caracter con getchar—: \n");
    while ((p = getchar()) != '\n' && p != EOF && i < 19)
        cad6[i++] = (char)p;
    cad6[i] = '\0';

    printf("\nLa cadena cad6 es: ");
    puts(cad6);

    return 0;
}
