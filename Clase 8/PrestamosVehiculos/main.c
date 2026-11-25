#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_DATOS "datos.json"

/* ------------------------------------------------------------------ */
/* Menus                                                               */
/* ------------------------------------------------------------------ */

static void pausa(void) {
    printf("\nPresione Enter para continuar...");
    fflush(stdout);
    getchar();
}

static void titulo(const char *texto) {
    printf("\n============================================================\n");
    printf("  %s\n", texto);
    printf("============================================================\n");
}

static void menu_vehiculos(Database *db) {
    int opcion;
    char marca[64], modelo[64], texto[128];
    int anio;

    for (;;) {
        titulo("GESTIÓN DE VEHÍCULOS (marca / modelo / año)");
        printf("  1) Agregar vehículo\n");
        printf("  2) Listar vehículos\n");
        printf("  3) Buscar vehículo\n");
        printf("  4) Borrar vehículo\n");
        printf("  0) Volver\n\n");
        opcion = util_leer_entero("  Opción: ", 0, 4);

        switch (opcion) {
            case 1:
                util_leer_texto("  Marca: ", marca, sizeof(marca));
                util_leer_texto("  Modelo: ", modelo, sizeof(modelo));
                anio = util_leer_entero("  Año (1900-2100): ", 1900, 2100);
                if (vehiculo_agregar(db, marca, modelo, anio))
                    printf("  Vehículo agregado correctamente.\n");
                pausa();
                break;

            case 2:
                vehiculo_listar(db);
                pausa();
                break;

            case 3:
                util_leer_texto("  Buscar por marca, modelo o año: ", texto, sizeof(texto));
                vehiculo_buscar(db, texto);
                pausa();
                break;

            case 4:
                vehiculo_listar(db);
                if (db->vehiculos) {
                    anio = util_leer_entero("  ID del vehículo a borrar (0=cancelar): ", 0, 2147483647);
                    if (anio) vehiculo_borrar(db, anio);
                }
                pausa();
                break;

            case 0:
                return;
        }
    }
}

static void menu_clientes(Database *db) {
    int opcion, id;
    char nombre[80], cedula[40], telefono[40], texto[128];

    for (;;) {
        titulo("GESTIÓN DE CLIENTES");
        printf("  1) Agregar cliente\n");
        printf("  2) Listar clientes\n");
        printf("  3) Buscar cliente\n");
        printf("  4) Borrar cliente\n");
        printf("  0) Volver\n\n");
        opcion = util_leer_entero("  Opción: ", 0, 4);

        switch (opcion) {
            case 1:
                util_leer_texto("  Nombre: ", nombre, sizeof(nombre));
                util_leer_texto("  Cédula: ", cedula, sizeof(cedula));
                util_leer_texto("  Teléfono: ", telefono, sizeof(telefono));
                if (cliente_agregar(db, nombre, cedula, telefono))
                    printf("  Cliente agregado correctamente.\n");
                pausa();
                break;

            case 2:
                cliente_listar(db);
                pausa();
                break;

            case 3:
                util_leer_texto("  Nombre o cédula a buscar: ", texto, sizeof(texto));
                cliente_buscar(db, texto);
                pausa();
                break;

            case 4:
                cliente_listar(db);
                if (db->clientes) {
                    id = util_leer_entero("  ID del cliente a borrar (0=cancelar): ", 0, 2147483647);
                    if (id) cliente_borrar(db, id);
                }
                pausa();
                break;

            case 0:
                return;
        }
    }
}

static Prestamo *seleccionar_prestamo(const Database *db) {
    int id;
    prestamo_listar(db);
    if (!db->prestamos) {
        printf("\n  No hay préstamos registrados.\n");
        pausa();
        return NULL;
    }
    id = util_leer_entero("\n  ID del préstamo (0=cancelar): ", 0, 2147483647);
    if (!id) return NULL;
    return prestamo_buscar(db, id);
}

