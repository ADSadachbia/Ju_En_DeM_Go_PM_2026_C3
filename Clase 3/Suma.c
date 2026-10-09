#include <stdio.h>
#include <stdlib.h>


int main()
{
int n1,n2,tmp;
    printf("Ingresa el numero 1\n");
    scanf ("%i",&n1);
      printf("Ingresa el numero 2\n");
    scanf ("%i",&n2);
    tmp = suma (n1,n2);
    printf("La suma de %i mas %i es: %i\n",n1,n2,tmp);
    return 0;
}
int suma (int n1, int n2)
{
 int t = 0;
 t = n1 + n2;
 return t;
}
