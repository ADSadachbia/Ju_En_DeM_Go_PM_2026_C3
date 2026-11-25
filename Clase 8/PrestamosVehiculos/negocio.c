#include "model.h"
#include "json.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Utilidades generales                                                */
/* ------------------------------------------------------------------ */

char *util_strdup(const char *s) {
    size_t n;
    char *copy;
    if (!s) s = "";
    n = strlen(s) + 1;
    copy = (char *)malloc(n);
    if (!copy) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    memcpy(copy, s, n);
    return copy;
}

char *util_fecha_hoy(void) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char *fecha = (char *)malloc(16);
    if (!fecha) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    if (local) strftime(fecha, 16, "%Y-%m-%d", local);
    else strcpy(fecha, "0000-00-00");
    return fecha;
}

void util_leer_texto(const char *prompt, char *buffer, size_t size) {
    if (prompt) printf("%s", prompt);
    if (!fgets(buffer, (int)size, stdin)) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

int util_leer_entero(const char *prompt, int min, int max) {
    char line[128];
    long value;
    char *end;
    for (;;) {
        util_leer_texto(prompt, line, sizeof(line));
        value = strtol(line, &end, 10);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != line && *end == '\0' && value >= min && value <= max)
            return (int)value;
        printf("  Ingrese un número entero entre %d y %d.\n", min, max);
    }
}

double util_leer_double(const char *prompt, double min, double max) {
    char line[128];
    double value;
    char *end;
    for (;;) {
        util_leer_texto(prompt, line, sizeof(line));
        value = strtod(line, &end);
        while (*end && isspace((unsigned char)*end)) end++;
        if (end != line && *end == '\0' && value >= min && value <= max)
            return value;
        printf("  Ingrese un número entre %.2f y %.2f.\n", min, max);
    }
}

int util_confirmar(const char *prompt) {
    char line[32];
    util_leer_texto(prompt, line, sizeof(line));
    return line[0] == 's' || line[0] == 'S';
}

static int contains_ci(const char *haystack, const char *needle) {
    const char *h, *n;
    if (!needle || !*needle) return 1;
    if (!haystack) return 0;
    for (h = haystack; *h; h++) {
        n = needle;
        while (*h && *n &&
               tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }
        if (!*n) return 1;
        if (!*h) break;
    }
    return 0;
}

static int texto_es_numero(const char *text) {
    char *end;
    if (!text || !*text) return 0;
    strtod(text, &end);
    while (*end && isspace((unsigned char)*end)) end++;
    return *end == '\0';
}

/* ------------------------------------------------------------------ */
/* Ciclo de vida de la base de datos                                   */
/* ------------------------------------------------------------------ */

void database_init(Database *db) {
    if (!db) return;
    db->clientes = NULL;
    db->vehiculos = NULL;
    db->prestamos = NULL;
    db->next_cliente = 1;
    db->next_vehiculo = 1;
    db->next_prestamo = 1;
    db->next_abono = 1;
}

void database_free(Database *db) {
    Cliente *c, *cnext;
    Vehiculo *v, *vnext;
    Prestamo *p, *pnext;
    Abono *a, *anext;
    MesPago *m, *mnext;

    if (!db) return;

    for (c = db->clientes; c; c = cnext) {
        cnext = c->next;
        free(c->nombre);
        free(c->cedula);
        free(c->telefono);
        free(c);
    }

    for (v = db->vehiculos; v; v = vnext) {
        vnext = v->next;
        free(v->marca);
        free(v->modelo);
        free(v);
    }

    for (p = db->prestamos; p; p = pnext) {
        pnext = p->next;
        for (a = p->abonos; a; a = anext) {
            anext = a->next;
            free(a->fecha);
            free(a);
        }
        for (m = p->meses; m; m = mnext) {
            mnext = m->next;
            free(m->fecha_pago);
            free(m);
        }
        free(p);
    }

    database_init(db);
}

/* ------------------------------------------------------------------ */
/* Clientes                                                            */
/* ------------------------------------------------------------------ */

static Cliente *cliente_por_id(const Database *db, int id) {
    Cliente *c;
    for (c = db->clientes; c; c = c->next)
        if (c->id == id) return c;
    return NULL;
}

