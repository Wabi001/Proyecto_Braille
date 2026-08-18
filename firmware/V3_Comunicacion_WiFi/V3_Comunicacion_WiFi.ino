// =====================================================
// Proyecto: Impresora Braille
// Versión: V3.2 - Serial + WiFi TCP
//
// Comunicación:
// - USB Serial
// - WiFi TCP puerto 3333
//
// Comandos:
// - PING
// - STATUS
// - HELP
// - MOVE X pasos velocidad
// - MOVE Y pasos velocidad
// - PUNCH
// - STOP
//
// ESP32 WROOM 38 pines
// =====================================================


// =====================================================
// LIBRERÍAS
// =====================================================

#include <WiFi.h>
#include "secrets.h"


// =====================================================
// PINES EJE X
// =====================================================

#define X_STEP 4
#define X_DIR 16
#define X_EN 17


// =====================================================
// PINES EJE Y
// =====================================================

#define Y_STEP 5
#define Y_DIR 18
#define Y_EN 19


// =====================================================
// SOLENOIDE
// =====================================================

#define SOLENOIDE 21


// =====================================================
// CONFIGURACIÓN GENERAL
// =====================================================

const int VELOCIDAD_MIN = 50;
const int VELOCIDAD_MAX = 2000;

const unsigned long TIEMPO_GOLPE = 150;


// =====================================================
// WIFI
// =====================================================

const unsigned long WIFI_TIMEOUT = 20000;

const uint16_t PUERTO_TCP = 3333;

WiFiServer servidor(PUERTO_TCP);

WiFiClient cliente;


// =====================================================
// BUFFERS DE COMUNICACIÓN
// =====================================================

String comandoSerial = "";
String comandoTCP = "";


// =====================================================
// MOVIMIENTO
// =====================================================

bool motorEnMovimiento = false;

char ejeActual = '-';

int pinStepActual = -1;
int pinDirActual = -1;

long pasosRestantes = 0;

unsigned long intervaloPulso = 0;
unsigned long tiempoAnteriorPaso = 0;

bool estadoStep = LOW;


// =====================================================
// SOLENOIDE
// =====================================================

bool solenoideActivo = false;

unsigned long tiempoInicioSolenoide = 0;


// =====================================================
// ESTADO
// =====================================================

bool detenidoPorStop = false;


// =====================================================
// ORIGEN DEL COMANDO
// =====================================================

enum OrigenComando {

  ORIGEN_SERIAL,
  ORIGEN_TCP
};


// =====================================================
// SETUP
// =====================================================

void setup() {

  // ---------------------------------------------------
  // SERIAL
  // ---------------------------------------------------

  Serial.begin(115200);


  // ---------------------------------------------------
  // EJE X
  // ---------------------------------------------------

  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  pinMode(X_EN, OUTPUT);


  // ---------------------------------------------------
  // EJE Y
  // ---------------------------------------------------

  pinMode(Y_STEP, OUTPUT);
  pinMode(Y_DIR, OUTPUT);
  pinMode(Y_EN, OUTPUT);


  // ---------------------------------------------------
  // SOLENOIDE
  // ---------------------------------------------------

  pinMode(SOLENOIDE, OUTPUT);


  // ---------------------------------------------------
  // ESTADOS INICIALES
  // ---------------------------------------------------

  digitalWrite(X_STEP, LOW);
  digitalWrite(Y_STEP, LOW);

  digitalWrite(SOLENOIDE, LOW);


  // LOW = A4988 habilitado

  digitalWrite(X_EN, LOW);
  digitalWrite(Y_EN, LOW);


  delay(500);


  // ---------------------------------------------------
  // MENSAJE INICIAL
  // ---------------------------------------------------

  Serial.println();
  Serial.println("======================================");
  Serial.println("IMPRESORA BRAILLE - ESP32");
  Serial.println("V3.2 - SERIAL + WIFI TCP");
  Serial.println("======================================");


  // ---------------------------------------------------
  // WIFI
  // ---------------------------------------------------

  conectarWiFi();


  if (WiFi.status() == WL_CONNECTED) {

    servidor.begin();

    Serial.println();
    Serial.println("SERVIDOR TCP INICIADO");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("PUERTO: ");
    Serial.println(PUERTO_TCP);

    Serial.println();
  }


  Serial.println("Sistema listo.");
  Serial.println();
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Comunicación USB
  leerSerial();

  // Comunicación WiFi
  gestionarTCP();

  // Movimiento no bloqueante
  actualizarMovimiento();

  // Solenoide no bloqueante
  actualizarSolenoide();

  // Recuperar WiFi si se pierde
  comprobarWiFi();
}


