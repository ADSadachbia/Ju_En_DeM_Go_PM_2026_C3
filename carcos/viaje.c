/*
 * viaje.c - Calculadora de costo de viaje y costo por kilometro
 *
 * Permite crear, listar, editar y borrar vehiculos con sus parametros de
 * costo, y calcular el costo total y por kilometro de un viaje segun la
 * distancia y el porcentaje recorrido en carretera y en ciudad.
 *
 * Criterios usados para los parametros:
 *   - Consumo de combustible: kilometros por litro (km/l).
 *   - Costos anuales (seguro, impuesto, limpieza): se prorratean entre los
 *     kilometros que recorre el vehiculo en un anio (campo "km por anio").
 *   - Depreciacion: (valor inicial - valor final) repartida entre los anios
 *     de uso; por km se divide ademas por los km anuales.
 *   - Neumaticos y mantenimiento: costo directo por kilometro.
 *
 * Los datos se guardan en el archivo "vehiculos.dat" del directorio actual.
 *
 * Compilacion:  gcc -std=c11 -Wall -Wextra -O2 -o viaje viaje.c
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_DATOS   "vehiculos.dat"
#define MAX_VEHICULOS   200
#define MAX_NOMBRE      64

typedef struct {
    int    id;
    char   nombre[MAX_NOMBRE];
    double consumo_ciudad;      /* km/l */
    double consumo_carretera;   /* km/l */
    double seguro_anual;        /* $/anio */
    double neumaticos_por_km;   /* $/km */
    double mantenimiento_por_km;/* $/km */
    double valor_inicial;       /* $ */
    double anios_uso;           /* anios */
    double valor_final;         /* $ residual al final del uso */
    double impuesto_anual;      /* $/anio */
    double limpieza_anual;      /* $/anio */
    double km_por_anio;         /* km/anio (para prorratear costos fijos) */
} Vehiculo;

typedef struct {
    double km_total;
    double km_ciudad;
    double km_carretera;
    double litros;
    double combustible;
    double neumaticos;
    double mantenimiento;
    double seguro;
    double impuesto;
    double depreciacion;
    double limpieza;
    double total;
    double por_km;
} Resultado;

/* ------------------------------------------------------------------ */
/* Entrada de datos                                                    */
/* ------------------------------------------------------------------ */

