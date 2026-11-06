#include <stdio.h>
#include <ctype.h>  /* Funciones para el manejo de caracteres */

/* Limpia el búfer de entrada (reemplazo de fflush(stdin) en Linux) */
void limpiar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(void)
{
    char p1;

    /* 1. ¿Es un dígito? */
    printf("\nIngrese un caracter para analizar si este es un digito: ");
    p1 = getchar();
    if (isdigit(p1))  /* isdigit regresa distinto de 0 si p1 es un dígito */
        printf("%c es un digito \n", p1);
    else
        printf("%c no es un digito \n", p1);
    if (p1 != '\n')
        limpiar_buffer();

    /* 2. ¿Es una letra? */
    printf("\nIngrese un caracter para examinar si este es una letra: ");
    p1 = getchar();
    if (isalpha(p1))  /* isalpha regresa distinto de 0 si p1 es una letra */
        printf("%c es una letra \n", p1);
    else
        printf("%c no es una letra \n", p1);
    if (p1 != '\n')
        limpiar_buffer();

    /* 3. ¿Es una letra minúscula? */
    printf("\nIngrese un caracter para examinar si este es una letra minuscula: ");
    p1 = getchar();
    if (isalpha(p1))
    {
        if (islower(p1))  /* islower: minúscula; isupper: mayúscula */
            printf("%c es una letra minuscula \n", p1);
        else
            printf("%c no es una letra minuscula \n", p1);
    }
    else
        printf("%c no es una letra \n", p1);
    if (p1 != '\n')
        limpiar_buffer();

    /* 4. Convertir de mayúscula a minúscula */
    printf("\nIngrese una letra para convertirla de mayuscula a minuscula: ");
    p1 = getchar();
    if (isalpha(p1))
    {
        if (isupper(p1))
            printf("%c fue convertida de mayuscula a minuscula \n", tolower(p1));
        else
            printf("%c es una letra minuscula \n", p1);
    }
    else
        printf("%c no es una letra \n", p1);

    return 0;
}
