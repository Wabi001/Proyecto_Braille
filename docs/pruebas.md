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


-------------------------------------------------

---

# V3 - Pruebas de Comunicación WiFi

## Conexión WiFi

El ESP32 se conectó correctamente a la red WiFi configurada.

Durante las pruebas obtuvo la dirección:

```text
192.168.0.29

------------------------------

Servidor TCP

Se implementó un servidor TCP en el ESP32.

Puerto utilizado:

3333

La computadora logró conectarse correctamente al ESP32.

Estado: APROBADO.

-------------------------------

Prueba PING por WiFi

Comando:

PING

Respuesta:

PONG

Estado: APROBADO.

-------------------------------

Prueba STATUS por WiFi

El comando STATUS devuelve correctamente información del sistema.

Ejemplo:

========== STATUS ==========
STATE: READY
X: ENABLED
Y: ENABLED
SOLENOID: READY
WIFI: CONNECTED
TCP: CONNECTED
SERIAL: OK
============================

Estado: APROBADO.

--------------------------------------

Prueba MOVE X por WiFi

Se probaron movimientos positivos y negativos.

Ejemplos:

MOVE X 100 100
MOVE X -100 100

Resultado:

El eje X responde correctamente mediante comandos TCP.

Estado: APROBADO.

----------------------------------------
Prueba MOVE Y por WiFi

Se probaron movimientos positivos y negativos.

Ejemplos:

MOVE Y 100 100
MOVE Y -100 100

Resultado:

El eje Y responde correctamente mediante comandos TCP.

Estado: APROBADO.

-------------------------------------------
Prueba PUNCH por WiFi

Comando:

PUNCH

Resultado:

El solenoide se activa correctamente mediante comunicación WiFi/TCP.

Estado: APROBADO.
-------------------------------------------

Prueba STOP por WiFi

Se inició un movimiento:

MOVE X 300 50

Durante el movimiento se envió:

STOP

Resultado:

El movimiento fue interrumpido correctamente.

El sistema pasó a:

STATE: STOPPED

Posteriormente se realizó un nuevo movimiento y el sistema regresó a:

STATE: READY

Estado: APROBADO.

----------------------------------------------------

Comunicación dual

Se comprobó que el ESP32 puede recibir comandos mediante:

USB Serial.
WiFi TCP.

Ambas interfaces utilizan el mismo procesador de comandos.

Comandos disponibles:

PING.
STATUS.
HELP.
MOVE X.
MOVE Y.
PUNCH.
STOP.

Estado: APROBADO.

-------------------------------------------------------

Conclusión V3

La comunicación WiFi/TCP funciona correctamente.

El ESP32 puede ser controlado tanto por USB Serial como por WiFi utilizando el mismo protocolo de comandos.

La etapa V3 queda aprobada.



Guarda con:


```text
Ctrl + S

----------------------------------------------------------