static void crear_prestamo(Database *db) {
    int cliente_id, vehiculo_id, anios, plazo_meses;
    double precio, enganche, tasa, cuota;
    Prestamo *p;

    titulo("NUEVO PRÉSTAMO");
    cliente_listar(db);
    vehiculo_listar(db);

    if (!db->clientes || !db->vehiculos) {
        printf("\n  Debe registrar al menos un cliente y un vehículo primero.\n");
        pausa();
        return;
    }

    cliente_id = util_leer_entero("\n  ID del cliente: ", 1, 2147483647);
    vehiculo_id = util_leer_entero("  ID del vehículo: ", 1, 2147483647);
    precio = util_leer_double("  Precio de contado del vehículo: ", 1, 100000000);
    enganche = util_leer_double("  Enganche / abono inicial: ", 0, precio - 0.01);
    tasa = util_leer_double("  Tasa de interés anual (ej. 18 para 18%): ", 0, 200);
    anios = util_leer_entero("  Plazo en años: ", 1, 30);
    plazo_meses = anios * 12;

    cuota = calcular_cuota(precio - enganche, tasa, plazo_meses);
    printf("\n  Resumen:\n");
    printf("    Monto financiado : %.2f\n", precio - enganche);
    printf("    Plazo            : %d meses (%d años)\n", plazo_meses, anios);
    printf("    Cuota mensual    : %.2f\n", cuota);
    printf("    Total a pagar    : %.2f\n", enganche + cuota * plazo_meses);

    if (!util_confirmar("\n  ¿Crear el préstamo? (s/n): ")) {
        printf("  Operación cancelada.\n");
        pausa();
        return;
    }

    p = prestamo_crear(db, cliente_id, vehiculo_id, precio, enganche, tasa, plazo_meses);
    if (p) {
        printf("  Préstamo #%d creado con %d cuotas de %.2f.\n",
               p->id, p->plazo_meses, p->cuota);
        meses_listar(p);
    }
    pausa();
}

static void agregar_abono(Database *db) {
    Prestamo *p = seleccionar_prestamo(db);
    double monto;
    char fecha[32];
    char *hoy;

    if (!p) return;
    printf("\n  Saldo actual: %.2f\n", prestamo_saldo(p));
    printf("  Total pagado hasta hoy: %.2f\n", prestamo_pagado(p));

    monto = util_leer_double("\n  Monto del abono: ", 0.01, 100000000);
    util_leer_texto("  Fecha (YYYY-MM-DD, Enter = hoy): ", fecha, sizeof(fecha));
    if (!*fecha) {
        hoy = util_fecha_hoy();
        strcpy(fecha, hoy);
        free(hoy);
    }

    if (abono_agregar(db, p, monto, fecha)) {
        printf("  Abono de %.2f registrado.\n", monto);
        printf("  Nuevo saldo: %.2f\n", prestamo_saldo(p));
        abono_listar(p);
    }
    pausa();
}

static void gestionar_pagos(Database *db) {
    Prestamo *p = seleccionar_prestamo(db);
    int opcion, numero, desde, hasta;
    MesPago *m;

    if (!p) return;

    for (;;) {
        titulo("GESTIÓN DE PAGOS MENSUALES");
        printf("  Préstamo #%d | Saldo: %.2f | Cuota: %.2f\n",
               p->id, prestamo_saldo(p), p->cuota);
        printf("  1) Marcar un mes como PAGADO\n");
        printf("  2) Marcar un mes como NO PAGADO\n");
        printf("  3) Marcar un rango de meses como PAGADOS\n");
        printf("  4) Marcar todos los meses como PAGADOS\n");
        printf("  5) Marcar todos los meses como NO PAGADOS\n");
        printf("  6) Ver lista de meses\n");
        printf("  0) Volver\n\n");
        opcion = util_leer_entero("  Opción: ", 0, 6);

        switch (opcion) {
            case 1:
                meses_listar(p);
                numero = util_leer_entero("\n  Número de mes a marcar PAGADO: ", 1, p->plazo_meses);
                if (mes_marcar(p, numero, 1, NULL)) printf("  Mes %d marcado como PAGADO.\n", numero);
                break;

            case 2:
                meses_listar(p);
                numero = util_leer_entero("\n  Número de mes a marcar NO PAGADO: ", 1, p->plazo_meses);
                if (mes_marcar(p, numero, 0, NULL)) printf("  Mes %d marcado como PENDIENTE.\n", numero);
                break;

            case 3:
                meses_listar(p);
                desde = util_leer_entero("\n  Desde el mes: ", 1, p->plazo_meses);
                hasta = util_leer_entero("  Hasta el mes: ", desde, p->plazo_meses);
                for (numero = desde; numero <= hasta; numero++)
                    mes_marcar(p, numero, 1, NULL);
                printf("  Meses %d a %d marcados como PAGADOS.\n", desde, hasta);
                break;

            case 4:
                for (m = p->meses; m; m = m->next)
                    mes_marcar(p, m->numero, 1, NULL);
                printf("  Todos los meses marcados como PAGADOS.\n");
                break;

            case 5:
                for (m = p->meses; m; m = m->next)
                    mes_marcar(p, m->numero, 0, NULL);
                printf("  Todos los meses marcados como PENDIENTES.\n");
                break;

            case 6:
                meses_listar(p);
                break;

            case 0:
                return;
        }

        if (opcion != 6) {
            printf("\n  Saldo actual: %.2f\n", prestamo_saldo(p));
            meses_listar(p);
        }
        pausa();
    }
}