Cliente *cliente_agregar(Database *db, const char *nombre,
                         const char *cedula, const char *telefono) {
    Cliente *nuevo;
    Cliente *actual;

    if (!db) return NULL;
    nuevo = (Cliente *)calloc(1, sizeof(Cliente));
    if (!nuevo) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    nuevo->id = db->next_cliente++;
    nuevo->nombre = util_strdup(nombre);
    nuevo->cedula = util_strdup(cedula);
    nuevo->telefono = util_strdup(telefono);
    nuevo->next = NULL;

    if (!db->clientes) {
        db->clientes = nuevo;
    } else {
        for (actual = db->clientes; actual->next; actual = actual->next) { }
        actual->next = nuevo;
    }
    return nuevo;
}

Cliente *cliente_buscar(const Database *db, const char *texto) {
    Cliente *c;
    int id;
    if (!db || !texto) return NULL;
    if (texto_es_numero(texto)) {
        id = (int)strtol(texto, NULL, 10);
        printf("\n%-6s %-25s %-18s %-15s\n", "ID", "NOMBRE", "CÉDULA", "TELÉFONO");
        printf("------ ------------------------- ------------------ ---------------\n");
        for (c = db->clientes; c; c = c->next) {
            if (c->id == id || contains_ci(c->cedula, texto)) {
                printf("%-6d %-25s %-18s %-15s\n",
                       c->id, c->nombre, c->cedula, c->telefono);
                return c;
            }
        }
    } else {
        printf("\n%-6s %-25s %-18s %-15s\n", "ID", "NOMBRE", "CÉDULA", "TELÉFONO");
        printf("------ ------------------------- ------------------ ---------------\n");
        for (c = db->clientes; c; c = c->next) {
            if (contains_ci(c->nombre, texto) || contains_ci(c->cedula, texto)) {
                printf("%-6d %-25s %-18s %-15s\n",
                       c->id, c->nombre, c->cedula, c->telefono);
                return c;
            }
        }
    }
    printf("No se encontró ningún cliente con «%s».\n", texto);
    return NULL;
}

int cliente_borrar(Database *db, int id) {
    Cliente *actual, *anterior = NULL;
    Prestamo *p;

    if (!db) return 0;
    for (p = db->prestamos; p; p = p->next) {
        if (p->cliente_id == id) {
            printf("No se puede borrar: el cliente tiene el préstamo #%d asociado.\n", p->id);
            return 0;
        }
    }

    for (actual = db->clientes; actual; actual = actual->next) {
        if (actual->id == id) {
            if (anterior) anterior->next = actual->next;
            else db->clientes = actual->next;
            free(actual->nombre);
            free(actual->cedula);
            free(actual->telefono);
            free(actual);
            printf("Cliente #%d eliminado.\n", id);
            return 1;
        }
        anterior = actual;
    }
    printf("No existe un cliente con ID %d.\n", id);
    return 0;
}

void cliente_listar(const Database *db) {
    Cliente *c;
    int count = 0;
    if (!db) return;
    printf("\n%-6s %-25s %-18s %-15s\n", "ID", "NOMBRE", "CÉDULA", "TELÉFONO");
    printf("------ ------------------------- ------------------ ---------------\n");
    for (c = db->clientes; c; c = c->next) {
        printf("%-6d %-25s %-18s %-15s\n", c->id, c->nombre, c->cedula, c->telefono);
        count++;
    }
    if (count == 0) printf("(sin clientes registrados)\n");
    else printf("\nTotal: %d cliente(s)\n", count);
}

/* ------------------------------------------------------------------ */
/* Vehículos                                                           */
/* ------------------------------------------------------------------ */

static Vehiculo *vehiculo_por_id(const Database *db, int id) {
    Vehiculo *v;
    for (v = db->vehiculos; v; v = v->next)
        if (v->id == id) return v;
    return NULL;
}

Vehiculo *vehiculo_agregar(Database *db, const char *marca,
                           const char *modelo, int anio) {
    Vehiculo *nuevo;
    Vehiculo *actual;

    if (!db) return NULL;
    nuevo = (Vehiculo *)calloc(1, sizeof(Vehiculo));
    if (!nuevo) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    nuevo->id = db->next_vehiculo++;
    nuevo->marca = util_strdup(marca);
    nuevo->modelo = util_strdup(modelo);
    nuevo->anio = anio;

    if (!db->vehiculos) {
        db->vehiculos = nuevo;
    } else {
        for (actual = db->vehiculos; actual->next; actual = actual->next) { }
        actual->next = nuevo;
    }
    return nuevo;
}

