# Calculadora de costo de viaje (C)

Programa en C para administrar vehículos y calcular el costo de un viaje y el
costo por kilómetro. Tiene **dos interfaces**: consola (`viaje.c`) y ventana
GTK 3 (`viaje_gui.c`). Ambas comparten el mismo archivo de datos.

## Compilar y ejecutar

```bash
make          # compila viaje (consola) y viaje_gui (ventana)
make run      # compila y ejecuta la versión de consola
make gui      # compila y ejecuta la versión gráfica (requiere GTK 3)
make clean    # borra los binarios y reporte_viaje.txt
```

Los vehículos se guardan en `vehiculos.dat` (en el directorio actual) y no se
borran con `make clean`.

## Interfaz gráfica (GTK 3)

`make gui` abre una ventana con dos pestañas:

- **Vehículos**: combo para elegir vehículo, botones *Nuevo*, *Guardar* y
  *Borrar* (con confirmación), y un formulario con los 12 parámetros.
- **Calcular viaje**: elige vehículo, distancia, % carretera, % ciudad y
  precio del combustible. Muestra el desglose en un área de texto y permite
  exportarlo con *Guardar reporte* a `reporte_viaje.txt`.

Requisito: `gtk+-3.0` (verificado con `pkg-config --modversion gtk+-3.0`).

## Funciones

1. Crear vehículo
2. Listar vehículos
3. Editar vehículo (campo por campo o todos)
4. Borrar vehículo (con confirmación)
5. Calcular costo de viaje

## Parámetros del vehículo

| Parámetro | Unidad |
|---|---|
| Consumo en ciudad | km/l |
| Consumo en carretera | km/l |
| Seguro | $/año |
| Neumáticos | $/km |
| Mantenimiento | $/km |
| Valor inicial / valor final | $ |
| Tiempo de uso | años |
| Impuesto | $/año |
| Limpieza | $/año |
| Km por año | km/año |

## Fórmulas

Entradas del cálculo: distancia total, % carretera, % ciudad y precio del
combustible. Si los porcentajes no suman 100 se normalizan.

- `km_carretera = km_total * %carretera / (%carretera + %ciudad)`
- `km_ciudad = km_total - km_carretera`
- `litros = km_ciudad / consumo_ciudad + km_carretera / consumo_carretera`
- `combustible = litros * precio_litro`
- `neumáticos = $/km * km_total`
- `mantenimiento = $/km * km_total`
- `seguro = seguro_año / km_año * km_total`
- `impuesto = impuesto_año / km_año * km_total`
- `limpieza = limpieza_año / km_año * km_total`
- `depreciación = (valor_inicial - valor_final) / años / km_año * km_total`

El resultado muestra cada concepto con su costo total, su costo por km y su
porcentaje del total, más el **costo total del viaje** y el **costo por km**.
