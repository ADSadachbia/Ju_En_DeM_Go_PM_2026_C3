/*
 * TAREA 8 - Periodo 3
 * Programa de prestamos para compra de vehiculos
 *
 * - Gestion de vehiculos (marca, modelo, anio)
 * - Gestion de clientes (agregar, buscar, borrar)
 * - Listas enlazadas + memoria dinamica (malloc/calloc/realloc/free)
 * - Datos guardados y cargados desde un archivo JSON (datos.json)
 * - Pagos de abono y pagos mensuales (cuotas)
 * - Reporte de estado de pago (meses, pagados / no pagados, saldo)
 *
 * Compilar:  gcc prestamos_vehiculos.c -o prestamos -Wall
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define ARCHIVO "datos.json"

/* ===================== ESTRUCTURAS (listas enlazadas) ===================== */

typedef struct Vehiculo {
    int id;
    char marca[40];
    char modelo[40];
    int anio;
    double precio;
    struct Vehiculo *sig;
} Vehiculo;

typedef struct Cliente {
    int id;
    char nombre[60];
    char cedula[20];
    char telefono[20];
    struct Cliente *sig;
} Cliente;

typedef struct Pago {
    int id;
    int tipo;           /* 1 = cuota mensual, 2 = abono */
    int mes;            /* numero de cuota (solo tipo 1) */
    double monto;
    char fecha[16];
    struct Pago *sig;
} Pago;

typedef struct Prestamo {
    int id;
    int idCliente;
    int idVehiculo;
    double inicial;     /* pago inicial */
    double monto;       /* precio - inicial */
    double tasa;        /* tasa anual en % (interes simple) */
    int meses;          /* plazo */
    double total;       /* monto + intereses */
    double cuota;       /* total / meses */
    Pago *pagos;        /* lista de pagos del prestamo */
    struct Prestamo *sig;
} Prestamo;

static Vehiculo *vehiculos = NULL;
static Cliente  *clientes  = NULL;
static Prestamo *prestamos = NULL;

/* ===================== ENTRADA DE DATOS ===================== */