Vehiculo *vehiculo_buscar(const Database *db, const char *texto) {
    Vehiculo *v;
    int found = 0;
    if (!db || !texto) return NULL;
    printf("\n%-6s %-18s %-20s %-8s\n", "ID", "MARCA", "MODELO", "AÑO");
    printf("------ ------------------ -------------------- --------\n");
    for (v = db->vehiculos; v; v = v->next) {
        char anio_text[16];
        sprintf(anio_text, "%d", v->anio);
        if (contains_ci(v->marca, texto) || contains_ci(v->modelo, texto) ||
            contains_ci(anio_text, texto)) {
            printf("%-6d %-18s %-20s %-8d\n", v->id, v->marca, v->modelo, v->anio);
            found = 1;
        }
    }
    if (!found) printf("No se encontró ningún vehículo con «%s».\n", texto);
    return db->vehiculos;
}

int vehiculo_borrar(Database *db, int id) {
    Vehiculo *actual, *anterior = NULL;
    Prestamo *p;

    if (!db) return 0;
    for (p = db->prestamos; p; p = p->next) {
        if (p->vehiculo_id == id) {
            printf("No se puede borrar: el vehículo tiene el préstamo #%d asociado.\n", p->id);
            return 0;
        }
    }

    for (actual = db->vehiculos; actual; actual = actual->next) {
        if (actual->id == id) {
            if (anterior) anterior->next = actual->next;
            else db->vehiculos = actual->next;
            free(actual->marca);
            free(actual->modelo);
            free(actual);
            printf("Vehículo #%d eliminado.\n", id);
            return 1;
        }
        anterior = actual;
    }
    printf("No existe un vehículo con ID %d.\n", id);
    return 0;
}

void vehiculo_listar(const Database *db) {
    Vehiculo *v;
    int count = 0;
    if (!db) return;
    printf("\n%-6s %-18s %-20s %-8s\n", "ID", "MARCA", "MODELO", "AÑO");
    printf("------ ------------------ -------------------- --------\n");
    for (v = db->vehiculos; v; v = v->next) {
        printf("%-6d %-18s %-20s %-8d\n", v->id, v->marca, v->modelo, v->anio);
        count++;
    }
    if (count == 0) printf("(sin vehículos registrados)\n");
    else printf("\nTotal: %d vehículo(s)\n", count);
}

/* ------------------------------------------------------------------ */
/* Cálculo financiero                                                  */
/* ------------------------------------------------------------------ */

double calcular_cuota(double capital, double tasa_anual, int plazo_meses) {
    double i;
    if (capital <= 0 || plazo_meses <= 0) return 0.0;
    if (tasa_anual <= 0) return capital / plazo_meses;
    i = (tasa_anual / 100.0) / 12.0;
    return capital * i / (1.0 - pow(1.0 + i, -plazo_meses));
}

/* ------------------------------------------------------------------ */
/* Préstamos                                                           */
/* ------------------------------------------------------------------ */

Prestamo *prestamo_buscar(const Database *db, int id) {
    Prestamo *p;
    if (!db) return NULL;
    for (p = db->prestamos; p; p = p->next)
        if (p->id == id) return p;
    return NULL;
}

