# Protocolo de Comunicación Serial

## Proyecto

Impresora Braille con ESP32 WROOM.

## Comunicación

- Interfaz: USB Serial
- Velocidad: 115200 baudios
- Terminación recomendada: New Line

---

## Comando PING

Comprueba si el ESP32 responde.

### Enviar

```text
PING

Respuesta 

PONG

Comando STATUS

Consulta el estado actual del firmware.

Enviar
STATUS

========== STATUS ==========
STATE: READY
X: ENABLED
Y: ENABLED
SOLENOID: READY
SERIAL: OK
============================

Comando MOVE

Permite mover los ejes X o Y.

Formato
MOVE EJE PASOS VELOCIDAD

Ejemplos
MOVE X 500 300
MOVE X -500 300
MOVE Y 500 300
MOVE Y -500 300

Comando PUNCH

Acciona el solenoide una vez.

Enviar
PUNCH

Respuesta
PUNCH: Activando solenoide
OK: PUNCH completado

Comando STOP

Detiene un movimiento activo o una acción del solenoide.

Enviar
STOP
Respuesta durante movimiento
STOP: Movimiento/accion interrumpida
OK: STOP ejecutado

Después de un STOP:

STATE: STOPPED

El sistema puede recibir posteriormente un nuevo comando MOVE.

Comando HELP

Muestra los comandos disponibles.

Enviar
HELP
Validaciones

El firmware detecta:

Eje inválido.
Pasos iguales a cero.
Velocidad fuera del rango permitido.
Comandos desconocidos.
Sistema ocupado.
Comandos demasiado largos.


Guarda con:


```text
Ctrl + S
2. docs\pruebas.md

No borres lo de V1.

Ve al final del archivo y agrega:

---


# V2 - Pruebas de Comunicación Serial


## Prueba PING


Comando:


```text
PING

Resultado:

PONG

Estado: APROBADO.

Prueba STATUS

El ESP32 informa correctamente estados como:

READY
STOPPED

Además informa:

Estado del eje X.
Estado del eje Y.
Estado del solenoide.
Estado de comunicación Serial.

Estado: APROBADO.

Prueba MOVE eje X

Se probaron movimientos positivos y negativos.

Ejemplos:

MOVE X 200 200
MOVE X -200 200
MOVE X 500 300
MOVE X -500 300
MOVE X 1000 500
MOVE X -1000 500

Resultado:

El motor X responde correctamente.

Estado: APROBADO.

Prueba MOVE eje Y

Se probaron movimientos positivos y negativos.

Ejemplos:

MOVE Y 200 200
MOVE Y -200 200
MOVE Y 500 300
MOVE Y -500 300

Resultado:

El motor Y responde correctamente.

Estado: APROBADO.

Prueba PUNCH

Comando:

PUNCH

Resultado:

El solenoide se activa correctamente mediante GPIO 21 y MOSFET.

Estado: APROBADO.

Prueba STOP eje X

Se inició un movimiento:

MOVE X 200 50

Durante el movimiento se envió:

STOP

Resultado:

El movimiento fue interrumpido correctamente.

Después del STOP:

STATE: STOPPED

Estado: APROBADO.

Prueba STOP eje Y

Se inició un movimiento:

MOVE Y 200 50

Durante el movimiento se envió:

STOP

Resultado:

El movimiento del eje Y fue interrumpido correctamente.

Estado: APROBADO.

Recuperación después de STOP

Después de ejecutar STOP se envió nuevamente un comando MOVE.

El sistema volvió a funcionar correctamente y STATUS regresó a:

STATE: READY

Estado: APROBADO.

Conclusión V2

La comunicación Serial funciona correctamente.

Se validaron:

PING.
STATUS.
HELP.
MOVE X.
MOVE Y.
PUNCH.
STOP.
Movimiento positivo y negativo.
Velocidad configurable.
Detección de errores.
Interrupción de movimientos.
Recuperación después de STOP.

La etapa V2 de comunicación Serial queda aprobada.



### 3. `docs\versiones.md`


Agrega al final:


```markdown
## v2.0.0 - Comunicación Serial funcional


Se implementó el protocolo de comunicación Serial entre PC y ESP32.


### Funciones implementadas


- PING / PONG.
- STATUS.
- HELP.
- MOVE X.
- MOVE Y.
- PUNCH.
- STOP.


### Mejoras


- Movimiento no bloqueante.
- Control de velocidad.
- Dirección mediante pasos positivos o negativos.
- Validación de comandos.
- Detección de errores.
- Estado READY.
- Estado MOVING.
- Estado PUNCHING.
- Estado STOPPED.


### Resultado


Comunicación Serial validada correctamente.


## Comunicación WiFi/TCP

El firmware también permite controlar la impresora mediante WiFi.

### Configuración

- Modo WiFi: Station.
- Protocolo: TCP.
- Puerto: 3333.

La dirección IP es asignada por el router y puede cambiar.

Durante las pruebas iniciales se utilizó:

```text
192.168.0.29


----------------

Protocolo

Los mismos comandos utilizados por USB Serial pueden enviarse mediante TCP:

PING
STATUS
HELP
MOVE X pasos velocidad
MOVE Y pasos velocidad
PUNCH
STOP

Esto permite utilizar un único protocolo independientemente del medio de comunicación.



Importante: esa IP `192.168.0.29` la documentamos como **IP observada durante la prueba**, no como dirección fija definitiva.


## PARTE 16.15 — GitHub


Ahora abre Git Bash o CMD dentro de:


```text
C:\Users\F4b1a\OneDrive\Documents\documentacion_2026_Braille\Proyect