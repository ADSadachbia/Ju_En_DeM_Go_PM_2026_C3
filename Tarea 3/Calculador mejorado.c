#include <stdio.h>
#include <stdlib.h>

#define SALIR 0
#define SUMAR 1
#define restar 2
#define multiplicar 3
#define DIVICION 4
#define RAIZ 5
#define CUADRADO 6
#define ERR_OK 0
#define ERR_DivByZero 789
#define ERR_RaizNegativa 790

int suma(double num1, double num2, double *result);//declaracion de funcion
int resta(double num1, double num2, double *result);
int multiplicacion(double num1, double num2, double *result);
int dividir(double divisor, double denominador, double *result);
int raiz_cuadrada(double num, double *result);
int al_cuadrado(double num, double *result);

int main()
{
    int menu = -1;
    int err = ERR_OK;
    double n1 = 0.0;
    double n2 = 0.0;
    double r = 0.0;

    do
    {
        printf("\n\n0-SALIR\n1-SUMAR\n2-RESTAR\n3-MULTIPLICAR\n4-DIVIDIR\n5-RAIZ CUADRADA\n6-ELEVAR AL CUADRADO\n");
        scanf("%i",&menu);

        if(menu == SUMAR)
        {
            printf("\nIngresa el primer sumando:");
            scanf("%lf",&n1);
            printf("\nIngresa el segundo sumando:");
            scanf("%lf",&n2);
            err = suma(n1,n2,&r);
            if(err == ERR_OK)
            {
                printf("\nSuma de %lf mas %lf es %lf",n1,n2,r);
            }
            else
            {
                printf("\nError de operacion suma");
            }
        }

        if(menu == restar)
        {
            printf("\nIngresa el minuendo:");
            scanf("%lf",&n1);
            printf("\nIngresa el sustraendo:");
            scanf("%lf",&n2);
            err = resta(n1,n2,&r);
            if(err == ERR_OK)
            {
                printf("\nResta de %lf menos %lf es %lf",n1,n2,r);
            }
            else
            {
                printf("\nError de operacion resta");
            }
        }

        if(menu == multiplicar)
        {
            printf("\nIngresa el primer factor:");
            scanf("%lf",&n1);
            printf("\nIngresa el segundo factor:");
            scanf("%lf",&n2);
            err = multiplicacion(n1,n2,&r);
            if(err == ERR_OK)
            {
                printf("\nMultiplicacion de %lf por %lf es %lf",n1,n2,r);
            }
            else
            {
                printf("\nError de operacion multiplicacion");
            }
        }

        if(menu == DIVICION)
        {
            printf("\nIngresa el dividendo:");
            scanf("%lf",&n1);
            printf("\nIngresa el denominador:");
            scanf("%lf",&n2);
            err = dividir(n1,n2,&r);
            if(err == ERR_OK)
            {
                printf("\nDivicion de %lf entre %lf es %lf",n1,n2,r);
            }
            else
            {
                if(err == ERR_DivByZero)
                {
                    printf("\nError no se puede dividir entre cero");
                }
                else
                {
                    printf("\nError inesperado");
                }
            }
        }

        if(menu == RAIZ)
        {
            printf("\nIngresa el numero:");
            scanf("%lf",&n1);
            err = raiz_cuadrada(n1,&r);
            if(err == ERR_OK)
            {
                printf("\nRaiz cuadrada de %lf es %lf",n1,r);
            }
            else
            {
                if(err == ERR_RaizNegativa)
                {
                    printf("\nError no existe raiz cuadrada real de un numero negativo");
                }
                else
                {
                    printf("\nError inesperado");
                }
            }
        }

        if(menu == CUADRADO)
        {
            printf("\nIngresa el numero:");
            scanf("%lf",&n1);
            err = al_cuadrado(n1,&r);
            if(err == ERR_OK)
            {
                printf("\n%lf elevado al cuadrado es %lf",n1,r);
            }
            else
            {
                printf("\nError de operacion elevar al cuadrado");
            }
        }
    }
    while(menu != SALIR);

    return 0;
}

int suma(double num1, double num2, double *result)
{
    *result = num1 + num2;
    return 0;
}

int resta(double num1, double num2, double *result)
{
    *result = num1 - num2;
    return 0;
}

int multiplicacion(double num1, double num2, double *result)
{
    *result = num1 * num2;
    return 0;
}

int dividir(double divisor, double denominador, double *result)
{
    if(denominador == 0.0)
    {
        return ERR_DivByZero;
    }
    else
    {
        *result = divisor / denominador;
        return ERR_OK;
    }
}

/* Raiz cuadrada por el metodo babilonico (Newton-Raphson):
   x(n+1) = (x(n) + num / x(n)) / 2 */
int raiz_cuadrada(double num, double *result)
{
    double x, siguiente, diferencia;
    int i;

    if(num < 0.0)
    {
        return ERR_RaizNegativa;
    }

    if(num == 0.0)
    {
        *result = 0.0;
        return ERR_OK;
    }

    x = (num >= 1.0) ? num : 1.0;   /* aproximacion inicial */

    for(i = 0; i < 100; i++)
    {
        siguiente = 0.5 * (x + num / x);
        diferencia = siguiente - x;
        if(diferencia < 0.0)
        {
            diferencia = -diferencia;
        }
        x = siguiente;
        if(diferencia < 1e-12)
        {
            break;
        }
    }

    *result = x;
    return ERR_OK;
}

int al_cuadrado(double num, double *result)
{
    *result = num * num;
    return ERR_OK;
}