Prestamo *prestamo_crear(Database *db, int cliente_id, int vehiculo_id,
                         double precio, double enganche,
                         double tasa_anual, int plazo_meses) {
    Prestamo *nuevo, *actual;
    MesPago *m, *ultimo_mes = NULL;
    Cliente *cliente;
    Vehiculo *vehiculo;
    int i;

    if (!db) return NULL;
    cliente = cliente_por_id(db, cliente_id);
    vehiculo = vehiculo_por_id(db, vehiculo_id);
    if (!cliente || !vehiculo) {
        printf("Error: cliente o vehículo no encontrado.\n");
        return NULL;
    }
    if (precio <= 0 || enganche < 0 || enganche >= precio) {
        printf("Error: el enganche debe ser mayor o igual a 0 y menor que el precio.\n");
        return NULL;
    }
    if (tasa_anual < 0 || plazo_meses <= 0) {
        printf("Error: tasa o plazo inválido.\n");
        return NULL;
    }

    nuevo = (Prestamo *)calloc(1, sizeof(Prestamo));
    if (!nuevo) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    nuevo->id = db->next_prestamo++;
    nuevo->cliente_id = cliente_id;
    nuevo->vehiculo_id = vehiculo_id;
    nuevo->precio = precio;
    nuevo->enganche = enganche;
    nuevo->tasa_anual = tasa_anual;
    nuevo->plazo_meses = plazo_meses;
    nuevo->cuota = calcular_cuota(precio - enganche, tasa_anual, plazo_meses);

    for (i = 1; i <= plazo_meses; i++) {
        m = (MesPago *)calloc(1, sizeof(MesPago));
        if (!m) {
            fputs("Error: memoria insuficiente.\n", stderr);
            exit(EXIT_FAILURE);
        }
        m->numero = i;
        m->monto = nuevo->cuota;
        m->pagado = 0;
        m->fecha_pago = util_strdup("");
        m->next = NULL;
        if (ultimo_mes) ultimo_mes->next = m;
        else nuevo->meses = m;
        ultimo_mes = m;
    }

    if (!db->prestamos) {
        db->prestamos = nuevo;
    } else {
        for (actual = db->prestamos; actual->next; actual = actual->next) { }
        actual->next = nuevo;
    }
    return nuevo;
}

int prestamo_borrar(Database *db, int id) {
    Prestamo *actual, *anterior = NULL;
    Abono *a, *anext;
    MesPago *m, *mnext;

    if (!db) return 0;
    for (actual = db->prestamos; actual; actual = actual->next) {
        if (actual->id == id) {
            if (anterior) anterior->next = actual->next;
            else db->prestamos = actual->next;
            for (a = actual->abonos; a; a = anext) {
                anext = a->next;
                free(a->fecha);
                free(a);
            }
            for (m = actual->meses; m; m = mnext) {
                mnext = m->next;
                free(m->fecha_pago);
                free(m);
            }
            free(actual);
            printf("Préstamo #%d eliminado.\n", id);
            return 1;
        }
        anterior = actual;
    }
    printf("No existe un préstamo con ID %d.\n", id);
    return 0;
}

void prestamo_listar(const Database *db) {
    Prestamo *p;
    int count = 0;
    if (!db) return;
    printf("\n%-6s %-10s %-10s %-10s %-10s %-12s\n",
           "ID", "CLIENTE", "VEHÍCULO", "MONTO", "CUOTA", "SALDO");
    printf("------ ---------- ---------- ---------- ---------- ------------\n");
    for (p = db->prestamos; p; p = p->next) {
        printf("%-6d %-10d %-10d %10.2f %10.2f %12.2f\n",
               p->id, p->cliente_id, p->vehiculo_id,
               p->precio, p->cuota, prestamo_saldo(p));
        count++;
    }
    if (count == 0) printf("(sin préstamos registrados)\n");
    else printf("\nTotal: %d préstamo(s)\n", count);
}

/* ------------------------------------------------------------------ */
/* Abonos                                                              */
/* ------------------------------------------------------------------ */

int abono_agregar(Database *db, Prestamo *prestamo, double monto, const char *fecha) {
    Abono *nuevo, *actual;
    if (!db || !prestamo || monto <= 0) return 0;

    nuevo = (Abono *)calloc(1, sizeof(Abono));
    if (!nuevo) {
        fputs("Error: memoria insuficiente.\n", stderr);
        exit(EXIT_FAILURE);
    }
    nuevo->id = db->next_abono++;
    nuevo->monto = monto;
    nuevo->fecha = util_strdup(fecha && *fecha ? fecha : "");

    if (!prestamo->abonos) {
        prestamo->abonos = nuevo;
    } else {
        for (actual = prestamo->abonos; actual->next; actual = actual->next) { }
        actual->next = nuevo;
    }
    return 1;
}

void abono_listar(const Prestamo *prestamo) {
    Abono *a;
    int count = 0;
    if (!prestamo) return;
    printf("\n  Abonos registrados:\n");
    printf("  %-6s %-14s %-12s\n", "ID", "FECHA", "MONTO");
    printf("  ------ -------------- ------------\n");
    for (a = prestamo->abonos; a; a = a->next) {
        printf("  %-6d %-14s %12.2f\n", a->id, a->fecha, a->monto);
        count++;
    }
    if (!count) printf("  (sin abonos)\n");
}

/* ------------------------------------------------------------------ */
/* Pagos mensuales                                                     */
/* ------------------------------------------------------------------ */

