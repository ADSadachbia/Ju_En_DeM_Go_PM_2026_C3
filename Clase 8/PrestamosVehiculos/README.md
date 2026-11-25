# Sistema de Préstamos para Compra de Vehículos

Programa de consola en **C** para gestionar la venta de vehículos a crédito.

## Requisitos cubiertos

| Requisito | Dónde está |
|---|---|
| Listas enlazadas | `Cliente`, `Vehiculo`, `Prestamo`, `Abono` y `MesPago` son nodos con puntero `next` (`model.h`) |
| Memoria dinámica | `malloc`, `calloc`, `realloc` y `free` en `negocio.c` y `json.c` |
| Archivos JSON | Parser/escritor propio en `json.c` / `json.h`; persistencia en `negocio.c` |
| Marca, modelo y año | Menú *Gestión de vehículos* |
| Clientes: agregar, buscar y borrar | Menú *Gestión de clientes* |
| Pagos de abono | Menú *Préstamos → Agregar pago de abono* |
| Pagos mensuales | Menú *Préstamos → Gestionar pagos mensuales* |
| Reporte de estado de pago | Menú *Préstamos → Generar reporte de estado de pago* |
| Lista de meses, pagados y no pagados | Cada préstamo genera `plazo_meses` nodos `MesPago` con estado |
| Visualizar el saldo | Listado de préstamos y reporte (se recalcula al agregar abonos o marcar pagos) |

## Compilación

```bash
make
```

Requiere `gcc` (o compatible) con soporte C11. Se enlaza con `-lm` por el uso de `pow()`.

Para ejecutar:

```bash
./prestamos
# o
make run
```

Para borrar los compilados:

```bash
make clean
```

## Estructura de archivos

| Archivo | Contenido |
|---|---|
| `main.c` | Menús y flujo de interacción con el usuario |
| `model.h` | Definición de las estructuras (nodos) y prototipos |
| `negocio.c` | Reglas de negocio: altas, búsquedas, bajas, cálculos, reportes y persistencia |
| `json.c` / `json.h` | Parser y serializador JSON mínimo, sin librerías externas |
| `Makefile` | Compilación |
| `datos.ejemplo.json` | Ejemplo del archivo generado por el programa |

Al iniciar, el programa intenta cargar `datos.json`. Si no existe, empieza vacío.
La opción **4) Guardar datos** crea/actualiza `datos.json`. Para probar con datos listos:

```bash
cp datos.ejemplo.json datos.json
./prestamos
```

## Cómo se calcula la cuota

Se usa el **sistema francés** (cuota constante):

```
capital   = precio - enganche
i         = (tasa_anual / 100) / 12
cuota     = capital * i / (1 - (1 + i)^(-meses))
```

Si la tasa es 0, la cuota es simplemente `capital / meses`.

Ejemplo: precio 5000, enganche 1000, tasa 18% anual, 3 años (36 meses):

```
capital = 4000
i       = 0.015
cuota   = 4000 * 0.015 / (1 - 1.015^-36) = 144.61
```

### Saldo

```
total a pagar = enganche + cuota * meses
pagado        = enganche + cuotas marcadas como pagadas + abonos
saldo         = total a pagar - pagado
```

Si el resultado es negativo, el reporte lo muestra como **saldo a favor**
(el cliente pagó más de lo comprometido).

> Los abonos se registran y descuentan del saldo, pero **no recalculan la cuota**
> ni reprograman los meses. Es la forma más simple y transparente de llevar el control.

## Menú principal

```
1) Gestión de vehículos (marca, modelo, año)
2) Gestión de clientes (agregar, buscar, borrar)
3) Gestión de préstamos y pagos
4) Guardar datos en datos.json
5) Cargar datos desde datos.json
0) Salir
```

Dentro de *Gestión de préstamos*:

```
1) Crear nuevo préstamo
2) Listar préstamos y saldos
3) Agregar pago de abono
4) Gestionar pagos mensuales
5) Generar reporte de estado de pago
6) Borrar préstamo
0) Volver
```

En *Gestionar pagos mensuales* se puede marcar un mes individual, un rango,
todos los meses como pagados o todos como pendientes. Cada marcado guarda la
fecha en que se realizó.

## Formato JSON

```json
{
  "meta": { "next_cliente": 2, "next_vehiculo": 2, "next_prestamo": 2, "next_abono": 2 },
  "clientes": [
    { "id": 1, "nombre": "Juan", "cedula": "123", "telefono": "555" }
  ],
  "vehiculos": [
    { "id": 1, "marca": "Toyota", "modelo": "Corolla", "anio": 2020 }
  ],
  "prestamos": [
    {
      "id": 1,
      "cliente_id": 1,
      "vehiculo_id": 1,
      "precio": 5000,
      "enganche": 1000,
      "tasa_anual": 18,
      "plazo_meses": 36,
      "cuota": 144.6095821,
      "abonos": [ { "id": 1, "monto": 200, "fecha": "2026-11-19" } ],
      "meses": [
        { "numero": 1, "monto": 144.6095821, "pagado": true, "fecha_pago": "2026-11-19" },
        { "numero": 2, "monto": 144.6095821, "pagado": false, "fecha_pago": "" }
      ]
    }
  ]
}
```

Los IDs se recalculan al cargar para evitar duplicados si el archivo fue editado a mano.

## Notas de implementación

- No se usan librerías externas: el parser JSON acepta cadenas, números,
  booleanos, nulos, arreglos y objetos, incluyendo escapes `\uXXXX`.
- Dar de baja un cliente o vehículo se bloquea si tiene un préstamo asociado,
  para evitar referencias colgantes.
- Toda la memoria se libera al salir (`database_free`).
- Compilado y probado con `gcc -std=c11 -Wall -Wextra -Wpedantic` sin advertencias.
