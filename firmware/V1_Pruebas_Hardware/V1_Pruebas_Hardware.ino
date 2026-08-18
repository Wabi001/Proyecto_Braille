// =====================================================
// Proyecto: Impresora Braille
// Versión: V1.0 - Pruebas de Hardware
// Prueba conjunta: Motor X + Motor Y + Solenoide
// Microcontrolador: ESP32 WROOM 38 pines
// Drivers: A4988
// =====================================================

// -----------------------------
// EJE X
// -----------------------------
#define X_STEP 4
#define X_DIR 16
#define X_EN 17

// -----------------------------
// EJE Y
// -----------------------------
#define Y_STEP 5
#define Y_DIR 18
#define Y_EN 19

// -----------------------------
// SOLENOIDE
// -----------------------------
#define SOLENOIDE 21

// Velocidad de los motores.
// Número mayor = movimiento más lento.
const int VELOCIDAD = 2000;


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Eje X
  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);
  pinMode(X_EN, OUTPUT);

  // Eje Y
  pinMode(Y_STEP, OUTPUT);
  pinMode(Y_DIR, OUTPUT);
  pinMode(Y_EN, OUTPUT);

  // Solenoide
  pinMode(SOLENOIDE, OUTPUT);

  // Habilitar los A4988
  digitalWrite(X_EN, LOW);
  digitalWrite(Y_EN, LOW);

  // Solenoide apagado
  digitalWrite(SOLENOIDE, LOW);

  Serial.println("======================================");
  Serial.println("PRUEBA GENERAL DE HARDWARE");
  Serial.println("MOTOR X + MOTOR Y + SOLENOIDE");
  Serial.println("======================================");

  delay(2000);
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // ---------------------------------
  // 1. MOVER EJE X
  // ---------------------------------

  Serial.println("1 -> Movimiento eje X");

  digitalWrite(X_DIR, HIGH);

  moverMotorX(1000);

  delay(1000);


  // ---------------------------------
  // 2. GOLPE DEL SOLENOIDE
  // ---------------------------------

  Serial.println("2 -> Golpe solenoide");

  golpearSolenoide();

  delay(1000);


  // ---------------------------------
  // 3. MOVER EJE Y
  // ---------------------------------

  Serial.println("3 -> Movimiento eje Y");

  digitalWrite(Y_DIR, HIGH);

  moverMotorY(500);

  delay(1000);


  // ---------------------------------
  // 4. SEGUNDO GOLPE
  // ---------------------------------

  Serial.println("4 -> Segundo golpe solenoide");

  golpearSolenoide();

  delay(1000);


  // ---------------------------------
  // 5. REGRESAR EJE X
  // ---------------------------------

  Serial.println("5 -> Regreso eje X");

  digitalWrite(X_DIR, LOW);

  moverMotorX(1000);

  delay(1000);


  // ---------------------------------
  // 6. REGRESAR EJE Y
  // ---------------------------------

  Serial.println("6 -> Regreso eje Y");

  digitalWrite(Y_DIR, LOW);

  moverMotorY(500);

  delay(1000);


  Serial.println("==============================");
  Serial.println("CICLO COMPLETADO");
  Serial.println("==============================");

  delay(3000);
}


// =====================================================
// FUNCION MOTOR X
// =====================================================

void moverMotorX(int pasos) {

  for (int i = 0; i < pasos; i++) {

    digitalWrite(X_STEP, HIGH);
    delayMicroseconds(VELOCIDAD);

    digitalWrite(X_STEP, LOW);
    delayMicroseconds(VELOCIDAD);
  }
}


// =====================================================
// FUNCION MOTOR Y
// =====================================================

void moverMotorY(int pasos) {

  for (int i = 0; i < pasos; i++) {

    digitalWrite(Y_STEP, HIGH);
    delayMicroseconds(VELOCIDAD);

    digitalWrite(Y_STEP, LOW);
    delayMicroseconds(VELOCIDAD);
  }
}


// =====================================================
// FUNCION SOLENOIDE
// =====================================================

void golpearSolenoide() {

  digitalWrite(SOLENOIDE, HIGH);

  delay(150);

  digitalWrite(SOLENOIDE, LOW);
}