// =====================================================
// CONEXIÓN WIFI
// =====================================================

void conectarWiFi() {

  Serial.println();
  Serial.println("Conectando WiFi...");

  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);


  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  unsigned long inicio =
    millis();


  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - inicio < WIFI_TIMEOUT
  ) {

    delay(500);

    Serial.print(".");
  }


  Serial.println();


  if (WiFi.status() == WL_CONNECTED) {

    Serial.println(
      "WIFI CONECTADO"
    );

    Serial.print(
      "IP: "
    );

    Serial.println(
      WiFi.localIP()
    );
  }

  else {

    Serial.println(
      "ERROR: WiFi no conectado"
    );
  }
}


// =====================================================
// COMPROBAR WIFI
// =====================================================

void comprobarWiFi() {

  static unsigned long ultimaRevision = 0;


  if (
    millis() - ultimaRevision < 5000
  ) {

    return;
  }


  ultimaRevision = millis();


  if (
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "WiFi perdido. Reconectando..."
    );


    WiFi.disconnect();

    conectarWiFi();


    if (
      WiFi.status() == WL_CONNECTED
    ) {

      servidor.begin();
    }
  }
}


// =====================================================
// LECTURA SERIAL USB
// =====================================================

void leerSerial() {

  while (
    Serial.available() > 0
  ) {

    char c =
      Serial.read();


    if (
      c == '\n' ||
      c == '\r'
    ) {

      if (
        comandoSerial.length() > 0
      ) {

        comandoSerial.trim();

        comandoSerial.toUpperCase();


        procesarComando(
          comandoSerial,
          ORIGEN_SERIAL
        );


        comandoSerial = "";
      }
    }

    else {

      comandoSerial += c;


      if (
        comandoSerial.length() > 100
      ) {

        comandoSerial = "";

        responder(
          ORIGEN_SERIAL,
          "ERROR: Comando demasiado largo"
        );
      }
    }
  }
}


// =====================================================
// GESTIÓN TCP
// =====================================================

void gestionarTCP() {

  // ---------------------------------------------------
  // BUSCAR CLIENTE
  // ---------------------------------------------------

  if (
    !cliente ||
    !cliente.connected()
  ) {

    WiFiClient nuevoCliente =
      servidor.available();


    if (nuevoCliente) {

      cliente = nuevoCliente;

      comandoTCP = "";


      Serial.println();
      Serial.println(
        "CLIENTE TCP CONECTADO"
      );


      Serial.print(
        "IP cliente: "
      );

      Serial.println(
        cliente.remoteIP()
      );


      cliente.println(
        "ESP32 BRAILLE READY"
      );

      cliente.println(
        "Comandos: PING STATUS HELP MOVE PUNCH STOP"
      );
    }


    return;
  }


  // ---------------------------------------------------
  // LEER CLIENTE
  // ---------------------------------------------------

  while (
    cliente.available() > 0
  ) {

    char c =
      cliente.read();


    if (
      c == '\n' ||
      c == '\r'
    ) {

      if (
        comandoTCP.length() > 0
      ) {

        comandoTCP.trim();

        comandoTCP.toUpperCase();


        Serial.print(
          "TCP RX: "
        );

        Serial.println(
          comandoTCP
        );


        procesarComando(
          comandoTCP,
          ORIGEN_TCP
        );


        comandoTCP = "";
      }
    }

    else {

      comandoTCP += c;


      if (
        comandoTCP.length() > 100
      ) {

        comandoTCP = "";

        responder(
          ORIGEN_TCP,
          "ERROR: Comando demasiado largo"
        );
      }
    }
  }
}


