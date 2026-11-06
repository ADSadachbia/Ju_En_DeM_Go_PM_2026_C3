#include <stdio.h>
#include <string.h>  /* Para strcspn */

/* Limpia el búfer de entrada (reemplazo de fflush(stdin) en Linux) */
void limpiar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    char *cad0 = "Buenos días";   /* Apuntador a cadena constante */
    char cad1[20] = "Hola";       /* Espacio para 20 caracteres */
    char cad2[] = "México";       /* El tamaño se ajusta a la cadena */
    char cad3[] = {'B', 'i', 'e', 'n', 'v', 'e', 'n', 'i', 'd', 'o', '\0'};
    char cad4[20], cad5[20], cad6[20];
    char p;
    int i = 0;

    printf("\nLa cadena cad0 es: ");
    puts(cad0);                   /* puts baja una línea al terminar */

    printf("\nLa cadena cad1 es: ");
    printf("%s\n", cad1);         /* printf NO baja línea, por eso agrego \n */

    printf("\nLa cadena cad2 es: ");
    puts(cad2);

    printf("\nLa cadena cad3 es: ");
    puts(cad3);

    /* Lectura con fgets (reemplaza a gets) */
    printf("\nIngrese una línea de texto -se lee con fgets-: \n");
    fgets(cad4, sizeof(cad4), stdin);
    cad4[strcspn(cad4, "\n")] = '\0';   /* Quita el \n que fgets deja */
    printf("\nLa cadena cad4 es: ");
    puts(cad4);

    /* Lectura con scanf: solo lee hasta el primer espacio */
    printf("\nIngrese una línea de texto -se lee con scanf-: \n");
    scanf("%19s", cad5);                /* %19s evita desbordar cad5 */
    printf("\nLa cadena cad5 es: ");
    printf("%s\n", cad5);
    limpiar_buffer();                   /* Quita el Enter pendiente */

    /* Lectura carácter por carácter con getchar */
    printf("\nIngrese una línea de texto -se lee cada caracter con getchar-: \n");
    while ((p = getchar()) != '\n' && p != EOF && i < 19)
        cad6[i++] = p;
    cad6[i] = '\0';                     /* Caracter de terminación */

    printf("\nLa cadena cad6 es: ");
    puts(cad6);

    return 0;
}
