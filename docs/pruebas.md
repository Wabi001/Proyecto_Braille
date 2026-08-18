# Pruebas del Proyecto - Impresora Braille

## V1.0 - Pruebas de Hardware

### Microcontrolador

ESP32 WROOM de 38 pines.

### Componentes probados

- 2 motores NEMA 17.
- 2 drivers A4988.
- 1 solenoide de 12 V.
- Etapa de potencia mediante MOSFET.
- Fuente de 12 V.
- GND común entre ESP32 y etapa de potencia.

---

## Prueba 1 - Motor Eje X

### Pines utilizados

- STEP: GPIO 4
- DIR: GPIO 16
- EN: GPIO 17

### Resultado

Motor probado correctamente.

El motor:

- Gira en dirección 1.
- Gira en dirección 2.
- Responde correctamente a las señales STEP y DIR.
- El pin EN del A4988 funciona correctamente.

### Estado

APROBADO.

---

## Prueba 2 - Motor Eje Y

### Pines utilizados

- STEP: GPIO 5
- DIR: GPIO 18
- EN: GPIO 19

### Resultado

Motor probado correctamente.

El motor:

- Gira en dirección 1.
- Gira en dirección 2.
- Responde correctamente a las señales STEP y DIR.
- El pin EN del A4988 funciona correctamente.

### Estado

APROBADO.

---

## Prueba 3 - Solenoide

### Pin utilizado

- Control MOSFET: GPIO 21

### Resultado

El solenoide funciona correctamente mediante la etapa de potencia.

Se realizaron pulsos cortos para comprobar su accionamiento.

### Estado

APROBADO.

---

## Prueba 4 - Sistema conjunto

Se realizó una prueba utilizando simultáneamente:

- Motor eje X.
- Motor eje Y.
- Solenoide.

La secuencia utilizada fue:

1. Movimiento del eje X.
2. Activación del solenoide.
3. Movimiento del eje Y.
4. Activación del solenoide.
5. Retorno del eje X.
6. Retorno del eje Y.

### Resultado

Todos los componentes funcionan correctamente dentro del mismo programa.

No se detectaron fallos de funcionamiento durante la prueba.

### Estado

APROBADO.

---

## Conclusión V1

La etapa básica de hardware se encuentra funcionando correctamente.

Se verificó:

- Control del eje X.
- Control del eje Y.
- Control del solenoide.
- Funcionamiento conjunto de los tres sistemas.

La siguiente etapa será implementar la comunicación Serial entre la computadora y el ESP32.