// =====================================================
// PROCESADOR CENTRAL
// =====================================================

void procesarComando(
  String cmd,
  OrigenComando origen
) {

  // ---------------------------------------------------
  // PING
  // ---------------------------------------------------

  if (
    cmd == "PING"
  ) {

    responder(
      origen,
      "PONG"
    );

    return;
  }


  // ---------------------------------------------------
  // STATUS
  // ---------------------------------------------------

  if (
    cmd == "STATUS"
  ) {

    mostrarEstado(
      origen
    );

    return;
  }


  // ---------------------------------------------------
  // HELP
  // ---------------------------------------------------

  if (
    cmd == "HELP"
  ) {

    mostrarAyuda(
      origen
    );

    return;
  }


  // ---------------------------------------------------
  // STOP
  // ---------------------------------------------------

  if (
    cmd == "STOP"
  ) {

    detenerSistema(
      origen
    );

    return;
  }


  // ---------------------------------------------------
  // PUNCH
  // ---------------------------------------------------

  if (
    cmd == "PUNCH"
  ) {

    iniciarGolpe(
      origen
    );

    return;
  }


  // ---------------------------------------------------
  // MOVE
  // ---------------------------------------------------

  if (
    cmd.startsWith("MOVE ")
  ) {

    procesarMovimiento(
      cmd,
      origen
    );

    return;
  }


  // ---------------------------------------------------
  // ERROR
  // ---------------------------------------------------

  responder(
    origen,
    "ERROR: Comando desconocido -> " + cmd
  );
}


// =====================================================
// PROCESAR MOVE
// =====================================================

void procesarMovimiento(
  String cmd,
  OrigenComando origen
) {

  // ---------------------------------------------------
  // SISTEMA OCUPADO
  // ---------------------------------------------------

  if (
    motorEnMovimiento ||
    solenoideActivo
  ) {

    responder(
      origen,
      "ERROR: Sistema ocupado. Use STOP o espere."
    );

    return;
  }


  // ---------------------------------------------------
  // BUSCAR ESPACIOS
  // ---------------------------------------------------

  int espacio1 =
    cmd.indexOf(' ');

  int espacio2 =
    cmd.indexOf(
      ' ',
      espacio1 + 1
    );

  int espacio3 =
    cmd.indexOf(
      ' ',
      espacio2 + 1
    );


  // ---------------------------------------------------
  // FORMATO
  // ---------------------------------------------------

  if (
    espacio1 == -1 ||
    espacio2 == -1 ||
    espacio3 == -1
  ) {

    responder(
      origen,
      "ERROR: Uso MOVE X pasos velocidad"
    );

    return;
  }


  // ---------------------------------------------------
  // EXTRAER DATOS
  // ---------------------------------------------------

  String eje =
    cmd.substring(
      espacio1 + 1,
      espacio2
    );


  long pasos =
    cmd.substring(
      espacio2 + 1,
      espacio3
    ).toInt();


  int velocidad =
    cmd.substring(
      espacio3 + 1
    ).toInt();


  // ---------------------------------------------------
  // VALIDAR EJE
  // ---------------------------------------------------

  if (
    eje != "X" &&
    eje != "Y"
  ) {

    responder(
      origen,
      "ERROR: Eje invalido. Use X o Y."
    );

    return;
  }


  // ---------------------------------------------------
  // VALIDAR PASOS
  // ---------------------------------------------------

  if (
    pasos == 0
  ) {

    responder(
      origen,
      "ERROR: Los pasos no pueden ser 0"
    );

    return;
  }


  // ---------------------------------------------------
  // VALIDAR VELOCIDAD
  // ---------------------------------------------------

  if (
    velocidad < VELOCIDAD_MIN ||
    velocidad > VELOCIDAD_MAX
  ) {

    responder(
      origen,
      "ERROR: Velocidad permitida: 50 - 2000 pasos/s"
    );

    return;
  }


  // ---------------------------------------------------
  // EJE X
  // ---------------------------------------------------

  if (
    eje == "X"
  ) {

    ejeActual = 'X';

    pinStepActual = X_STEP;

    pinDirActual = X_DIR;
  }


  // ---------------------------------------------------
  // EJE Y
  // ---------------------------------------------------

  else {

    ejeActual = 'Y';

    pinStepActual = Y_STEP;

    pinDirActual = Y_DIR;
  }


  // ---------------------------------------------------
  // DIRECCIÓN
  // ---------------------------------------------------

  if (
    pasos > 0
  ) {

    digitalWrite(
      pinDirActual,
      HIGH
    );

    pasosRestantes =
      pasos;
  }

  else {

    digitalWrite(
      pinDirActual,
      LOW
    );

    pasosRestantes =
      -pasos;
  }


  // ---------------------------------------------------
  // VELOCIDAD
  // ---------------------------------------------------

  intervaloPulso =
    500000UL / velocidad;


  digitalWrite(
    pinStepActual,
    LOW
  );


  estadoStep = LOW;

  tiempoAnteriorPaso =
    micros();


  motorEnMovimiento = true;

  detenidoPorStop = false;


  String mensaje =
    "MOVIMIENTO INICIADO | Eje: ";

  mensaje += ejeActual;

  mensaje += " | Pasos: ";

  mensaje += pasosRestantes;

  mensaje += " | Velocidad: ";

  mensaje += velocidad;

  mensaje += " pasos/s";


  responder(
    origen,
    mensaje
  );
}


