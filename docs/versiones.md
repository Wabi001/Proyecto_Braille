## v3.0.0 - Comunicación WiFi/TCP

Se implementó comunicación inalámbrica utilizando WiFi.

### Funciones implementadas

- Conexión del ESP32 a una red WiFi.
- Obtención automática de dirección IP.
- Servidor TCP.
- Puerto TCP 3333.
- Cliente remoto desde computadora.
- Comunicación bidireccional.
- Integración con comunicación Serial.

### Comandos disponibles por WiFi

- PING.
- STATUS.
- HELP.
- MOVE X.
- MOVE Y.
- PUNCH.
- STOP.

### Arquitectura

El mismo procesador de comandos puede recibir instrucciones desde:

- USB Serial.
- WiFi TCP.

### Resultado

La comunicación WiFi/TCP fue validada correctamente.