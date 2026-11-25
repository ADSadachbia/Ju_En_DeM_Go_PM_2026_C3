#ifndef MODEL_H
#define MODEL_H

#include <stddef.h>

/* ------------------------------------------------------------------
 * Modelo de datos del sistema de préstamos vehiculares.
 * Todas las colecciones son listas enlazadas simples con memoria
 * dinámica (malloc / free).
 * ------------------------------------------------------------------ */

typedef struct Cliente {
    int id;
    char *nombre;
    char *cedula;
    char *telefono;
    struct Cliente *next;
} Cliente;

typedef struct Vehiculo {
    int id;
    char *marca;
    char *modelo;
    int anio;
    struct Vehiculo *next;
} Vehiculo;

typedef struct Abono {
    int id;
    double monto;
    char *fecha;
    struct Abono *next;
} Abono;

typedef struct MesPago {
    int numero;          /* 1 .. plazo_meses */
    double monto;        /* cuota programada */
    int pagado;          /* 0 = pendiente, 1 = pagado */
    char *fecha_pago;    /* "" si está pendiente */
    struct MesPago *next;
} MesPago;

typedef struct Prestamo {
    int id;
    int cliente_id;
    int vehiculo_id;
    double precio;         /* precio de contado del vehículo */
    double enganche;       /* abono inicial */
    double tasa_anual;     /* porcentaje: 18 => 18% anual */
    int plazo_meses;
    double cuota;          /* cuota mensual calculada */
    Abono *abonos;
    MesPago *meses;
    struct Prestamo *next;
} Prestamo;

typedef struct Database {
    Cliente *clientes;
    Vehiculo *vehiculos;
    Prestamo *prestamos;
    int next_cliente;
    int next_vehiculo;
    int next_prestamo;
    int next_abono;
} Database;

/* -------------------------- Ciclo de vida ------------------------- */
void database_init(Database *db);
void database_free(Database *db);

/* --------------------------- Clientes ----------------------------- */
Cliente *cliente_agregar(Database *db, const char *nombre,
                         const char *cedula, const char *telefono);
Cliente *cliente_buscar(const Database *db, const char *texto);
int      cliente_borrar(Database *db, int id);
void     cliente_listar(const Database *db);

/* -------------------------- Vehículos ----------------------------- */
Vehiculo *vehiculo_agregar(Database *db, const char *marca,
                           const char *modelo, int anio);
Vehiculo *vehiculo_buscar(const Database *db, const char *texto);
int       vehiculo_borrar(Database *db, int id);
void      vehiculo_listar(const Database *db);

/* -------------------------- Préstamos ----------------------------- */
Prestamo *prestamo_crear(Database *db, int cliente_id, int vehiculo_id,
                         double precio, double enganche,
                         double tasa_anual, int plazo_meses);
Prestamo *prestamo_buscar(const Database *db, int id);
int       prestamo_borrar(Database *db, int id);
void      prestamo_listar(const Database *db);

/* ---------------------------- Abonos ------------------------------ */
int abono_agregar(Database *db, Prestamo *prestamo, double monto, const char *fecha);
void abono_listar(const Prestamo *prestamo);

/* -------------------------- Pagos mensuales ----------------------- */
MesPago *mes_buscar(Prestamo *prestamo, int numero);
int      mes_marcar(Prestamo *prestamo, int numero, int pagado, const char *fecha);
void     meses_listar(const Prestamo *prestamo);

/* ------------------------- Cálculos y reportes -------------------- */
double calcular_cuota(double capital, double tasa_anual, int plazo_meses);
double prestamo_total_pagar(const Prestamo *prestamo);
double prestamo_pagado(const Prestamo *prestamo);
double prestamo_saldo(const Prestamo *prestamo);
void   prestamo_reporte(const Database *db, const Prestamo *prestamo);

/* ------------------------- Persistencia JSON ---------------------- */
int database_guardar(const Database *db, const char *path);
int database_cargar(Database *db, const char *path);

/* --------------------------- Utilidades --------------------------- */
char *util_strdup(const char *s);
char *util_fecha_hoy(void);
int   util_leer_entero(const char *prompt, int min, int max);
double util_leer_double(const char *prompt, double min, double max);
void  util_leer_texto(const char *prompt, char *buffer, size_t size);
int   util_confirmar(const char *prompt);

#endif