static void ver_reporte(Database *db) {
    Prestamo *p = seleccionar_prestamo(db);
    if (!p) return;
    prestamo_reporte(db, p);
    pausa();
}

static void borrar_prestamo(Database *db) {
    Prestamo *p = seleccionar_prestamo(db);
    char mensaje[128];
    if (!p) return;
    sprintf(mensaje, "\n  ¿Borrar definitivamente el préstamo #%d? (s/n): ", p->id);
    if (util_confirmar(mensaje))
        prestamo_borrar(db, p->id);
    pausa();
}

static void menu_prestamos(Database *db) {
    int opcion;

    for (;;) {
        titulo("GESTIÓN DE PRÉSTAMOS");
        printf("  1) Crear nuevo préstamo\n");
        printf("  2) Listar préstamos y saldos\n");
        printf("  3) Agregar pago de abono\n");
        printf("  4) Gestionar pagos mensuales\n");
        printf("  5) Generar reporte de estado de pago\n");
        printf("  6) Borrar préstamo\n");
        printf("  0) Volver\n\n");
        opcion = util_leer_entero("  Opción: ", 0, 6);

        switch (opcion) {
            case 1: crear_prestamo(db);            break;
            case 2: prestamo_listar(db); pausa();  break;
            case 3: agregar_abono(db);             break;
            case 4: gestionar_pagos(db);           break;
            case 5: ver_reporte(db);               break;
            case 6: borrar_prestamo(db);           break;
            case 0: return;
        }
    }
}

int main(void) {
    Database db;
    int opcion;

    printf("============================================================\n");
    printf("   SISTEMA DE PRÉSTAMOS PARA COMPRA DE VEHÍCULOS\n");
    printf("   Listas enlazadas + memoria dinámica + JSON\n");
    printf("============================================================\n\n");

    database_init(&db);
    database_cargar(&db, ARCHIVO_DATOS);

    for (;;) {
        titulo("MENÚ PRINCIPAL");
        printf("  1) Gestión de vehículos (marca, modelo, año)\n");
        printf("  2) Gestión de clientes (agregar, buscar, borrar)\n");
        printf("  3) Gestión de préstamos y pagos\n");
        printf("  4) Guardar datos en %s\n", ARCHIVO_DATOS);
        printf("  5) Cargar datos desde %s\n", ARCHIVO_DATOS);
        printf("  0) Salir\n\n");
        opcion = util_leer_entero("  Opción: ", 0, 5);

        switch (opcion) {
            case 1:
                menu_vehiculos(&db);
                break;
            case 2:
                menu_clientes(&db);
                break;
            case 3:
                menu_prestamos(&db);
                break;
            case 4:
                if (database_guardar(&db, ARCHIVO_DATOS))
                    printf("\n  Datos guardados correctamente.\n");
                else
                    printf("\n  Error: no se pudo guardar el archivo.\n");
                pausa();
                break;
            case 5:
                database_cargar(&db, ARCHIVO_DATOS);
                pausa();
                break;
            case 0:
                if (util_confirmar("\n  ¿Guardar antes de salir? (s/n): "))
                    database_guardar(&db, ARCHIVO_DATOS);
                printf("  Hasta luego.\n");
                database_free(&db);
                return EXIT_SUCCESS;
        }
    }
}
