#include <stdio.h>

#include <stdlib.h>

/*

Realizar un programa que imprima en un mensaje

En base a la edad del usuario

menor de 2 agnos es un infante

mayor de 2 agnos y menor igual de 10 es un nigno

mayor de 10 y menor de 18 es un adolescente

mayor de 18 y menor de 60 es adulto

mayor de 60 es envejeciente

*/

int main()

{

    int edad = 0;

    printf("\nHola Ingresa tu edad:");

    scanf("%i",&edad);

    //

    if(edad >= 0 && edad <= 130)

    {

        if(edad <= 2)

        {

            printf("\nEs un Infante!");

        }else if(edad <=10)

        {

            printf("\nEs un nigno!");

        }else if(edad <= 18)

        {

            printf("\nEs Adolescente!");

        }
        else if (edad <= 60){

        printf("\nEs Adulto!");
        }

        else if (edad >=60)
        {
         printf("\nEs Envejeciente!");
         }
    }

    else

    {

        if(edad < 0)

        {

            printf("\n!ERROR EDAD DEBE SER MAYOR QUE 0!");

        }

        else

        {

            if(edad > 130 && edad <=500)

            {

                printf("\n!ERROR EDAD DEBE SER MENOR QUE 130!");

            }else if(edad > 500 && edad <= 1000)

            {

                printf("\n!ERROR USTED ES UN VAMPIRO!");

            }else

            {

                printf("\n!ERROR USTED ESTA EXAGERANDO CON LA EDAD!");

            }

        }

    }


    return 0;

}
