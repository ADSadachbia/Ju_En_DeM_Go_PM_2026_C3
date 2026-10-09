#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Volumen y área del cilindro
   El programa, al recibir como datos el radio y la altura de un cilindro,
   calcula su área y su volumen.
   RAD, ALT, VOL y ARE: variables de tipo real. */

/* Algunos compiladores (como MSVC) necesitan esto para usar M_PI */


int main(void)
{
    float RAD, ALT, VOL, ARE;

    printf("Ingrese el radio y la altura del cilindro: ");
    scanf("%f %f", &RAD, &ALT);

    VOL = M_PI * RAD * RAD * ALT;

    /* Área total (lateral + dos tapas) */
    ARE = 2 * M_PI * RAD * (RAD + ALT);

    /* Si solo quieres el área lateral, usa esta línea en su lugar:
       ARE = 2 * M_PI * RAD * ALT; */

    printf("\nEl volumen es: %6.2f \t El area es: %6.2f\n", VOL, ARE);

    return 0;
}
