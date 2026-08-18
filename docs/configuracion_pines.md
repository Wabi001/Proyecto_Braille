# Configuración de Pines - Impresora Braille

## Microcontrolador

ESP32 WROOM de 38 pines.

## Eje X

| Función | GPIO ESP32 |
|---|---:|
| STEP | GPIO 4 |
| DIR | GPIO 16 |
| EN | GPIO 17 |

## Eje Y

| Función | GPIO ESP32 |
|---|---:|
| STEP | GPIO 5 |
| DIR | GPIO 18 |
| EN | GPIO 19 |

## Solenoide

| Función | GPIO ESP32 |
|---|---:|
| Control del MOSFET | GPIO 21 |

## Comunicación

- USB Serial: 115200 baudios
- WiFi: integrado en ESP32 WROOM

## Drivers

- Driver eje X: A4988
- Driver eje Y: A4988
- Microstepping previsto: 1/16

## Motores

- 2 motores NEMA 17
- Un motor para el eje X
- Un motor para el eje Y

## Alimentación

- Motores: 12 V
- Solenoide: 12 V
- GND común entre ESP32, drivers y etapa de potencia del solenoide

## Resumen de conexiones

ESP32 WROOM:

- GPIO 4  → STEP X
- GPIO 16 → DIR X
- GPIO 17 → EN X
- GPIO 5  → STEP Y
- GPIO 18 → DIR Y
- GPIO 19 → EN Y
- GPIO 21 → Solenoide