// =====================================================
// ACTUALIZAR MOVIMIENTO
// =====================================================

void actualizarMovimiento() {

  if (
    !motorEnMovimiento
  ) {

    return;
  }


  unsigned long ahora =
    micros();


  if (
    ahora - tiempoAnteriorPaso
    < intervaloPulso
  ) {

    return;
  }


  tiempoAnteriorPaso =
    ahora;


  // ---------------------------------------------------
  // SUBIR STEP
  // ---------------------------------------------------

  if (
    estadoStep == LOW
  ) {

    digitalWrite(
      pinStepActual,
      HIGH
    );

    estadoStep =
      HIGH;
  }


  // ---------------------------------------------------
  // BAJAR STEP
  // ---------------------------------------------------

  else {

    digitalWrite(
      pinStepActual,
      LOW
    );

    estadoStep =
      LOW;


    if (
      pasosRestantes > 0
    ) {

      pasosRestantes--;
    }


    // -------------------------------------------------
    // FINAL DEL MOVIMIENTO
    // -------------------------------------------------

    if (
      pasosRestantes == 0
    ) {

      motorEnMovimiento =
        false;


      String mensaje =
        "OK: Movimiento ";

      mensaje += ejeActual;

      mensaje +=
        " completado";


      // Mandarlo por ambos medios
      enviarATodos(
        mensaje
      );


      ejeActual = '-';
    }
  }
}


// =====================================================
// PUNCH
// =====================================================

void iniciarGolpe(
  OrigenComando origen
) {

  if (
    motorEnMovimiento ||
    solenoideActivo
  ) {

    responder(
      origen,
      "ERROR: Sistema ocupado."
    );

    return;
  }


  detenidoPorStop =
    false;


  digitalWrite(
    SOLENOIDE,
    HIGH
  );


  solenoideActivo =
    true;


  tiempoInicioSolenoide =
    millis();


  responder(
    origen,
    "PUNCH: Activando solenoide"
  );
}


// =====================================================
// ACTUALIZAR SOLENOIDE
// =====================================================

void actualizarSolenoide() {

  if (
    !solenoideActivo
  ) {

    return;
  }


  if (
    millis() - tiempoInicioSolenoide
    >= TIEMPO_GOLPE
  ) {

    digitalWrite(
      SOLENOIDE,
      LOW
    );


    solenoideActivo =
      false;


    enviarATodos(
      "OK: PUNCH completado"
    );
  }
}


