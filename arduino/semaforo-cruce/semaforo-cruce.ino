/*
 * MitsuMira — Semáforo de cruce v1
 *
 * Lógica:
 *   - Ciclo normal: Coche A verde → amarillo → todo rojo → Coche B verde → amarillo → todo rojo → repetir
 *   - Los peatones están siempre en ROJO durante el ciclo de coches
 *   - Cualquier pulsador activa una interrupción hardware → activa un flag
 *   - El flag se comprueba en la siguiente transición todo-rojo
 *   - Si el flag está activo: se extiende el todo-rojo con cruce peatonal
 *   - Durante el cruce: ambos peatones en VERDE simultáneamente
 *   - Parpadeo de aviso antes de cerrar el cruce
 *   - Segunda pulsación durante el cruce: ignorada hasta el siguiente ciclo
 *
 * Pines:
 *   BTN_A           : 2  (INT0 — interrupción hardware)
 *   BTN_B           : 3  (INT1 — interrupción hardware)
 *   Coche A         : Rojo=4, Amarillo=5, Verde=6
 *   Peatón A        : Rojo=7, Verde=8
 *   Coche B         : Rojo=9, Amarillo=10, Verde=11
 *   Peatón B        : Rojo=12, Verde=13
 */

// --- Pines ---
const int BTN_A            = 2;   // INT0
const int BTN_B            = 3;   // INT1
const int COCHE_A_ROJO     = 4;
const int COCHE_A_AMARILLO = 5;
const int COCHE_A_VERDE    = 6;
const int PEAT_A_ROJO      = 7;
const int PEAT_A_VERDE     = 8;
const int COCHE_B_ROJO     = 9;
const int COCHE_B_AMARILLO = 10;
const int COCHE_B_VERDE    = 11;
const int PEAT_B_ROJO      = 12;
const int PEAT_B_VERDE     = 13;

// --- Tiempos en milisegundos ---
const unsigned long T_COCHE_VERDE    = 8000;  // tiempo en verde para los coches
const unsigned long T_COCHE_AMARILLO = 2000;  // tiempo en amarillo
const unsigned long T_TODO_ROJO      = 1000;  // transición todo rojo entre fases
const unsigned long T_PEAT_VERDE     = 6000;  // tiempo total del verde peatonal (incluye parpadeo)
const unsigned long T_PEAT_PARPADEO  = 2000;  // últimos 2s del verde — parpadeo de aviso
const unsigned long T_INTERVALO_BLINK = 400;  // intervalo del parpadeo

// --- Estados posibles del semáforo ---
enum Estado {
  COCHE_A_VERDE_E,      // Coche A en verde, Coche B en rojo
  COCHE_A_AMARILLO_E,   // Coche A en amarillo
  TODO_ROJO_1,          // Todo rojo entre A→B: aquí se comprueba el pulsador
  COCHE_B_VERDE_E,      // Coche B en verde, Coche A en rojo
  COCHE_B_AMARILLO_E,   // Coche B en amarillo
  TODO_ROJO_2,          // Todo rojo entre B→A: aquí se comprueba el pulsador
  PEAT_VERDE_E,         // Cruce peatonal: ambos peatones en verde
  PEAT_PARPADEO_E,      // Cruce peatonal: parpadeo de aviso
  PEAT_TODO_ROJO        // Todo rojo tras el cruce — vuelta al ciclo de coches
};

// Declaración adelantada — necesaria para avr-gcc / Tinkercad
void cambiarEstado(Estado e);

Estado estadoActual;
Estado estadoRetorno;       // estado al que se vuelve tras el cruce peatonal
unsigned long inicioEstado;
unsigned long ultimoParpadeo;
bool luzParpadeo = true;

// volatile: variable escrita por la ISR, leída en el loop principal
volatile bool peatonPulsado = false;

