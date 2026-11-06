
#include <stdio.h>
#include <string.h>

/* Estructuras-1. Muestra cómo se declara una estructura y cómo se accede
   a sus campos para asignación, lectura y escritura. */

struct alumno   /* Declaración de la estructura. */
{
    int matricula;
    char nombre[20];
    char carrera[20];   /* Campos de la estructura. */
    float promedio;
    char direccion[20];
};   /* La declaración de una estructura termina con punto y coma. */

/* Vacía el búfer de entrada (reemplaza a fflush(stdin)). */
void limpiarBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Lee una línea de texto de forma segura (reemplaza a gets). */
void leerLinea(char *destino, int tam)
{
    fgets(destino, tam, stdin);
    destino[strcspn(destino, "\n")] = '\0';
}

int main(void)
{
    /* a1 recibe valores directamente al declararse. */
    struct alumno a1 = {120, "María", "Contabilidad", 8.9, "Querétaro"}, a2, a3;

    char nom[20], car[20], dir[20];
    int mat;
    float pro;

    /* Los campos de a2 reciben valores por medio de una lectura directa. */
    printf("\nIngrese la matrícula del alumno 2: ");
    scanf("%d", &a2.matricula);
    limpiarBuffer();
    printf("Ingrese el nombre del alumno 2: ");
    leerLinea(a2.nombre, sizeof(a2.nombre));
    printf("Ingrese la carrera del alumno 2: ");
    leerLinea(a2.carrera, sizeof(a2.carrera));
    printf("Ingrese el promedio del alumno 2: ");
    scanf("%f", &a2.promedio);
    limpiarBuffer();
    printf("Ingrese la dirección del alumno 2: ");
    leerLinea(a2.direccion, sizeof(a2.direccion));

    /* Los campos de a3 reciben valores por medio de asignaciones. */
    printf("\nIngrese la matrícula del alumno 3: ");
    scanf("%d", &mat);
    a3.matricula = mat;
    limpiarBuffer();
    printf("Ingrese el nombre del alumno 3: ");
    leerLinea(nom, sizeof(nom));
    strcpy(a3.nombre, nom);
    printf("Ingrese la carrera del alumno 3: ");
    leerLinea(car, sizeof(car));
    strcpy(a3.carrera, car);
    printf("Ingrese el promedio del alumno 3: ");
    scanf("%f", &pro);
    a3.promedio = pro;
    limpiarBuffer();
    printf("Ingrese la dirección del alumno 3: ");
    leerLinea(dir, sizeof(dir));
    strcpy(a3.direccion, dir);

    /* Forma de imprimir los campos de a1 y a2. */
    printf("\nDatos del alumno 1\n");
    printf("%d\n", a1.matricula);
    puts(a1.nombre);
    puts(a1.carrera);
    printf("%.2f\n", a1.promedio);
    puts(a1.direccion);

    printf("\nDatos del alumno 2\n");
    printf("%d\n", a2.matricula);
    puts(a2.nombre);
    puts(a2.carrera);
    printf("%.2f\n", a2.promedio);
    puts(a2.direccion);

    /* Otra forma de escribir los campos de una estructura. */
    printf("\nDatos del alumno 3\n");
    printf("%d \t %s \t %s \t %.2f \t %s\n", a3.matricula, a3.nombre,
           a3.carrera, a3.promedio, a3.direccion);

    return 0;
}