static int leer_linea(char *buf, size_t tam)
{
    if (fgets(buf, (int)tam, stdin) == NULL) {
        printf("\n(Fin de la entrada)\n");
        exit(0);
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    return 1;
}

static int texto_a_double(const char *texto, double *salida)
{
    char *fin = NULL;
    double valor = strtod(texto, &fin);
    while (fin != NULL && isspace((unsigned char)*fin)) fin++;
    if (fin == texto || (fin != NULL && *fin != '\0')) return 0;
    *salida = valor;
    return 1;
}

static int texto_a_entero(const char *texto, long *salida)
{
    char *fin = NULL;
    long valor = strtol(texto, &fin, 10);
    while (fin != NULL && isspace((unsigned char)*fin)) fin++;
    if (fin == texto || (fin != NULL && *fin != '\0')) return 0;
    *salida = valor;
    return 1;
}

/* Lee un numero. Si con_default es distinto de 0, Enter conserva "def". */
static double pedir_double(const char *etiqueta, int con_default, double def)
{
    char linea[128];
    double valor;
    for (;;) {
        printf("  %s", etiqueta);
        if (con_default) printf(" [%.2f]", def);
        printf(": ");
        fflush(stdout);
        leer_linea(linea, sizeof linea);

        if (linea[0] == '\0') {
            if (con_default) return def;
            printf("  Debe ingresar un numero.\n");
            continue;
        }
        if (!texto_a_double(linea, &valor)) {
            printf("  Numero invalido, intente de nuevo.\n");
            continue;
        }
        return valor;
    }
}

static double pedir_double_positivo(const char *etiqueta, int con_default, double def)
{
    for (;;) {
        double valor = pedir_double(etiqueta, con_default, def);
        if (valor > 0.0) return valor;
        printf("  El valor debe ser mayor que cero.\n");
    }
}

static double pedir_double_no_negativo(const char *etiqueta, int con_default, double def)
{
    for (;;) {
        double valor = pedir_double(etiqueta, con_default, def);
        if (valor >= 0.0) return valor;
        printf("  El valor no puede ser negativo.\n");
    }
}

static int pedir_opcion(const char *etiqueta, int minimo, int maximo)
{
    char linea[32];
    long valor;
    for (;;) {
        printf("%s", etiqueta);
        fflush(stdout);
        leer_linea(linea, sizeof linea);
        if (texto_a_entero(linea, &valor) && valor >= minimo && valor <= maximo)
            return (int)valor;
        printf("  Opcion invalida (debe ser un numero entre %d y %d).\n", minimo, maximo);
    }
}

static void pedir_nombre(char *destino, const char *actual)
{
    char linea[MAX_NOMBRE + 2];
    for (;;) {
        printf("  Nombre del vehiculo%s: ",
               actual != NULL ? " (Enter para mantener)" : "");
        fflush(stdout);
        leer_linea(linea, sizeof linea);

        if (linea[0] == '\0') {
            if (actual != NULL) return;
            printf("  El nombre no puede estar vacio.\n");
            continue;
        }
        size_t largo = strlen(linea);
        if (largo >= MAX_NOMBRE) largo = MAX_NOMBRE - 1;
        memcpy(destino, linea, largo);
        destino[largo] = '\0';
        return;
    }
}

/* ------------------------------------------------------------------ */
/* Persistencia                                                        */
/* ------------------------------------------------------------------ */

static int guardar_vehiculos(const Vehiculo *vehiculos, int cantidad)
{
    FILE *archivo = fopen(ARCHIVO_DATOS, "wb");
    if (archivo == NULL) {
        perror("  No se pudo abrir el archivo de datos");
        return 0;
    }
    int ok = fwrite(&cantidad, sizeof cantidad, 1, archivo) == 1 &&
             fwrite(vehiculos, sizeof *vehiculos, (size_t)cantidad, archivo) == (size_t)cantidad;
    fclose(archivo);
    if (!ok) printf("  Error al guardar los datos.\n");
    return ok;
}

static void cargar_vehiculos(Vehiculo *vehiculos, int *cantidad)
{
    *cantidad = 0;
    FILE *archivo = fopen(ARCHIVO_DATOS, "rb");
    if (archivo == NULL) return; /* primera ejecucion */

    int cantidad_leida = 0;
    if (fread(&cantidad_leida, sizeof cantidad_leida, 1, archivo) != 1 ||
        cantidad_leida < 0 || cantidad_leida > MAX_VEHICULOS) {
        fclose(archivo);
        printf("  Aviso: el archivo %s no es valido. Se empieza sin datos.\n", ARCHIVO_DATOS);
        return;
    }
    if (cantidad_leida > 0 &&
        fread(vehiculos, sizeof *vehiculos, (size_t)cantidad_leida, archivo) != (size_t)cantidad_leida) {
        fclose(archivo);
        printf("  Aviso: los datos de %s estan incompletos. Se empieza sin datos.\n", ARCHIVO_DATOS);
        return;
    }
    fclose(archivo);
    *cantidad = cantidad_leida;
}

/* ------------------------------------------------------------------ */
/* Presentacion                                                        */
/* ------------------------------------------------------------------ */

static double costo_fijo_por_km(const Vehiculo *v)
{
    double km_anio = v->km_por_anio;
    if (km_anio <= 0.0) return 0.0;

    double anual = v->seguro_anual + v->impuesto_anual + v->limpieza_anual;
    if (v->anios_uso > 0.0)
        anual += (v->valor_inicial - v->valor_final) / v->anios_uso;
    return anual / km_anio;
}

static void mostrar_vehiculo(const Vehiculo *v)
{
    double km_vida = v->anios_uso * v->km_por_anio;
    double depreciacion_km = 0.0;
    if (km_vida > 0.0)
        depreciacion_km = (v->valor_inicial - v->valor_final) / km_vida;

    printf("  [%d] %s\n", v->id, v->nombre);
    printf("      Consumo........: %.2f km/l ciudad | %.2f km/l carretera\n",
           v->consumo_ciudad, v->consumo_carretera);
    printf("      Costos anuales.: seguro $%.2f | impuesto $%.2f | limpieza $%.2f\n",
           v->seguro_anual, v->impuesto_anual, v->limpieza_anual);
    printf("      Por kilometro..: neumaticos $%.4f | mantenimiento $%.4f | depreciacion $%.4f\n",
           v->neumaticos_por_km, v->mantenimiento_por_km, depreciacion_km);
    printf("      Depreciacion...: inicial $%.2f -> final $%.2f en %.1f anio(s) (%.0f km/anio)\n",
           v->valor_inicial, v->valor_final, v->anios_uso, v->km_por_anio);
    printf("      Fijos totales..: $%.4f/km (seguro + impuesto + limpieza + depreciacion)\n",
           costo_fijo_por_km(v));
}

static int siguiente_id(const Vehiculo *vehiculos, int cantidad)
{
    int maximo = 0;
    for (int i = 0; i < cantidad; i++)
        if (vehiculos[i].id > maximo) maximo = vehiculos[i].id;
    return maximo + 1;
}

/* Devuelve el indice del vehiculo elegido, o -1 si se cancela. */
static int elegir_vehiculo(const Vehiculo *vehiculos, int cantidad)
{
    if (cantidad <= 0) {
        printf("  No hay vehiculos registrados.\n");
        return -1;
    }
    char linea[32];
    for (;;) {
        printf("  ID del vehiculo (0 para cancelar): ");
        fflush(stdout);
        leer_linea(linea, sizeof linea);

        long id;
        if (!texto_a_entero(linea, &id)) {
            printf("  ID invalido.\n");
            continue;
        }
        if (id == 0) return -1;
        for (int i = 0; i < cantidad; i++)
            if (vehiculos[i].id == (int)id) return i;
        printf("  No existe un vehiculo con ID %ld.\n", id);
    }
}

/* ------------------------------------------------------------------ */
/* Captura de parametros del vehiculo                                  */
/* ------------------------------------------------------------------ */

/* Si usar_defaults es distinto de 0, Enter conserva el valor actual. */
static void capturar_datos(Vehiculo *v, int usar_defaults)
{
    int d = usar_defaults;

    if (usar_defaults) pedir_nombre(v->nombre, v->nombre);
    else               pedir_nombre(v->nombre, NULL);

    v->consumo_ciudad       = pedir_double_positivo("Consumo en ciudad (km/l)", d, v->consumo_ciudad);
    v->consumo_carretera    = pedir_double_positivo("Consumo en carretera (km/l)", d, v->consumo_carretera);
    v->seguro_anual         = pedir_double_no_negativo("Costo de seguro por anio ($)", d, v->seguro_anual);
    v->neumaticos_por_km    = pedir_double_no_negativo("Costo de neumaticos por km ($)", d, v->neumaticos_por_km);
    v->mantenimiento_por_km = pedir_double_no_negativo("Costo de mantenimiento por km ($)", d, v->mantenimiento_por_km);
    v->valor_inicial        = pedir_double_no_negativo("Valor inicial del vehiculo ($)", d, v->valor_inicial);
    v->anios_uso            = pedir_double_positivo("Tiempo de uso / anios de depreciacion", d, v->anios_uso);
    v->valor_final          = pedir_double_no_negativo("Valor final o residual ($)", d, v->valor_final);
    v->impuesto_anual       = pedir_double_no_negativo("Costo de impuesto por anio ($)", d, v->impuesto_anual);
    v->limpieza_anual       = pedir_double_no_negativo("Costo de limpieza por anio ($)", d, v->limpieza_anual);
    v->km_por_anio          = pedir_double_positivo("Km recorridos por anio (ej: 20000)", d, v->km_por_anio);

    if (v->valor_final > v->valor_inicial)
        printf("  Nota: el valor final supera al inicial; la depreciacion sera negativa.\n");
}

/* ------------------------------------------------------------------ */
/* Operaciones                                                         */
/* ------------------------------------------------------------------ */

static void crear_vehiculo(Vehiculo *vehiculos, int *cantidad)
{
    if (*cantidad >= MAX_VEHICULOS) {
        printf("  Se alcanzo el limite de %d vehiculos.\n", MAX_VEHICULOS);
        return;
    }
    Vehiculo nuevo;
    memset(&nuevo, 0, sizeof nuevo);
    nuevo.id = siguiente_id(vehiculos, *cantidad);

    printf("\n-- Nuevo vehiculo (ID %d) --\n", nuevo.id);
    capturar_datos(&nuevo, 0);

    vehiculos[*cantidad] = nuevo;
    (*cantidad)++;
    if (guardar_vehiculos(vehiculos, *cantidad))
        printf("  Vehiculo [%d] %s creado.\n", nuevo.id, nuevo.nombre);
}

static void listar_vehiculos(const Vehiculo *vehiculos, int cantidad)
{
    if (cantidad <= 0) {
        printf("  No hay vehiculos registrados.\n");
        return;
    }
    printf("\n-- Vehiculos registrados (%d) --\n", cantidad);
    for (int i = 0; i < cantidad; i++) {
        mostrar_vehiculo(&vehiculos[i]);
        if (i + 1 < cantidad) printf("\n");
    }
}

static void editar_vehiculo(Vehiculo *vehiculos, int cantidad)
{
    int indice = elegir_vehiculo(vehiculos, cantidad);
    if (indice < 0) return;
    Vehiculo *v = &vehiculos[indice];

    for (;;) {
        printf("\n-- Editando [%d] %s --\n", v->id, v->nombre);
        mostrar_vehiculo(v);
        printf("\n  1) Nombre\n");
        printf("  2) Consumo en ciudad (km/l)\n");
        printf("  3) Consumo en carretera (km/l)\n");
        printf("  4) Seguro por anio\n");
        printf("  5) Neumaticos por km\n");
        printf("  6) Mantenimiento por km\n");
        printf("  7) Valor inicial\n");
        printf("  8) Tiempo de uso (anios)\n");
        printf("  9) Valor final\n");
        printf(" 10) Impuesto por anio\n");
        printf(" 11) Limpieza por anio\n");
        printf(" 12) Km por anio\n");
        printf(" 13) Editar todos los campos\n");
        printf("  0) Terminar edicion\n");
        int opcion = pedir_opcion("  Opcion: ", 0, 13);

        switch (opcion) {
        case 0:
            printf("  Edicion finalizada.\n");
            return;
        case 1:  pedir_nombre(v->nombre, v->nombre); break;
        case 2:  v->consumo_ciudad = pedir_double_positivo("Consumo en ciudad (km/l)", 1, v->consumo_ciudad); break;
        case 3:  v->consumo_carretera = pedir_double_positivo("Consumo en carretera (km/l)", 1, v->consumo_carretera); break;
        case 4:  v->seguro_anual = pedir_double_no_negativo("Costo de seguro por anio ($)", 1, v->seguro_anual); break;
        case 5:  v->neumaticos_por_km = pedir_double_no_negativo("Costo de neumaticos por km ($)", 1, v->neumaticos_por_km); break;
        case 6:  v->mantenimiento_por_km = pedir_double_no_negativo("Costo de mantenimiento por km ($)", 1, v->mantenimiento_por_km); break;
        case 7:  v->valor_inicial = pedir_double_no_negativo("Valor inicial del vehiculo ($)", 1, v->valor_inicial); break;
        case 8:  v->anios_uso = pedir_double_positivo("Tiempo de uso / anios de depreciacion", 1, v->anios_uso); break;
        case 9:  v->valor_final = pedir_double_no_negativo("Valor final o residual ($)", 1, v->valor_final); break;
        case 10: v->impuesto_anual = pedir_double_no_negativo("Costo de impuesto por anio ($)", 1, v->impuesto_anual); break;
        case 11: v->limpieza_anual = pedir_double_no_negativo("Costo de limpieza por anio ($)", 1, v->limpieza_anual); break;
        case 12: v->km_por_anio = pedir_double_positivo("Km recorridos por anio (ej: 20000)", 1, v->km_por_anio); break;
        case 13: capturar_datos(v, 1); break;
        }

        guardar_vehiculos(vehiculos, cantidad);
        printf("  Cambios guardados.\n");
    }
}

static void borrar_vehiculo(Vehiculo *vehiculos, int *cantidad)
{
    int indice = elegir_vehiculo(vehiculos, *cantidad);
    if (indice < 0) return;

    printf("  Se borrara [%d] %s. Confirma? (s/n): ", vehiculos[indice].id, vehiculos[indice].nombre);
    char linea[16];
    leer_linea(linea, sizeof linea);
    if (linea[0] != 's' && linea[0] != 'S') {
        printf("  Cancelado.\n");
        return;
    }

    for (int i = indice; i + 1 < *cantidad; i++)
        vehiculos[i] = vehiculos[i + 1];
    (*cantidad)--;
    if (guardar_vehiculos(vehiculos, *cantidad))
        printf("  Vehiculo borrado.\n");
}

/* ------------------------------------------------------------------ */
/* Calculo del viaje                                                   */
/* ------------------------------------------------------------------ */

static void imprimir_item(const char *nombre, double valor, double km_total, double total)
{
    printf("  %-14s $%12.2f   $%8.4f/km   %5.1f%%\n",
           nombre, valor, km_total > 0.0 ? valor / km_total : 0.0,
           total > 0.0 ? 100.0 * valor / total : 0.0);
}

static void calcular_viaje(const Vehiculo *vehiculos, int cantidad)
{
    int indice = elegir_vehiculo(vehiculos, cantidad);
    if (indice < 0) return;
    const Vehiculo *v = &vehiculos[indice];

    printf("\n-- Calculo de viaje con [%d] %s --\n", v->id, v->nombre);

    double km_total = pedir_double_positivo("Distancia total del viaje (km)", 0, 0.0);

    double pct_carretera, pct_ciudad;
    for (;;) {
        pct_carretera = pedir_double_no_negativo("Porcentaje en carretera (%)", 0, 0.0);
        pct_ciudad    = pedir_double_no_negativo("Porcentaje en ciudad (%)", 0, 0.0);
        if (pct_carretera + pct_ciudad <= 0.0) {
            printf("  La suma de los porcentajes debe ser mayor que cero.\n");
            continue;
        }
        break;
    }
    double suma = pct_carretera + pct_ciudad;
    if (suma < 99.99 || suma > 100.01)
        printf("  Nota: los porcentajes suman %.2f%%, se normalizan a 100%%.\n", suma);

    double precio_litro = pedir_double_positivo("Precio del combustible por litro ($)", 0, 0.0);

    Resultado r;
    memset(&r, 0, sizeof r);
    r.km_total     = km_total;
    r.km_carretera = km_total * pct_carretera / suma;
    r.km_ciudad    = km_total - r.km_carretera;

    r.litros = r.km_ciudad / v->consumo_ciudad + r.km_carretera / v->consumo_carretera;
    r.combustible    = r.litros * precio_litro;
    r.neumaticos     = v->neumaticos_por_km * km_total;
    r.mantenimiento  = v->mantenimiento_por_km * km_total;

    double km_anio = v->km_por_anio;
    r.seguro       = v->seguro_anual / km_anio * km_total;
    r.impuesto     = v->impuesto_anual / km_anio * km_total;
    r.limpieza     = v->limpieza_anual / km_anio * km_total;
    r.depreciacion = 0.0;
    if (v->anios_uso > 0.0)
        r.depreciacion = (v->valor_inicial - v->valor_final) / v->anios_uso / km_anio * km_total;

    r.total  = r.combustible + r.neumaticos + r.mantenimiento +
               r.seguro + r.impuesto + r.limpieza + r.depreciacion;
    r.por_km = r.total / km_total;

    printf("\n================ RESULTADO DEL VIAJE ================\n");
    printf("  Vehiculo.......: %s\n", v->nombre);
    printf("  Distancia......: %.2f km (carretera %.2f km, ciudad %.2f km)\n",
           r.km_total, r.km_carretera, r.km_ciudad);
    printf("  Combustible....: %.2f litros a $%.2f/l\n", r.litros, precio_litro);
    printf("\n  %-14s %14s   %13s   %6s\n", "Concepto", "Costo", "Por km", "% total");
    printf("  ------------------------------------------------------------\n");
    imprimir_item("Combustible", r.combustible, km_total, r.total);
    imprimir_item("Neumaticos", r.neumaticos, km_total, r.total);
    imprimir_item("Mantenimiento", r.mantenimiento, km_total, r.total);
    imprimir_item("Seguro", r.seguro, km_total, r.total);
    imprimir_item("Impuesto", r.impuesto, km_total, r.total);
    imprimir_item("Limpieza", r.limpieza, km_total, r.total);
    imprimir_item("Depreciacion", r.depreciacion, km_total, r.total);
    printf("  ------------------------------------------------------------\n");
    printf("  COSTO TOTAL DEL VIAJE: $%.2f\n", r.total);
    printf("  COSTO POR KILOMETRO.: $%.4f/km\n", r.por_km);
    printf("  (Costos fijos prorrateados usando %.0f km/anio)\n", v->km_por_anio);
    printf("=====================================================\n");
}

/* ------------------------------------------------------------------ */
/* Programa principal                                                  */
/* ------------------------------------------------------------------ */

static void mostrar_menu(void)
{
    printf("\n====== CALCULADORA DE COSTO POR KILOMETRO ======\n");
    printf("  1) Crear vehiculo\n");
    printf("  2) Listar vehiculos\n");
    printf("  3) Editar vehiculo\n");
    printf("  4) Borrar vehiculo\n");
    printf("  5) Calcular costo de viaje\n");
    printf("  0) Salir\n");
    printf("================================================\n");
}

int main(void)
{
    Vehiculo vehiculos[MAX_VEHICULOS];
    int cantidad = 0;

    cargar_vehiculos(vehiculos, &cantidad);

    for (;;) {
        mostrar_menu();
        int opcion = pedir_opcion("  Opcion: ", 0, 5);

        switch (opcion) {
        case 0:
            printf("Hasta pronto.\n");
            return 0;
        case 1: crear_vehiculo(vehiculos, &cantidad); break;
        case 2: listar_vehiculos(vehiculos, cantidad); break;
        case 3: editar_vehiculo(vehiculos, cantidad); break;
        case 4: borrar_vehiculo(vehiculos, &cantidad); break;
        case 5: calcular_viaje(vehiculos, cantidad); break;
        }
    }
}
