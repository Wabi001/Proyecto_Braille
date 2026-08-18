// =====================================================
// Proyecto: Impresora Braille
// Versión: V2.4 - Comunicación Serial
//
// Funciones:
// - PING
// - STATUS
// - HELP
// - MOVE X pasos velocidad
// - MOVE Y pasos velocidad
// - PUNCH
// - STOP
//
// Microcontrolador: ESP32 WROOM 38 pines
// Drivers: A4988
// Solenoide mediante MOSFET
// =====================================================


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
// CONFIGURACIÓN
// =====================================================

const int VELOCIDAD_MIN = 50;
const int VELOCIDAD_MAX = 2000;

const unsigned long TIEMPO_GOLPE = 150;


// =====================================================
// COMUNICACIÓN SERIAL
// =====================================================

String comando = "";


// =====================================================
// VARIABLES DEL MOVIMIENTO
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
// VARIABLES DEL SOLENOIDE
// =====================================================

bool solenoideActivo = false;

unsigned long tiempoInicioSolenoide = 0;


// =====================================================
// ESTADO STOP
// =====================================================

bool detenidoPorStop = false;


// =====================================================
// SETUP
// =====================================================

void setup() {

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


  // A4988:
  // LOW = habilitado

  digitalWrite(X_EN, LOW);
  digitalWrite(Y_EN, LOW);


  delay(500);


  // ---------------------------------------------------
  // MENSAJE INICIAL
  // ---------------------------------------------------

  Serial.println();
  Serial.println("======================================");
  Serial.println("IMPRESORA BRAILLE - ESP32");
  Serial.println("V2.4 - CONTROL SERIAL + STOP");
  Serial.println("======================================");

  Serial.println();

  Serial.println("Sistema listo.");

  Serial.println();

  Serial.println("Comandos:");

  Serial.println("PING");
  Serial.println("STATUS");
  Serial.println("HELP");

  Serial.println("MOVE X pasos velocidad");
  Serial.println("MOVE Y pasos velocidad");

  Serial.println("PUNCH");
  Serial.println("STOP");

  Serial.println();
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Leer comandos sin bloquear el ESP32
  leerSerial();

  // Actualizar movimiento
  actualizarMovimiento();

  // Actualizar solenoide
  actualizarSolenoide();
}


// =====================================================
// LECTURA SERIAL NO BLOQUEANTE
// =====================================================

void leerSerial() {

  while (Serial.available() > 0) {

    char caracter = Serial.read();


    // -----------------------------------------------
    // ENTER
    // -----------------------------------------------

    if (
      caracter == '\n' ||
      caracter == '\r'
    ) {

      if (comando.length() > 0) {

        comando.trim();

        comando.toUpperCase();

        procesarComando(comando);

        comando = "";
      }
    }


    // -----------------------------------------------
    // CARÁCTER NORMAL
    // -----------------------------------------------

    else {

      comando += caracter;


      // Protección contra comandos demasiado largos

      if (comando.length() > 100) {

        comando = "";

        Serial.println(
          "ERROR: Comando demasiado largo"
        );
      }
    }
  }
}


// =====================================================
// PROCESAR COMANDO
// =====================================================

void procesarComando(String cmd) {


  // ---------------------------------------------------
  // PING
  // ---------------------------------------------------

  if (cmd == "PING") {

    Serial.println("PONG");

    return;
  }


  // ---------------------------------------------------
  // STATUS
  // ---------------------------------------------------

  if (cmd == "STATUS") {

    mostrarEstado();

    return;
  }


  // ---------------------------------------------------
  // HELP
  // ---------------------------------------------------

  if (cmd == "HELP") {

    mostrarAyuda();

    return;
  }


  // ---------------------------------------------------
  // STOP
  // ---------------------------------------------------

  if (cmd == "STOP") {

    detenerSistema();

    return;
  }


  // ---------------------------------------------------
  // PUNCH
  // ---------------------------------------------------

  if (cmd == "PUNCH") {

    iniciarGolpe();

    return;
  }


  // ---------------------------------------------------
  // MOVE
  // ---------------------------------------------------

  if (cmd.startsWith("MOVE ")) {

    procesarMovimiento(cmd);

    return;
  }


  // ---------------------------------------------------
  // ERROR
  // ---------------------------------------------------

  Serial.print(
    "ERROR: Comando desconocido -> "
  );

  Serial.println(cmd);
}


// =====================================================
// PROCESAR MOVE
// =====================================================