MesPago *mes_buscar(Prestamo *prestamo, int numero) {
    MesPago *m;
    if (!prestamo) return NULL;
    for (m = prestamo->meses; m; m = m->next)
        if (m->numero == numero) return m;
    return NULL;
}

int mes_marcar(Prestamo *prestamo, int numero, int pagado, const char *fecha) {
    MesPago *m = mes_buscar(prestamo, numero);
    if (!m) return 0;
    m->pagado = pagado ? 1 : 0;
    free(m->fecha_pago);
    if (pagado) {
        if (fecha && *fecha) m->fecha_pago = util_strdup(fecha);
        else m->fecha_pago = util_fecha_hoy();
    } else {
        m->fecha_pago = util_strdup("");
    }
    return 1;
}

void meses_listar(const Prestamo *prestamo) {
    MesPago *m;
    if (!prestamo) return;
    printf("\n  %-6s %-10s %-10s %-14s\n", "MES", "MONTO", "ESTADO", "FECHA PAGO");
    printf("  ------ ---------- ---------- --------------\n");
    for (m = prestamo->meses; m; m = m->next) {
        printf("  %-6d %10.2f %-10s %-14s\n",
               m->numero, m->monto,
               m->pagado ? "PAGADO" : "PENDIENTE",
               m->pagado && *m->fecha_pago ? m->fecha_pago : "-");
    }
}

/* ------------------------------------------------------------------ */
/* Totales y reporte                                                   */
/* ------------------------------------------------------------------ */

double prestamo_total_pagar(const Prestamo *prestamo) {
    if (!prestamo) return 0;
    return prestamo->enganche + prestamo->cuota * prestamo->plazo_meses;
}

double prestamo_pagado(const Prestamo *prestamo) {
    MesPago *m;
    Abono *a;
    double total;
    if (!prestamo) return 0;
    total = prestamo->enganche;
    for (m = prestamo->meses; m; m = m->next)
        if (m->pagado) total += m->monto;
    for (a = prestamo->abonos; a; a = a->next)
        total += a->monto;
    return total;
}

double prestamo_saldo(const Prestamo *prestamo) {
    return prestamo_total_pagar(prestamo) - prestamo_pagado(prestamo);
}

void prestamo_reporte(const Database *db, const Prestamo *prestamo) {
    Cliente *cliente;
    Vehiculo *vehiculo;
    MesPago *m;
    int pagados = 0, pendientes = 0;
    double saldo;

    if (!db || !prestamo) return;
    cliente = cliente_por_id(db, prestamo->cliente_id);
    vehiculo = vehiculo_por_id(db, prestamo->vehiculo_id);

    printf("\n============================================================\n");
    printf("            REPORTE DE ESTADO DE PAGO #%d\n", prestamo->id);
    printf("============================================================\n");
    printf(" Cliente : %s (ID %d)\n",
           cliente ? cliente->nombre : "(eliminado)", prestamo->cliente_id);
    printf(" Vehículo: %s %s %d (ID %d)\n",
           vehiculo ? vehiculo->marca : "?",
           vehiculo ? vehiculo->modelo : "?",
           vehiculo ? vehiculo->anio : 0,
           prestamo->vehiculo_id);
    printf(" Precio de contado : %12.2f\n", prestamo->precio);
    printf(" Enganche          : %12.2f\n", prestamo->enganche);
    printf(" Monto financiado  : %12.2f\n", prestamo->precio - prestamo->enganche);
    printf(" Tasa anual        : %11.2f %%\n", prestamo->tasa_anual);
    printf(" Plazo             : %d meses\n", prestamo->plazo_meses);
    printf(" Cuota mensual     : %12.2f\n", prestamo->cuota);
    printf(" Total a pagar     : %12.2f\n", prestamo_total_pagar(prestamo));
    printf(" Total pagado      : %12.2f\n", prestamo_pagado(prestamo));

    saldo = prestamo_saldo(prestamo);
    if (saldo >= 0) printf(" SALDO PENDIENTE    : %12.2f\n", saldo);
    else printf(" SALDO A FAVOR      : %12.2f\n", -saldo);

    for (m = prestamo->meses; m; m = m->next) {
        if (m->pagado) pagados++;
        else pendientes++;
    }
    printf(" Meses pagados     : %d de %d (%d pendientes)\n",
           pagados, prestamo->plazo_meses, pendientes);

    meses_listar(prestamo);
    abono_listar(prestamo);
    printf("============================================================\n");
}

