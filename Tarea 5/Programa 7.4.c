#include <stdio.h>
#include <string.h>

int main(void)
{
    char *cad0;
    cad0 = "Argentina";   /* Correcto: el apuntador apunta a una cadena constante */
    puts(cad0);
    cad0 = "Brasil";      /* Correcto: ahora apunta a otra cadena */
    puts(cad0);

    /* INCORRECTO: cad1 no apunta a ninguna memoria reservada.
       Leer con gets/scanf sobre él falla en la ejecución.
    char *cad1;
    gets(*cad1);
    gets(cad1);
    */

    /* INCORRECTO: no se reserva espacio, error de compilación.
    char cad1[];
    */

    char cad2[20] = "México";   /* Correcto */
    puts(cad2);

    printf("Ingrese una cadena: ");
    fgets(cad2, sizeof(cad2), stdin);       /* Se modifica por lectura */
    cad2[strcspn(cad2, "\n")] = '\0';
    puts(cad2);

    strcpy(cad2, "Guatemala");              /* Se modifica con string.h */
    puts(cad2);

    /* INCORRECTO: no se puede asignar una cadena a un solo caracter.
    cad2[10] = "Guatemala";
    */

    return 0;
}