// =====================================================
// STOP
// =====================================================

void detenerSistema(
  OrigenComando origen
) {

  bool habiaActividad =
    motorEnMovimiento ||
    solenoideActivo;


  motorEnMovimiento =
    false;


  pasosRestantes =
    0;


  estadoStep =
    LOW;


  digitalWrite(
    X_STEP,
    LOW
  );


  digitalWrite(
    Y_STEP,
    LOW
  );


  digitalWrite(
    SOLENOIDE,
    LOW
  );


  solenoideActivo =
    false;


  detenidoPorStop =
    true;


  ejeActual = '-';


  if (
    habiaActividad
  ) {

    responder(
      origen,
      "STOP: Movimiento/accion interrumpida"
    );
  }

  else {

    responder(
      origen,
      "STOP: Sistema detenido"
    );
  }


  responder(
    origen,
    "OK: STOP ejecutado"
  );
}


// =====================================================
// STATUS
// =====================================================

void mostrarEstado(
  OrigenComando origen
) {

  responder(
    origen,
    "========== STATUS =========="
  );


  if (
    detenidoPorStop
  ) {

    responder(
      origen,
      "STATE: STOPPED"
    );
  }

  else if (
    motorEnMovimiento
  ) {

    responder(
      origen,
      "STATE: MOVING"
    );
  }

  else if (
    solenoideActivo
  ) {

    responder(
      origen,
      "STATE: PUNCHING"
    );
  }

  else {

    responder(
      origen,
      "STATE: READY"
    );
  }


  responder(
    origen,
    "X: ENABLED"
  );

  responder(
    origen,
    "Y: ENABLED"
  );


  if (
    solenoideActivo
  ) {

    responder(
      origen,
      "SOLENOID: ACTIVE"
    );
  }

  else {

    responder(
      origen,
      "SOLENOID: READY"
    );
  }


  if (
    WiFi.status() == WL_CONNECTED
  ) {

    responder(
      origen,
      "WIFI: CONNECTED"
    );
  }

  else {

    responder(
      origen,
      "WIFI: DISCONNECTED"
    );
  }


  if (
    cliente &&
    cliente.connected()
  ) {

    responder(
      origen,
      "TCP: CONNECTED"
    );
  }

  else {

    responder(
      origen,
      "TCP: DISCONNECTED"
    );
  }


  responder(
    origen,
    "SERIAL: OK"
  );


  responder(
    origen,
    "============================"
  );
}


// =====================================================
// HELP
// =====================================================

void mostrarAyuda(
  OrigenComando origen
) {

  responder(
    origen,
    "===== COMANDOS ====="
  );

  responder(
    origen,
    "PING"
  );

  responder(
    origen,
    "STATUS"
  );

  responder(
    origen,
    "MOVE X pasos velocidad"
  );

  responder(
    origen,
    "MOVE Y pasos velocidad"
  );

  responder(
    origen,
    "PUNCH"
  );

  responder(
    origen,
    "STOP"
  );

  responder(
    origen,
    "HELP"
  );

  responder(
    origen,
    "===================="
  );
}


// =====================================================
// RESPONDER AL ORIGEN DEL COMANDO
// =====================================================

void responder(
  OrigenComando origen,
  String mensaje
) {

  if (
    origen == ORIGEN_SERIAL
  ) {

    Serial.println(
      mensaje
    );
  }

  else {

    if (
      cliente &&
      cliente.connected()
    ) {

      cliente.println(
        mensaje
      );
    }
  }
}


// =====================================================
// ENVIAR RESPUESTA A SERIAL + TCP
// =====================================================

void enviarATodos(
  String mensaje
) {

  Serial.println(
    mensaje
  );


  if (
    cliente &&
    cliente.connected()
  ) {

    cliente.println(
      mensaje
    );
  }
}