/* ------------------------------------------------------------------ */
/* Persistencia JSON                                                   */
/* ------------------------------------------------------------------ */

static char *leer_archivo(const char *path) {
    FILE *file = fopen(path, "rb");
    char *content;
    long size;
    size_t read_bytes;

    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size < 0) { fclose(file); return NULL; }

    content = (char *)malloc((size_t)size + 1);
    if (!content) { fclose(file); return NULL; }
    read_bytes = fread(content, 1, (size_t)size, file);
    content[read_bytes] = '\0';
    fclose(file);
    return content;
}

int database_guardar(const Database *db, const char *path) {
    Json *root, *meta, *arr, *obj;
    Cliente *c;
    Vehiculo *v;
    Prestamo *p;
    Abono *a;
    MesPago *m;
    int ok;

    if (!db) return 0;
    root = json_object();

    meta = json_object();
    json_object_set(meta, "next_cliente", json_number(db->next_cliente));
    json_object_set(meta, "next_vehiculo", json_number(db->next_vehiculo));
    json_object_set(meta, "next_prestamo", json_number(db->next_prestamo));
    json_object_set(meta, "next_abono", json_number(db->next_abono));
    json_object_set(root, "meta", meta);

    arr = json_array();
    for (c = db->clientes; c; c = c->next) {
        obj = json_object();
        json_object_set(obj, "id", json_number(c->id));
        json_object_set(obj, "nombre", json_string(c->nombre));
        json_object_set(obj, "cedula", json_string(c->cedula));
        json_object_set(obj, "telefono", json_string(c->telefono));
        json_array_add(arr, obj);
    }
    json_object_set(root, "clientes", arr);

    arr = json_array();
    for (v = db->vehiculos; v; v = v->next) {
        obj = json_object();
        json_object_set(obj, "id", json_number(v->id));
        json_object_set(obj, "marca", json_string(v->marca));
        json_object_set(obj, "modelo", json_string(v->modelo));
        json_object_set(obj, "anio", json_number(v->anio));
        json_array_add(arr, obj);
    }
    json_object_set(root, "vehiculos", arr);

    arr = json_array();
    for (p = db->prestamos; p; p = p->next) {
        Json *abonos, *meses;

        obj = json_object();
        json_object_set(obj, "id", json_number(p->id));
        json_object_set(obj, "cliente_id", json_number(p->cliente_id));
        json_object_set(obj, "vehiculo_id", json_number(p->vehiculo_id));
        json_object_set(obj, "precio", json_number(p->precio));
        json_object_set(obj, "enganche", json_number(p->enganche));
        json_object_set(obj, "tasa_anual", json_number(p->tasa_anual));
        json_object_set(obj, "plazo_meses", json_number(p->plazo_meses));
        json_object_set(obj, "cuota", json_number(p->cuota));

        abonos = json_array();
        for (a = p->abonos; a; a = a->next) {
            Json *item = json_object();
            json_object_set(item, "id", json_number(a->id));
            json_object_set(item, "monto", json_number(a->monto));
            json_object_set(item, "fecha", json_string(a->fecha));
            json_array_add(abonos, item);
        }
        json_object_set(obj, "abonos", abonos);

        meses = json_array();
        for (m = p->meses; m; m = m->next) {
            Json *item = json_object();
            json_object_set(item, "numero", json_number(m->numero));
            json_object_set(item, "monto", json_number(m->monto));
            json_object_set(item, "pagado", json_bool(m->pagado));
            json_object_set(item, "fecha_pago", json_string(m->fecha_pago));
            json_array_add(meses, item);
        }
        json_object_set(obj, "meses", meses);

        json_array_add(arr, obj);
    }
    json_object_set(root, "prestamos", arr);

    ok = json_write_file(path, root);
    json_free(root);
    return ok;
}

static void cargar_clientes(Database *db, const Json *root) {
    Json *arr = json_get(root, "clientes");
    size_t i;
    for (i = 0; i < json_array_size(arr); i++) {
        Json *o = json_array_at(arr, i);
        Cliente *c = cliente_agregar(
            db,
            json_as_string(json_get(o, "nombre"), ""),
            json_as_string(json_get(o, "cedula"), ""),
            json_as_string(json_get(o, "telefono"), ""));
        if (c) c->id = json_as_int(json_get(o, "id"), c->id);
    }
}