// --- ISR compartida para los dos pulsadores ---
// Se llama automáticamente cuando cualquier pulsador se pulsa
void alPulsar() {
  peatonPulsado = true;
}

// -----------------------------------------------
// Apaga todos los LEDs
// -----------------------------------------------
void apagarTodo() {
  int pines[] = {
    COCHE_A_ROJO, COCHE_A_AMARILLO, COCHE_A_VERDE,
    COCHE_B_ROJO, COCHE_B_AMARILLO, COCHE_B_VERDE,
    PEAT_A_ROJO, PEAT_A_VERDE,
    PEAT_B_ROJO, PEAT_B_VERDE
  };
  for (int p : pines) digitalWrite(p, LOW);
}

// -----------------------------------------------
// Cambia al nuevo estado y activa los LEDs correspondientes
// -----------------------------------------------
void cambiarEstado(Estado e) {
  estadoActual    = e;
  inicioEstado    = millis();
  ultimoParpadeo  = millis();
  luzParpadeo     = true;

  apagarTodo();

  switch (e) {

    case COCHE_A_VERDE_E:
      // Coche A circula — peatones en rojo
      digitalWrite(COCHE_A_VERDE, HIGH);
      digitalWrite(COCHE_B_ROJO,  HIGH);
      digitalWrite(PEAT_A_ROJO,   HIGH);
      digitalWrite(PEAT_B_ROJO,   HIGH);
      break;

    case COCHE_A_AMARILLO_E:
      digitalWrite(COCHE_A_AMARILLO, HIGH);
      digitalWrite(COCHE_B_ROJO,     HIGH);
      digitalWrite(PEAT_A_ROJO,      HIGH);
      digitalWrite(PEAT_B_ROJO,      HIGH);
      break;

    case TODO_ROJO_1:
    case TODO_ROJO_2:
    case PEAT_TODO_ROJO:
      // Todo rojo: coches parados, peatones en rojo
      digitalWrite(COCHE_A_ROJO, HIGH);
      digitalWrite(COCHE_B_ROJO, HIGH);
      digitalWrite(PEAT_A_ROJO,  HIGH);
      digitalWrite(PEAT_B_ROJO,  HIGH);
      break;

    case COCHE_B_VERDE_E:
      // Coche B circula — peatones en rojo
      digitalWrite(COCHE_A_ROJO,  HIGH);
      digitalWrite(COCHE_B_VERDE, HIGH);
      digitalWrite(PEAT_A_ROJO,   HIGH);
      digitalWrite(PEAT_B_ROJO,   HIGH);
      break;

    case COCHE_B_AMARILLO_E:
      digitalWrite(COCHE_A_ROJO,     HIGH);
      digitalWrite(COCHE_B_AMARILLO, HIGH);
      digitalWrite(PEAT_A_ROJO,      HIGH);
      digitalWrite(PEAT_B_ROJO,      HIGH);
      break;

    case PEAT_VERDE_E:
      // Cruce peatonal: coches parados, ambos peatones en verde
      digitalWrite(COCHE_A_ROJO,  HIGH);
      digitalWrite(COCHE_B_ROJO,  HIGH);
      digitalWrite(PEAT_A_VERDE,  HIGH);
      digitalWrite(PEAT_B_VERDE,  HIGH);
      break;

    case PEAT_PARPADEO_E:
      // El parpadeo lo gestiona gestionarParpadeo()
      // Ponemos verde como estado inicial antes de que empiece a parpadear
      digitalWrite(COCHE_A_ROJO,  HIGH);
      digitalWrite(COCHE_B_ROJO,  HIGH);
      digitalWrite(PEAT_A_VERDE,  HIGH);
      digitalWrite(PEAT_B_VERDE,  HIGH);
      break;
  }
}