static void leerLinea(const char *msg, char *buf, int n)
{
    printf("%s", msg);
    if (!fgets(buf, n, stdin)) {
        printf("\nFin de la entrada.\n");
        exit(0);
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static int leerInt(const char *msg)
{
    char b[64], *fin;
    long v;
    for (;;) {
        leerLinea(msg, b, sizeof b);
        v = strtol(b, &fin, 10);
        if (fin != b && *fin == '\0') return (int)v;
        printf("Entrada invalida, intente de nuevo.\n");
    }
}

static double leerDouble(const char *msg)
{
    char b[64], *fin;
    double v;
    for (;;) {
        leerLinea(msg, b, sizeof b);
        v = strtod(b, &fin);
        if (fin != b && *fin == '\0') return v;
        printf("Entrada invalida, intente de nuevo.\n");
    }
}

/* busca "buscado" dentro de "texto" sin distinguir mayusculas */
static int contiene(const char *texto, const char *buscado)
{
    size_t i, j, lt = strlen(texto), lb = strlen(buscado);
    if (lb == 0) return 1;
    for (i = 0; i + lb <= lt; i++) {
        for (j = 0; j < lb; j++)
            if (tolower((unsigned char)texto[i + j]) != tolower((unsigned char)buscado[j]))
                break;
        if (j == lb) return 1;
    }
    return 0;
}

static void fechaHoy(char *dest, size_t n)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(dest, n, "%Y-%m-%d", tm);
}

/* ===================== JSON: ESCRITURA ===================== */

static void escribirCadena(FILE *f, const char *s)
{
    fputc('"', f);
    for (; *s; s++) {
        if (*s == '"' || *s == '\\') { fputc('\\', f); fputc(*s, f); }
        else if (*s == '\n') fputs("\\n", f);
        else if (*s == '\t') fputs("\\t", f);
        else fputc(*s, f);
    }
    fputc('"', f);
}

static void guardarDatos(void)
{
    FILE *f = fopen(ARCHIVO, "w");
    Vehiculo *v; Cliente *c; Prestamo *p; Pago *g;

    if (!f) { printf("No se pudo guardar en %s\n", ARCHIVO); return; }

    fprintf(f, "{\n  \"vehiculos\": [");
    for (v = vehiculos; v; v = v->sig) {
        fprintf(f, "%s\n    {\"id\": %d, \"marca\": ", v == vehiculos ? "" : ",", v->id);
        escribirCadena(f, v->marca);
        fprintf(f, ", \"modelo\": ");
        escribirCadena(f, v->modelo);
        fprintf(f, ", \"anio\": %d, \"precio\": %.2f}", v->anio, v->precio);
    }
    fprintf(f, "\n  ],\n  \"clientes\": [");
    for (c = clientes; c; c = c->sig) {
        fprintf(f, "%s\n    {\"id\": %d, \"nombre\": ", c == clientes ? "" : ",", c->id);
        escribirCadena(f, c->nombre);
        fprintf(f, ", \"cedula\": ");
        escribirCadena(f, c->cedula);
        fprintf(f, ", \"telefono\": ");
        escribirCadena(f, c->telefono);
        fprintf(f, "}");
    }
    fprintf(f, "\n  ],\n  \"prestamos\": [");
    for (p = prestamos; p; p = p->sig) {
        fprintf(f, "%s\n    {\"id\": %d, \"idCliente\": %d, \"idVehiculo\": %d, "
                   "\"inicial\": %.2f, \"monto\": %.2f, \"tasa\": %.2f, \"meses\": %d, "
                   "\"total\": %.2f, \"cuota\": %.2f,\n     \"pagos\": [",
                p == prestamos ? "" : ",", p->id, p->idCliente, p->idVehiculo,
                p->inicial, p->monto, p->tasa, p->meses, p->total, p->cuota);
        for (g = p->pagos; g; g = g->sig) {
            fprintf(f, "%s\n       {\"id\": %d, \"tipo\": %d, \"mes\": %d, \"monto\": %.2f, \"fecha\": ",
                    g == p->pagos ? "" : ",", g->id, g->tipo, g->mes, g->monto);
            escribirCadena(f, g->fecha);
            fprintf(f, "}");
        }
        fprintf(f, "\n     ]}");
    }
    fprintf(f, "\n  ]\n}\n");
    fclose(f);
}

/* ===================== JSON: LECTURA (mini parser) ===================== */

typedef enum { J_NULL, J_NUM, J_STR, J_ARR, J_OBJ } JTipo;

typedef struct JNodo {
    JTipo tipo;
    char *clave;            /* nombre si es miembro de un objeto */
    char *str;              /* valor si es cadena */
    double num;             /* valor si es numero */
    struct JNodo *hijo;     /* primer hijo (objeto o arreglo) */
    struct JNodo *sig;      /* siguiente hermano */
} JNodo;

static const char *cursor;

static void liberarJ(JNodo *n)
{
    JNodo *h, *sig;
    if (!n) return;
    for (h = n->hijo; h; h = sig) {
        sig = h->sig;
        liberarJ(h);
    }
    free(n->clave);
    free(n->str);
    free(n);
}

static void saltar(void)
{
    while (isspace((unsigned char)*cursor)) cursor++;
}

static char *parsearCadena(void)
{
    size_t cap = 32, len = 0;
    char *s, *t, c;

    if (*cursor != '"') return NULL;
    cursor++;
    s = malloc(cap);
    if (!s) return NULL;
    while (*cursor && *cursor != '"') {
        c = *cursor++;
        if (c == '\\' && *cursor) {
            c = *cursor++;
            if (c == 'n') c = '\n';
            else if (c == 't') c = '\t';
        }
        if (len + 1 >= cap) {
            cap *= 2;
            t = realloc(s, cap);
            if (!t) { free(s); return NULL; }
            s = t;
        }
        s[len++] = c;
    }
    if (*cursor != '"') { free(s); return NULL; }
    cursor++;
    s[len] = '\0';
    return s;
}

static JNodo *parsearValor(void)
{
    JNodo *n, *h, **cola;
    char *clave, *fin, cierre;

    saltar();
    n = calloc(1, sizeof *n);
    if (!n) return NULL;

    if (*cursor == '{' || *cursor == '[') {
        n->tipo = (*cursor == '{') ? J_OBJ : J_ARR;
        cierre  = (*cursor == '{') ? '}' : ']';
        cursor++;
        cola = &n->hijo;
        for (;;) {
            saltar();
            if (*cursor == cierre) { cursor++; break; }
            clave = NULL;
            if (n->tipo == J_OBJ) {
                clave = parsearCadena();
                if (!clave) { liberarJ(n); return NULL; }
                saltar();
                if (*cursor != ':') { free(clave); liberarJ(n); return NULL; }
                cursor++;
            }
            h = parsearValor();
            if (!h) { free(clave); liberarJ(n); return NULL; }
            h->clave = clave;
            *cola = h;
            cola = &h->sig;
            saltar();
            if (*cursor == ',') cursor++;
            else if (*cursor != cierre) { liberarJ(n); return NULL; }
        }
    } else if (*cursor == '"') {
        n->tipo = J_STR;
        n->str = parsearCadena();
        if (!n->str) { liberarJ(n); return NULL; }
    } else if (*cursor == '-' || isdigit((unsigned char)*cursor)) {
        n->tipo = J_NUM;
        n->num = strtod(cursor, &fin);
        cursor = fin;
    } else if (strncmp(cursor, "true", 4) == 0)  { n->tipo = J_NUM;  n->num = 1; cursor += 4; }
    else if (strncmp(cursor, "false", 5) == 0)   { n->tipo = J_NUM;  n->num = 0; cursor += 5; }
    else if (strncmp(cursor, "null", 4) == 0)    { n->tipo = J_NULL; cursor += 4; }
    else { liberarJ(n); return NULL; }

    return n;
}

static JNodo *obtener(JNodo *obj, const char *clave)
{
    JNodo *h;
    if (!obj) return NULL;
    for (h = obj->hijo; h; h = h->sig)
        if (h->clave && strcmp(h->clave, clave) == 0) return h;
    return NULL;
}

static double jnum(JNodo *obj, const char *clave)
{
    JNodo *h = obtener(obj, clave);
    return (h && h->tipo == J_NUM) ? h->num : 0;
}

static const char *jstr(JNodo *obj, const char *clave)
{
    JNodo *h = obtener(obj, clave);
    return (h && h->tipo == J_STR) ? h->str : "";
}

static void cargarDatos(void)
{
    FILE *f = fopen(ARCHIVO, "rb");
    long n;
    char *buf;
    JNodo *raiz, *arr, *e, *ep;
    Vehiculo *v, **pv = &vehiculos;
    Cliente *c, **pc = &clientes;
    Prestamo *p, **pp = &prestamos;
    Pago *g, **pg;

    if (!f) return;                      /* primera ejecucion: no hay archivo */
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    rewind(f);
    buf = malloc(n + 1);
    if (!buf) { fclose(f); return; }
    n = (long)fread(buf, 1, n, f);
    buf[n] = '\0';
    fclose(f);

    cursor = buf;
    raiz = parsearValor();
    free(buf);
    if (!raiz || raiz->tipo != J_OBJ) {
        printf("Aviso: %s no tiene un JSON valido. Se inicia vacio.\n", ARCHIVO);
        liberarJ(raiz);
        return;
    }

    arr = obtener(raiz, "vehiculos");
    if (arr) for (e = arr->hijo; e; e = e->sig) {
        v = calloc(1, sizeof *v);
        if (!v) break;
        v->id = (int)jnum(e, "id");
        snprintf(v->marca, sizeof v->marca, "%s", jstr(e, "marca"));
        snprintf(v->modelo, sizeof v->modelo, "%s", jstr(e, "modelo"));
        v->anio = (int)jnum(e, "anio");
        v->precio = jnum(e, "precio");
        *pv = v; pv = &v->sig;
    }

    arr = obtener(raiz, "clientes");
    if (arr) for (e = arr->hijo; e; e = e->sig) {
        c = calloc(1, sizeof *c);
        if (!c) break;
        c->id = (int)jnum(e, "id");
        snprintf(c->nombre, sizeof c->nombre, "%s", jstr(e, "nombre"));
        snprintf(c->cedula, sizeof c->cedula, "%s", jstr(e, "cedula"));
        snprintf(c->telefono, sizeof c->telefono, "%s", jstr(e, "telefono"));
        *pc = c; pc = &c->sig;
    }

    arr = obtener(raiz, "prestamos");
    if (arr) for (e = arr->hijo; e; e = e->sig) {
        p = calloc(1, sizeof *p);
        if (!p) break;
        p->id = (int)jnum(e, "id");
        p->idCliente = (int)jnum(e, "idCliente");
        p->idVehiculo = (int)jnum(e, "idVehiculo");
        p->inicial = jnum(e, "inicial");
        p->monto = jnum(e, "monto");
        p->tasa = jnum(e, "tasa");
        p->meses = (int)jnum(e, "meses");
        p->total = jnum(e, "total");
        p->cuota = jnum(e, "cuota");
        pg = &p->pagos;
        if (obtener(e, "pagos")) for (ep = obtener(e, "pagos")->hijo; ep; ep = ep->sig) {
            g = calloc(1, sizeof *g);
            if (!g) break;
            g->id = (int)jnum(ep, "id");
            g->tipo = (int)jnum(ep, "tipo");
            g->mes = (int)jnum(ep, "mes");
            g->monto = jnum(ep, "monto");
            snprintf(g->fecha, sizeof g->fecha, "%s", jstr(ep, "fecha"));
            *pg = g; pg = &g->sig;
        }
        *pp = p; pp = &p->sig;
    }

    liberarJ(raiz);
}

/* ===================== BUSQUEDAS E IDS ===================== */

static Vehiculo *buscarVehiculo(int id)
{
    Vehiculo *v;
    for (v = vehiculos; v; v = v->sig) if (v->id == id) return v;
    return NULL;
}

static Cliente *buscarCliente(int id)
{
    Cliente *c;
    for (c = clientes; c; c = c->sig) if (c->id == id) return c;
    return NULL;
}

static int nuevoIdVehiculo(void)
{
    int m = 0; Vehiculo *v;
    for (v = vehiculos; v; v = v->sig) if (v->id > m) m = v->id;
    return m + 1;
}

static int nuevoIdCliente(void)
{
    int m = 0; Cliente *c;
    for (c = clientes; c; c = c->sig) if (c->id > m) m = c->id;
    return m + 1;
}

static int nuevoIdPrestamo(void)
{
    int m = 0; Prestamo *p;
    for (p = prestamos; p; p = p->sig) if (p->id > m) m = p->id;
    return m + 1;
}

/* ===================== VEHICULOS ===================== */

static void mostrarVehiculo(const Vehiculo *v)
{
    printf("%-4d %-15s %-15s %-6d %12.2f\n", v->id, v->marca, v->modelo, v->anio, v->precio);
}

static void listarVehiculos(void)
{
    Vehiculo *v;
    if (!vehiculos) { printf("No hay vehiculos registrados.\n"); return; }
    printf("\n%-4s %-15s %-15s %-6s %12s\n", "ID", "Marca", "Modelo", "Anio", "Precio");
    for (v = vehiculos; v; v = v->sig) mostrarVehiculo(v);
}

static void agregarVehiculo(void)
{
    Vehiculo *v = calloc(1, sizeof *v), **pp = &vehiculos;
    if (!v) { printf("Sin memoria.\n"); return; }

    v->id = nuevoIdVehiculo();
    leerLinea("Marca: ", v->marca, sizeof v->marca);
    leerLinea("Modelo: ", v->modelo, sizeof v->modelo);
    v->anio = leerInt("Anio: ");
    v->precio = leerDouble("Precio: ");
    if (v->marca[0] == '\0' || v->modelo[0] == '\0' || v->anio < 1900 || v->precio <= 0) {
        printf("Datos invalidos. Vehiculo no agregado.\n");
        free(v);
        return;
    }
    while (*pp) pp = &(*pp)->sig;     /* agregar al final de la lista */
    *pp = v;
    guardarDatos();
    printf("Vehiculo agregado con ID %d.\n", v->id);
}

static void buscarVehiculos(void)
{
    char t[40];
    Vehiculo *v;
    int hallados = 0;
    leerLinea("Buscar por marca o modelo: ", t, sizeof t);
    for (v = vehiculos; v; v = v->sig)
        if (contiene(v->marca, t) || contiene(v->modelo, t)) {
            if (!hallados) printf("\n%-4s %-15s %-15s %-6s %12s\n", "ID", "Marca", "Modelo", "Anio", "Precio");
            mostrarVehiculo(v);
            hallados++;
        }
    if (!hallados) printf("Sin resultados.\n");
}

static void borrarVehiculo(void)
{
    Vehiculo **pp, *v;
    Prestamo *p;
    int id;

    listarVehiculos();
    if (!vehiculos) return;
    id = leerInt("ID del vehiculo a borrar: ");
    for (p = prestamos; p; p = p->sig)
        if (p->idVehiculo == id) {
            printf("No se puede borrar: el vehiculo tiene un prestamo asociado.\n");
            return;
        }
    for (pp = &vehiculos; *pp; pp = &(*pp)->sig)
        if ((*pp)->id == id) {
            v = *pp;
            *pp = v->sig;
            free(v);
            guardarDatos();
            printf("Vehiculo borrado.\n");
            return;
        }
    printf("No existe un vehiculo con ese ID.\n");
}

static void menuVehiculos(void)
{
    int op;
    do {
        printf("\n--- VEHICULOS ---\n1. Agregar\n2. Listar\n3. Buscar\n4. Borrar\n0. Volver\n");
        op = leerInt("Opcion: ");
        switch (op) {
            case 1: agregarVehiculo(); break;
            case 2: listarVehiculos(); break;
            case 3: buscarVehiculos(); break;
            case 4: borrarVehiculo(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (op != 0);
}

/* ===================== CLIENTES ===================== */

static void mostrarCliente(const Cliente *c)
{
    printf("%-4d %-28s %-15s %-15s\n", c->id, c->nombre, c->cedula, c->telefono);
}

static void listarClientes(void)
{
    Cliente *c;
    if (!clientes) { printf("No hay clientes registrados.\n"); return; }
    printf("\n%-4s %-28s %-15s %-15s\n", "ID", "Nombre", "Cedula", "Telefono");
    for (c = clientes; c; c = c->sig) mostrarCliente(c);
}

static void agregarCliente(void)
{
    Cliente *c = calloc(1, sizeof *c), *o, **pp = &clientes;
    if (!c) { printf("Sin memoria.\n"); return; }

    c->id = nuevoIdCliente();
    leerLinea("Nombre: ", c->nombre, sizeof c->nombre);
    leerLinea("Cedula: ", c->cedula, sizeof c->cedula);
    leerLinea("Telefono: ", c->telefono, sizeof c->telefono);
    if (c->nombre[0] == '\0' || c->cedula[0] == '\0') {
        printf("Nombre y cedula son obligatorios. Cliente no agregado.\n");
        free(c);
        return;
    }
    for (o = clientes; o; o = o->sig)
        if (strcmp(o->cedula, c->cedula) == 0) {
            printf("Ya existe un cliente con esa cedula.\n");
            free(c);
            return;
        }
    while (*pp) pp = &(*pp)->sig;
    *pp = c;
    guardarDatos();
    printf("Cliente agregado con ID %d.\n", c->id);
}

static void buscarClientes(void)
{
    char t[60];
    Cliente *c;
    int hallados = 0;
    leerLinea("Buscar por nombre o cedula: ", t, sizeof t);
    for (c = clientes; c; c = c->sig)
        if (contiene(c->nombre, t) || contiene(c->cedula, t)) {
            if (!hallados) printf("\n%-4s %-28s %-15s %-15s\n", "ID", "Nombre", "Cedula", "Telefono");
            mostrarCliente(c);
            hallados++;
        }
    if (!hallados) printf("Sin resultados.\n");
}

static void borrarCliente(void)
{
    Cliente **pp, *c;
    Prestamo *p;
    int id;

    listarClientes();
    if (!clientes) return;
    id = leerInt("ID del cliente a borrar: ");
    for (p = prestamos; p; p = p->sig)
        if (p->idCliente == id) {
            printf("No se puede borrar: el cliente tiene un prestamo asociado.\n");
            return;
        }
    for (pp = &clientes; *pp; pp = &(*pp)->sig)
        if ((*pp)->id == id) {
            c = *pp;
            *pp = c->sig;
            free(c);
            guardarDatos();
            printf("Cliente borrado.\n");
            return;
        }
    printf("No existe un cliente con ese ID.\n");
}

static void menuClientes(void)
{
    int op;
    do {
        printf("\n--- CLIENTES ---\n1. Agregar\n2. Listar\n3. Buscar\n4. Borrar\n0. Volver\n");
        op = leerInt("Opcion: ");
        switch (op) {
            case 1: agregarCliente(); break;
            case 2: listarClientes(); break;
            case 3: buscarClientes(); break;
            case 4: borrarCliente(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (op != 0);
}

/* ===================== PRESTAMOS Y PAGOS ===================== */

static double totalPagado(const Prestamo *p)
{
    double s = 0;
    Pago *g;
    for (g = p->pagos; g; g = g->sig) s += g->monto;
    return s;
}

static double saldoPrestamo(const Prestamo *p)
{
    double s = p->total - totalPagado(p);
    return (s < 0.005) ? 0 : s;
}

static Prestamo *buscarPrestamo(int id)
{
    Prestamo *p;
    for (p = prestamos; p; p = p->sig) if (p->id == id) return p;
    return NULL;
}

static void listarPrestamos(void)
{
    Prestamo *p;
    Cliente *c;
    Vehiculo *v;
    if (!prestamos) { printf("No hay prestamos registrados.\n"); return; }
    printf("\n%-4s %-22s %-22s %12s %12s\n", "ID", "Cliente", "Vehiculo", "Cuota", "Saldo");
    for (p = prestamos; p; p = p->sig) {
        char veh[96];
        c = buscarCliente(p->idCliente);
        v = buscarVehiculo(p->idVehiculo);
        if (v) snprintf(veh, sizeof veh, "%s %s", v->marca, v->modelo);
        else   snprintf(veh, sizeof veh, "(desconocido)");
        printf("%-4d %-22.22s %-22.22s %12.2f %12.2f\n", p->id,
               c ? c->nombre : "(desconocido)", veh, p->cuota, saldoPrestamo(p));
    }
}

static void crearPrestamo(void)
{
    Cliente *c;
    Vehiculo *v;
    Prestamo *p, **pp = &prestamos;

    if (!clientes || !vehiculos) {
        printf("Debe registrar al menos un cliente y un vehiculo primero.\n");
        return;
    }
    listarClientes();
    c = buscarCliente(leerInt("ID del cliente: "));
    if (!c) { printf("Cliente no encontrado.\n"); return; }
    listarVehiculos();
    v = buscarVehiculo(leerInt("ID del vehiculo: "));
    if (!v) { printf("Vehiculo no encontrado.\n"); return; }

    p = calloc(1, sizeof *p);
    if (!p) { printf("Sin memoria.\n"); return; }

    p->id = nuevoIdPrestamo();
    p->idCliente = c->id;
    p->idVehiculo = v->id;
    p->inicial = leerDouble("Pago inicial: ");
    p->meses = leerInt("Plazo en meses: ");
    p->tasa = leerDouble("Tasa de interes anual (%): ");
    if (p->inicial < 0 || p->inicial >= v->precio || p->meses <= 0 || p->tasa < 0) {
        printf("Datos invalidos. Prestamo no creado.\n");
        free(p);
        return;
    }
    p->monto = v->precio - p->inicial;
    p->total = p->monto * (1.0 + (p->tasa / 100.0) * p->meses / 12.0);   /* interes simple */
    p->cuota = p->total / p->meses;

    while (*pp) pp = &(*pp)->sig;
    *pp = p;
    guardarDatos();
    printf("\nPrestamo %d creado. Monto financiado: %.2f | Total a pagar: %.2f | Cuota mensual: %.2f\n",
           p->id, p->monto, p->total, p->cuota);
}

/* devuelve un arreglo dinamico [1..meses] con 1 en los meses ya pagados */
static int *mesesPagados(const Prestamo *p)
{
    int *flags = calloc(p->meses + 1, sizeof(int));
    Pago *g;
    if (!flags) return NULL;
    for (g = p->pagos; g; g = g->sig)
        if (g->tipo == 1 && g->mes >= 1 && g->mes <= p->meses) flags[g->mes] = 1;
    return flags;
}

static void agregarPago(Prestamo *p, int tipo, int mes, double monto)
{
    Pago *g = calloc(1, sizeof *g), **pp = &p->pagos;
    int n = 0;
    if (!g) { printf("Sin memoria.\n"); return; }
    for (Pago *t = p->pagos; t; t = t->sig) n++;
    g->id = n + 1;
    g->tipo = tipo;
    g->mes = mes;
    g->monto = monto;
    fechaHoy(g->fecha, sizeof g->fecha);
    while (*pp) pp = &(*pp)->sig;
    *pp = g;
    guardarDatos();
}

static Prestamo *elegirPrestamo(void)
{
    Prestamo *p;
    if (!prestamos) { printf("No hay prestamos registrados.\n"); return NULL; }
    listarPrestamos();
    p = buscarPrestamo(leerInt("ID del prestamo: "));
    if (!p) printf("Prestamo no encontrado.\n");
    return p;
}

static void pagarCuota(void)
{
    Prestamo *p = elegirPrestamo();
    int *pagados, mes, sugerido;
    double monto, saldo;
    char msg[80];

    if (!p) return;
    saldo = saldoPrestamo(p);
    if (saldo <= 0) { printf("Este prestamo ya esta saldado.\n"); return; }
    pagados = mesesPagados(p);
    if (!pagados) { printf("Sin memoria.\n"); return; }

    for (sugerido = 1; sugerido <= p->meses && pagados[sugerido]; sugerido++) ;
    if (sugerido > p->meses) {
        printf("Todas las cuotas ya fueron pagadas.\n");
        free(pagados);
        return;
    }
    printf("Cuota mensual: %.2f | Saldo: %.2f\n", p->cuota, saldo);
    snprintf(msg, sizeof msg, "Numero de mes a pagar (siguiente pendiente: %d): ", sugerido);
    mes = leerInt(msg);

    if (mes < 1 || mes > p->meses) {
        printf("Mes fuera del plazo (1 a %d).\n", p->meses);
    } else if (pagados[mes]) {
        printf("El mes %d ya esta pagado.\n", mes);
    } else {
        monto = (p->cuota > saldo) ? saldo : p->cuota;
        agregarPago(p, 1, mes, monto);
        printf("Pago de la cuota %d registrado: %.2f | Nuevo saldo: %.2f\n",
               mes, monto, saldoPrestamo(p));
    }
    free(pagados);
}

static void registrarAbono(void)
{
    Prestamo *p = elegirPrestamo();
    double monto, saldo;

    if (!p) return;
    saldo = saldoPrestamo(p);
    if (saldo <= 0) { printf("Este prestamo ya esta saldado.\n"); return; }
    printf("Saldo actual: %.2f\n", saldo);
    monto = leerDouble("Monto del abono: ");
    if (monto <= 0 || monto > saldo + 0.005) {
        printf("Monto invalido (debe ser mayor a 0 y no superar el saldo).\n");
        return;
    }
    agregarPago(p, 2, 0, monto);
    printf("Abono registrado. Nuevo saldo: %.2f\n", saldoPrestamo(p));
}

static void historialPagos(void)
{
    Prestamo *p = elegirPrestamo();
    Pago *g;
    if (!p) return;
    if (!p->pagos) { printf("Este prestamo no tiene pagos.\n"); return; }
    printf("\n%-4s %-12s %-10s %-6s %12s\n", "ID", "Fecha", "Tipo", "Mes", "Monto");
    for (g = p->pagos; g; g = g->sig) {
        printf("%-4d %-12s %-10s ", g->id, g->fecha, g->tipo == 1 ? "Cuota" : "Abono");
        if (g->tipo == 1) printf("%-6d", g->mes); else printf("%-6s", "-");
        printf(" %12.2f\n", g->monto);
    }
}

static void menuPagos(void)
{
    int op;
    do {
        printf("\n--- PAGOS ---\n1. Pagar cuota mensual\n2. Registrar abono\n3. Ver historial de pagos\n0. Volver\n");
        op = leerInt("Opcion: ");
        switch (op) {
            case 1: pagarCuota(); break;
            case 2: registrarAbono(); break;
            case 3: historialPagos(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (op != 0);
}

static void menuPrestamos(void)
{
    int op;
    do {
        printf("\n--- PRESTAMOS ---\n1. Crear prestamo\n2. Listar prestamos\n0. Volver\n");
        op = leerInt("Opcion: ");
        switch (op) {
            case 1: crearPrestamo(); break;
            case 2: listarPrestamos(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (op != 0);
}

/* ===================== REPORTE DE ESTADO DE PAGO ===================== */

static void reporteEstado(void)
{
    Prestamo *p = elegirPrestamo();
    Cliente *c;
    Vehiculo *v;
    int *pagados, m, cuotasPagadas = 0;
    double saldo, abonos = 0;
    Pago *g;

    if (!p) return;
    c = buscarCliente(p->idCliente);
    v = buscarVehiculo(p->idVehiculo);
    pagados = mesesPagados(p);
    if (!pagados) { printf("Sin memoria.\n"); return; }
    saldo = saldoPrestamo(p);
    for (g = p->pagos; g; g = g->sig) if (g->tipo == 2) abonos += g->monto;

    printf("\n==================================================\n");
    printf("        REPORTE DE ESTADO DE PAGO - PRESTAMO %d\n", p->id);
    printf("==================================================\n");
    printf("Cliente : %s (%s)\n", c ? c->nombre : "(desconocido)", c ? c->cedula : "-");
    if (v) printf("Vehiculo: %s %s %d\n", v->marca, v->modelo, v->anio);
    printf("Monto financiado: %.2f | Interes anual: %.2f%% | Plazo: %d meses\n", p->monto, p->tasa, p->meses);
    printf("Total a pagar: %.2f | Cuota mensual: %.2f\n\n", p->total, p->cuota);

    printf("%-6s %12s   %-10s\n", "Mes", "Cuota", "Estado");
    printf("--------------------------------------\n");
    for (m = 1; m <= p->meses; m++) {
        const char *estado;
        if (pagados[m]) { estado = "PAGADO"; cuotasPagadas++; }
        else if (saldo <= 0) estado = "CUBIERTO";
        else estado = "NO PAGADO";
        printf("%-6d %12.2f   %-10s\n", m, p->cuota, estado);
    }
    printf("--------------------------------------\n");
    printf("Cuotas pagadas : %d de %d\n", cuotasPagadas, p->meses);
    printf("Abonos         : %.2f\n", abonos);
    printf("Total pagado   : %.2f\n", totalPagado(p));
    printf("SALDO PENDIENTE: %.2f\n", saldo);
    if (saldo <= 0) printf("*** PRESTAMO SALDADO ***\n");
    free(pagados);
}

/* ===================== LIBERAR MEMORIA ===================== */

static void liberarTodo(void)
{
    Vehiculo *v; Cliente *c; Prestamo *p; Pago *g;
    while (vehiculos) { v = vehiculos; vehiculos = v->sig; free(v); }
    while (clientes)  { c = clientes;  clientes  = c->sig; free(c); }
    while (prestamos) {
        p = prestamos;
        prestamos = p->sig;
        while (p->pagos) { g = p->pagos; p->pagos = g->sig; free(g); }
        free(p);
    }
}

/* ===================== PROGRAMA PRINCIPAL ===================== */

int main(void)
{
    int op;

    cargarDatos();
    do {
        printf("\n========== PRESTAMOS PARA COMPRA DE VEHICULOS ==========\n");
        printf("1. Vehiculos\n2. Clientes\n3. Prestamos\n4. Pagos (cuotas y abonos)\n");
        printf("5. Reporte de estado de pago\n0. Salir\n");
        op = leerInt("Opcion: ");
        switch (op) {
            case 1: menuVehiculos(); break;
            case 2: menuClientes(); break;
            case 3: menuPrestamos(); break;
            case 4: menuPagos(); break;
            case 5: reporteEstado(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (op != 0);

    guardarDatos();
    liberarTodo();
    printf("Datos guardados en %s. Hasta luego.\n", ARCHIVO);
    return 0;
}