static void cargar_vehiculos(Database *db, const Json *root) {
    Json *arr = json_get(root, "vehiculos");
    size_t i;
    for (i = 0; i < json_array_size(arr); i++) {
        Json *o = json_array_at(arr, i);
        Vehiculo *v = vehiculo_agregar(
            db,
            json_as_string(json_get(o, "marca"), ""),
            json_as_string(json_get(o, "modelo"), ""),
            json_as_int(json_get(o, "anio"), 0));
        if (v) v->id = json_as_int(json_get(o, "id"), v->id);
    }
}

static void cargar_abonos(Database *db, Prestamo *p, const Json *array) {
    size_t i;
    for (i = 0; i < json_array_size(array); i++) {
        Json *o = json_array_at(array, i);
        if (abono_agregar(db, p,
                          json_as_number(json_get(o, "monto"), 0),
                          json_as_string(json_get(o, "fecha"), ""))) {
            Abono *last = p->abonos;
            while (last && last->next) last = last->next;
            if (last) last->id = json_as_int(json_get(o, "id"), last->id);
        }
    }
}

static void cargar_meses(Prestamo *p, const Json *array) {
    size_t i;
    for (i = 0; i < json_array_size(array); i++) {
        Json *o = json_array_at(array, i);
        int numero = json_as_int(json_get(o, "numero"), 0);
        MesPago *m = mes_buscar(p, numero);
        if (!m) continue;
        m->pagado = json_as_bool(json_get(o, "pagado"), 0);
        free(m->fecha_pago);
        m->fecha_pago = util_strdup(json_as_string(json_get(o, "fecha_pago"), ""));
        m->monto = json_as_number(json_get(o, "monto"), m->monto);
    }
}

static void cargar_prestamos(Database *db, const Json *root) {
    Json *arr = json_get(root, "prestamos");
    size_t i;
    for (i = 0; i < json_array_size(arr); i++) {
        Json *o = json_array_at(arr, i);
        Prestamo *p = prestamo_crear(
            db,
            json_as_int(json_get(o, "cliente_id"), 0),
            json_as_int(json_get(o, "vehiculo_id"), 0),
            json_as_number(json_get(o, "precio"), 0),
            json_as_number(json_get(o, "enganche"), 0),
            json_as_number(json_get(o, "tasa_anual"), 0),
            json_as_int(json_get(o, "plazo_meses"), 0));
        if (!p) continue;
        p->id = json_as_int(json_get(o, "id"), p->id);
        p->cuota = json_as_number(json_get(o, "cuota"), p->cuota);
        cargar_abonos(db, p, json_get(o, "abonos"));
        cargar_meses(p, json_get(o, "meses"));
    }
}

static void recalcular_ids(Database *db) {
    Cliente *c;
    Vehiculo *v;
    Prestamo *p;
    Abono *a;
    int max;

    max = 0;
    for (c = db->clientes; c; c = c->next) if (c->id > max) max = c->id;
    db->next_cliente = max + 1;

    max = 0;
    for (v = db->vehiculos; v; v = v->next) if (v->id > max) max = v->id;
    db->next_vehiculo = max + 1;

    max = 0;
    for (p = db->prestamos; p; p = p->next) {
        if (p->id > max) max = p->id;
        for (a = p->abonos; a; a = a->next) if (a->id > max) max = a->id;
    }
    db->next_prestamo = max + 1;
    db->next_abono = max + 1;
}

int database_cargar(Database *db, const char *path) {
    char *texto;
    Json *root;
    Database temporal;

    if (!db) return 0;
    texto = leer_archivo(path);
    if (!texto) {
        printf("No se encontró el archivo «%s». Se iniciará vacío.\n", path);
        return 0;
    }

    root = json_parse(texto);
    free(texto);
    if (!root) {
        printf("Error al leer JSON: %s\n", json_error());
        return 0;
    }

    database_init(&temporal);
    cargar_clientes(&temporal, root);
    cargar_vehiculos(&temporal, root);
    cargar_prestamos(&temporal, root);
    recalcular_ids(&temporal);
    json_free(root);

    database_free(db);
    *db = temporal;
    printf("Datos cargados desde «%s».\n", path);
    return 1;
}