// -----------------------------------------------
// Hace parpadear los dos LEDs verdes de peatón
// -----------------------------------------------
void gestionarParpadeo() {
  if (millis() - ultimoParpadeo >= T_INTERVALO_BLINK) {
    luzParpadeo = !luzParpadeo;
    digitalWrite(PEAT_A_VERDE, luzParpadeo ? HIGH : LOW);
    digitalWrite(PEAT_B_VERDE, luzParpadeo ? HIGH : LOW);
    ultimoParpadeo = millis();
  }
}

// -----------------------------------------------
// Máquina de estados — se ejecuta en cada iteración del loop
// -----------------------------------------------
void actualizar() {
  unsigned long tiempoTranscurrido = millis() - inicioEstado;

  switch (estadoActual) {

    case COCHE_A_VERDE_E:
      if (tiempoTranscurrido >= T_COCHE_VERDE) cambiarEstado(COCHE_A_AMARILLO_E);
      break;

    case COCHE_A_AMARILLO_E:
      if (tiempoTranscurrido >= T_COCHE_AMARILLO) cambiarEstado(TODO_ROJO_1);
      break;

    case TODO_ROJO_1:
      if (tiempoTranscurrido >= T_TODO_ROJO) {
        if (peatonPulsado) {
          // Hay una petición de cruce — activamos el cruce peatonal
          peatonPulsado = false;
          estadoRetorno = COCHE_B_VERDE_E; // después del cruce, sigue el Coche B
          cambiarEstado(PEAT_VERDE_E);
        } else {
          cambiarEstado(COCHE_B_VERDE_E);
        }
      }
      break;

    case COCHE_B_VERDE_E:
      if (tiempoTranscurrido >= T_COCHE_VERDE) cambiarEstado(COCHE_B_AMARILLO_E);
      break;

    case COCHE_B_AMARILLO_E:
      if (tiempoTranscurrido >= T_COCHE_AMARILLO) cambiarEstado(TODO_ROJO_2);
      break;

    case TODO_ROJO_2:
      if (tiempoTranscurrido >= T_TODO_ROJO) {
        if (peatonPulsado) {
          peatonPulsado = false;
          estadoRetorno = COCHE_A_VERDE_E; // después del cruce, sigue el Coche A
          cambiarEstado(PEAT_VERDE_E);
        } else {
          cambiarEstado(COCHE_A_VERDE_E);
        }
      }
      break;

    case PEAT_VERDE_E:
      // Cuando faltan T_PEAT_PARPADEO ms para terminar, pasamos al parpadeo
      if (tiempoTranscurrido >= T_PEAT_VERDE - T_PEAT_PARPADEO) {
        cambiarEstado(PEAT_PARPADEO_E);
      }
      break;

    case PEAT_PARPADEO_E:
      gestionarParpadeo();
      if (tiempoTranscurrido >= T_PEAT_PARPADEO) cambiarEstado(PEAT_TODO_ROJO);
      break;

    case PEAT_TODO_ROJO:
      if (tiempoTranscurrido >= T_TODO_ROJO) cambiarEstado(estadoRetorno);
      break;
  }
}

// -----------------------------------------------
void setup() {
  // Configurar pines de salida
  int salidas[] = {
    COCHE_A_ROJO, COCHE_A_AMARILLO, COCHE_A_VERDE,
    COCHE_B_ROJO, COCHE_B_AMARILLO, COCHE_B_VERDE,
    PEAT_A_ROJO, PEAT_A_VERDE,
    PEAT_B_ROJO, PEAT_B_VERDE
  };
  for (int p : salidas) pinMode(p, OUTPUT);

  // Pulsadores con resistencia pull-up interna (no necesitan resistencia externa)
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);

  // Interrupciones hardware
  // FALLING = flanco de bajada (el pulsador lleva el pin de HIGH a LOW al pulsar)
  attachInterrupt(digitalPinToInterrupt(BTN_A), alPulsar, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_B), alPulsar, FALLING);

  // Estado inicial
  cambiarEstado(COCHE_A_VERDE_E);
}

// -----------------------------------------------
void loop() {
  actualizar();
}