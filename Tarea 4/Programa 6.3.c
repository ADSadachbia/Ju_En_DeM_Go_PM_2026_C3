}
printf(”\n\nAño con mayor ingreso de alumnos: %d Alumnos: %d”, AO+1, MAY);
}
void Funcion2(int A[][C][P],int FI, int CO, int PR)
/* Esta función se utiliza para determinar la carrera que recibió el mayor
➥número de alumnos el último año. Observa que no se utiliza un ciclo para los
➥planos de la profundidad, ya que es un dato (PR). */
{
int I, J, MAY = 0, CAR = -1, SUM;
for (I=0; I<FI; I++)
{
SUM = 0;
for (J=0; J<CO; J++)
SUM += A[I][J][PR-1];
if (SUM > MAY)
{
MAY = SUM;
CAR = I;
}
}
printf(”\n\nCarrera con mayor número de alumnos: %d Alumnos: %d”, CAR+1,
MAY);
}
void Funcion3(int A[][C][P],int FI, int CO, int PR)
/* Esta función se utiliza para determinar el año en el que la carrera
➥Ingeniería en Computación recibió el mayor número de alumnos. Observa que no
➥se utiliza un ciclo para trabajar con las filas, ya que es un dato (FI). */
{
int K, J, MAY = 0, AO = -1, SUM;
for (K=0; K<PR; K++)
{
SUM = 0;
for (J=0; J<CO; J++)
SUM += A[FI-1][J][K];
if (SUM > MAY)
{
MAY = SUM;
AO = K;
}
}
printf(”\n\nAño con mayor ingreso de alumnos: %d Alumnos: %d”, AO+1, MAY);
}
