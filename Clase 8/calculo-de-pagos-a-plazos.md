# Cómo calcular el pago mensual de un préstamo o venta a plazos

Este documento explica **dos conceptos distintos** que suelen confundirse. La diferencia clave está en si el precio que tienes ya incluye los intereses o no.

| Concepto | ¿Qué es? | Cuándo usarlo |
|---|---|---|
| **1. Precio total cerrado (sin intereses)** | El cliente debe exactamente ese monto; solo se divide en partes | Quando dices: "me debe $3,600 en 3 años" |
| **2. Precio de contado + interés (amortización)** | Al precio base se le agrega un costo financiero | Cuando dices: "cuesta $5,000, pero lo pago en 3 años con 18% anual" |

---

## Concepto 1 — Precio total cerrado, sin intereses

Es el caso más simple. No hay tasa ni cálculo financiero: **solo divides el total entre el número de meses**.

### Fórmula

$$
\text{Cuota mensual} = \frac{\text{Precio total}}{\text{Años} \times 12}
$$

### Ejemplo

Un cliente te debe **$3,600** y quieres que lo pague en **3 años**.

- Meses: `3 × 12 = 36`
- Cuota: `$3,600 ÷ 36 = $100 al mes`

| Dato | Valor |
|---|---:|
| Precio total | $3,600 |
| Plazo | 3 años (36 meses) |
| Pago mensual | **$100** |
| Total pagado | $3,600 |
| Intereses | $0 |

### En Excel / Google Sheets

```excel
=3600/(3*12)
```

> ⚠️ Ojo: si el cliente deja de pagar, no hay "interés de mora" incluido en esta cuota. Ese monto tendrías que acordarlo aparte.

---

## Concepto 2 — Precio de contado con interés (sistema francés)

Es el sistema estándar de hipotecas, autos y créditos. La **cuota es constante**, pero cada pago se divide en dos partes:

1. **Intereses:** se calculan sobre el saldo que aún se debe.
2. **Capital (amortización):** el resto del pago, que reduce la deuda.

Al principio se paga casi todo interés; al final casi todo capital.

### Datos necesarios

| Variable | Significado | Ejemplo |
|---|---|---|
| `P` | Capital o precio de contado | $5,000 |
| `i` | Tasa mensual = tasa anual ÷ 12 | 18% anual → 1.5% mensual (`0.015`) |
| `n` | Número de pagos = años × 12 | 3 años → 36 meses |

### Fórmula de la cuota

$$
M = P \cdot \frac{i}{1 - (1+i)^{-n}}
$$

Donde **M** es la cuota mensual fija.

### Ejemplo

$$
M = 5000 \cdot \frac{0.015}{1 - (1.015)^{-36}} \approx \$180.70
$$

| Dato | Valor |
|---|---:|
| Precio de contado | $5,000 |
| Tasa anual | 18% |
| Tasa mensual | 1.5% |
| Plazo | 3 años (36 meses) |
| **Cuota mensual** | **$180.70** |
| Total pagado | ≈ $6,505.20 |
| Intereses totales | ≈ $1,505.20 |

### Tabla de amortización (primeras cuotas)

| Mes | Pago | Intereses (1.5%) | Capital | Saldo restante |
|---:|---:|---:|---:|---:|
| 1 | 180.70 | 75.00 | 105.70 | 4,894.30 |
| 2 | 180.70 | 73.41 | 107.29 | 4,787.01 |
| 3 | 180.70 | 71.81 | 108.89 | 4,678.12 |
| ... | ... | ... | ... | ... |
| 36 | 180.70 | 2.67 | 178.03 | 0.00 |

*(los centavos pueden variar ligeramente por redondeo)*

### En Excel / Google Sheets

```excel
=PAGO(tasa_anual/12; años*12; -precio)
```

Ejemplo real:

```excel
=PAGO(18%/12; 3*12; -5000)     → 180.70
```

Funciones relacionadas (en español / inglés):

| Español | Inglés | Para qué sirve |
|---|---|---|
| `PAGO` | `PMT` | Cuota mensual fija |
| `PAGOINT` | `IPMT` | Intereses de un mes específico |
| `PAGOCAPITAL` | `PPMT` | Capital de un mes específico |
| `VNA` | `PV` | Monto que puedes financiar dado un pago |

> En Excel en inglés, los argumentos van con coma: `=PMT(18%/12, 36, -5000)`.

---

## Cómo elegir el plazo (el "máximo de años")

Con el mismo precio y la misma tasa, **más años = cuota más baja, pero más intereses**. Ejemplo con $5,000 al 18% anual:

| Plazo | Meses | Cuota mensual | Total pagado | Intereses |
|---|---:|---:|---:|---:|
| 1 año | 12 | $458.39 | $5,500.68 | $500.68 |
| 2 años | 24 | $249.62 | $5,990.88 | $990.88 |
| **3 años (máximo)** | 36 | **$180.70** | **$6,505.20** | **$1,505.20** |
| 5 años | 60 | $126.96 | $7,617.60 | $2,617.60 |

El "máximo de años" no cambia la fórmula: solo limita el valor que puedes darle a `n`.

---

## Caso inverso: ya sabes cuánto puede pagar al mes

Si el cliente te dice "puedo pagar $200 al mes", puedes despejar cuántos meses necesita:

$$
n = \frac{-\ln\left(1 - \dfrac{P \cdot i}{M}\right)}{\ln(1+i)}
$$

**Ejemplo:** $5,000 al 1.5% mensual, cuota de $200:

$$
n = \frac{-\ln(1 - 75/200)}{\ln(1.015)} \approx 31.6 \rightarrow \textbf{32 meses (≈2.7 años)}
$$

Como 32 ≤ 36 meses, cabe dentro del máximo de 3 años. Si el resultado diera más de 36, habría que **subir la cuota, bajar el precio o reducir la tasa**.

En Excel:

```excel
=-LOG(1-(P*i)/M)/LOG(1+i)
```

Ejemplo: `=-LOG(1-(5000*1.5%)/200)/LOG(1+1.5%)` → `31.57`

> Condición: la cuota `M` debe ser mayor que `P × i` (los intereses del primer mes). Si no, la deuda nunca se termina de pagar.

---

## Otros sistemas de amortización (referencia)

| Sistema | Característica | Uso típico |
|---|---|---|
| **Francés** | Cuota fija; intereses bajan, capital sube | Hipotecas, créditos personales |
| **Alemán** | Capital fijo; cuota total decreciente | Créditos a corto plazo |
| **Americano** | Solo intereses durante el plazo; capital al final | Bonos, pagos "balloon" |
| **Interés simple** | `Intereses = P · i · t` | Préstamos muy cortos |

---

## Resumen rápido

| Situación | Cómo calcular |
|---|---|
| Me deben un total fijo, sin interés | `precio ÷ (años × 12)` |
| Precio de contado + tasa de interés | `=PAGO(tasa/12; años*12; -precio)` |
| Quiero saber los meses dado un pago fijo | `n = -LN(1-(P*i)/M) / LN(1+i)` |
| Quiero bajar la cuota | Extender el plazo hasta el máximo de años acordado |

### Checklist antes de cobrar

- [ ] ¿El precio que tengo **ya incluye** intereses o es de contado?
- [ ] ¿La tasa es **anual**? Divídela entre 12.
- [ ] ¿El plazo está en **años**? Multiplícalo por 12.
- [ ] ¿La cuota cubre al menos los intereses del primer mes (`P × i`)?
- [ ] ¿Acordamos qué pasa con pagos atrasados?