void procesarMovimiento(String cmd) {


  // No iniciar otro movimiento mientras está ocupado

  if (
    motorEnMovimiento ||
    solenoideActivo
  ) {

    Serial.println(
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
  // VALIDAR FORMATO
  // ---------------------------------------------------

  if (
    espacio1 == -1 ||
    espacio2 == -1 ||
    espacio3 == -1
  ) {

    Serial.println(
      "ERROR: Formato incorrecto"
    );

    Serial.println(
      "Uso: MOVE X pasos velocidad"
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


  String textoPasos =
    cmd.substring(
      espacio2 + 1,
      espacio3
    );


  String textoVelocidad =
    cmd.substring(
      espacio3 + 1
    );


  long pasos =
    textoPasos.toInt();


  int velocidad =
    textoVelocidad.toInt();


  // ---------------------------------------------------
  // VALIDAR EJE
  // ---------------------------------------------------

  if (
    eje != "X" &&
    eje != "Y"
  ) {

    Serial.println(
      "ERROR: Eje invalido. Use X o Y."
    );

    return;
  }


  // ---------------------------------------------------
  // VALIDAR PASOS
  // ---------------------------------------------------

  if (pasos == 0) {

    Serial.println(
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

    Serial.print(
      "ERROR: Velocidad permitida: "
    );

    Serial.print(
      VELOCIDAD_MIN
    );

    Serial.print(" - ");

    Serial.print(
      VELOCIDAD_MAX
    );

    Serial.println(
      " pasos/s"
    );

    return;
  }


  // ---------------------------------------------------
  // SELECCIONAR EJE X
  // ---------------------------------------------------

  if (eje == "X") {

    ejeActual = 'X';

    pinStepActual = X_STEP;

    pinDirActual = X_DIR;
  }


  // ---------------------------------------------------
  // SELECCIONAR EJE Y
  // ---------------------------------------------------

  else {

    ejeActual = 'Y';

    pinStepActual = Y_STEP;

    pinDirActual = Y_DIR;
  }


  // ---------------------------------------------------
  // DIRECCIÓN
  // ---------------------------------------------------

  if (pasos > 0) {

    digitalWrite(
      pinDirActual,
      HIGH
    );

    pasosRestantes = pasos;
  }

  else {

    digitalWrite(
      pinDirActual,
      LOW
    );

    pasosRestantes = -pasos;
  }


  // ---------------------------------------------------
  // CALCULAR VELOCIDAD
  // ---------------------------------------------------

  intervaloPulso =
    500000UL / velocidad;


  // ---------------------------------------------------
  // PREPARAR MOVIMIENTO
  // ---------------------------------------------------

  digitalWrite(
    pinStepActual,
    LOW
  );

  estadoStep = LOW;

  tiempoAnteriorPaso =
    micros();


  motorEnMovimiento = true;

  detenidoPorStop = false;


  // ---------------------------------------------------
  // INFORMACIÓN
  // ---------------------------------------------------

  Serial.print(
    "MOVIMIENTO INICIADO | Eje: "
  );

  Serial.print(
    ejeActual
  );

  Serial.print(
    " | Pasos: "
  );

  Serial.print(
    pasosRestantes
  );

  Serial.print(
    " | Velocidad: "
  );

  Serial.print(
    velocidad
  );

  Serial.println(
    " pasos/s"
  );
}


// =====================================================
// ACTUALIZAR MOVIMIENTO
// =====================================================

void actualizarMovimiento() {

  if (!motorEnMovimiento) {

    return;
  }


  unsigned long tiempoActual =
    micros();


  // Comprobar tiempo de medio pulso

  if (
    tiempoActual - tiempoAnteriorPaso
    >= intervaloPulso
  ) {

    tiempoAnteriorPaso =
      tiempoActual;


    // -----------------------------------------------
    // STEP LOW -> HIGH
    // -----------------------------------------------

    if (estadoStep == LOW) {

      digitalWrite(
        pinStepActual,
        HIGH
      );

      estadoStep = HIGH;
    }


    // -----------------------------------------------
    // STEP HIGH -> LOW
    // Un paso completo ha terminado
    // -----------------------------------------------

    else {

      digitalWrite(
        pinStepActual,
        LOW
      );

      estadoStep = LOW;


      if (pasosRestantes > 0) {

        pasosRestantes--;
      }


      // ---------------------------------------------
      // MOVIMIENTO TERMINADO
      // ---------------------------------------------

      if (pasosRestantes == 0) {

        motorEnMovimiento = false;


        Serial.print(
          "OK: Movimiento "
        );

        Serial.print(
          ejeActual
        );

        Serial.println(
          " completado"
        );


        ejeActual = '-';
      }
    }
  }
}


// =====================================================
// INICIAR PUNCH
// =====================================================

void iniciarGolpe() {

  if (
    motorEnMovimiento ||
    solenoideActivo
  ) {

    Serial.println(
      "ERROR: Sistema ocupado."
    );

    return;
  }


  detenidoPorStop = false;


  Serial.println(
    "PUNCH: Activando solenoide"
  );


  digitalWrite(
    SOLENOIDE,
    HIGH
  );


  solenoideActivo = true;


  tiempoInicioSolenoide =
    millis();
}


// =====================================================
// ACTUALIZAR SOLENOIDE
// =====================================================

void actualizarSolenoide() {

  if (!solenoideActivo) {

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


    solenoideActivo = false;


    Serial.println(
      "OK: PUNCH completado"
    );
  }
}


// =====================================================
// STOP
// =====================================================

void detenerSistema() {

  bool habiaActividad =
    motorEnMovimiento ||
    solenoideActivo;


  // ---------------------------------------------------
  // DETENER MOTOR
  // ---------------------------------------------------

  motorEnMovimiento = false;

  pasosRestantes = 0;

  estadoStep = LOW;


  // Asegurar STEP en LOW

  digitalWrite(
    X_STEP,
    LOW
  );

  digitalWrite(
    Y_STEP,
    LOW
  );


  // ---------------------------------------------------
  // APAGAR SOLENOIDE
  // ---------------------------------------------------

  digitalWrite(
    SOLENOIDE,
    LOW
  );

  solenoideActivo = false;


  // ---------------------------------------------------
  // ESTADO
  // ---------------------------------------------------

  detenidoPorStop = true;

  ejeActual = '-';


  if (habiaActividad) {

    Serial.println(
      "STOP: Movimiento/accion interrumpida"
    );
  }

  else {

    Serial.println(
      "STOP: Sistema detenido"
    );
  }


  Serial.println(
    "OK: STOP ejecutado"
  );
}


// =====================================================
// STATUS
// =====================================================

void mostrarEstado() {

  Serial.println();

  Serial.println(
    "========== STATUS =========="
  );


  // ---------------------------------------------------
  // ESTADO GENERAL
  // ---------------------------------------------------

  if (detenidoPorStop) {

    Serial.println(
      "STATE: STOPPED"
    );
  }

  else if (motorEnMovimiento) {

    Serial.println(
      "STATE: MOVING"
    );
  }

  else if (solenoideActivo) {

    Serial.println(
      "STATE: PUNCHING"
    );
  }

  else {

    Serial.println(
      "STATE: READY"
    );
  }


  // ---------------------------------------------------
  // MOTORES
  // ---------------------------------------------------

  Serial.println(
    "X: ENABLED"
  );

  Serial.println(
    "Y: ENABLED"
  );


  // ---------------------------------------------------
  // MOVIMIENTO
  // ---------------------------------------------------

  if (motorEnMovimiento) {

    Serial.print(
      "AXIS: "
    );

    Serial.println(
      ejeActual
    );


    Serial.print(
      "STEPS REMAINING: "
    );

    Serial.println(
      pasosRestantes
    );
  }


  // ---------------------------------------------------
  // SOLENOIDE
  // ---------------------------------------------------

  if (solenoideActivo) {

    Serial.println(
      "SOLENOID: ACTIVE"
    );
  }

  else {

    Serial.println(
      "SOLENOID: READY"
    );
  }


  Serial.println(
    "SERIAL: OK"
  );


  Serial.println(
    "============================"
  );

  Serial.println();
}


// =====================================================
// HELP
// =====================================================

void mostrarAyuda() {

  Serial.println();

  Serial.println(
    "========== AYUDA =========="
  );


  Serial.println(
    "PING"
  );

  Serial.println(
    "  Comprobar comunicacion"
  );


  Serial.println();


  Serial.println(
    "STATUS"
  );

  Serial.println(
    "  Consultar estado"
  );


  Serial.println();


  Serial.println(
    "MOVE X pasos velocidad"
  );

  Serial.println(
    "  Mover eje X"
  );


  Serial.println();


  Serial.println(
    "MOVE Y pasos velocidad"
  );

  Serial.println(
    "  Mover eje Y"
  );


  Serial.println();


  Serial.println(
    "PUNCH"
  );

  Serial.println(
    "  Accionar solenoide"
  );


  Serial.println();


  Serial.println(
    "STOP"
  );

  Serial.println(
    "  Detener movimiento o solenoide"
  );


  Serial.println();

  Serial.println(
    "Ejemplos:"
  );

  Serial.println(
    "MOVE X 200 100"
  );

  Serial.println(
    "MOVE Y -200 100"
  );

  Serial.println(
    "PUNCH"
  );

  Serial.println(
    "STOP"
  );


  Serial.println();

  Serial.println(
    "==========================="
  );

  Serial.println();
}
