#include <Arduino.h>

// Pino conectado no C1
#define PIN_C1 4  

// Vamos usar Serial1 para mandar dados pelo pino C1
// TX = PIN_C1, RX não usado (coloca -1)
HardwareSerial C1Serial(1);

void send_request(int command, int a, int b, int c, int d, int e) {
  int checksum = (command + a + b + c + d + e);

  // Pulso inicial
  pinMode(PIN_C1, OUTPUT);
  digitalWrite(PIN_C1, LOW);
  delayMicroseconds(2750);
  digitalWrite(PIN_C1, HIGH);
  delayMicroseconds(200);

  // Inicia serial a 4800 baud no pino C1
  C1Serial.begin(4800, SERIAL_8N1, -1, PIN_C1);

  // Envia dados
  C1Serial.write((byte)0x00);
  C1Serial.write(command);
  C1Serial.write(a);
  C1Serial.write(b);
  C1Serial.write(c);
  C1Serial.write(d);
  C1Serial.write(e);
  C1Serial.write(checksum);
  C1Serial.flush();
  C1Serial.end();

  // Mantém pino alto no final
  pinMode(PIN_C1, OUTPUT);
  digitalWrite(PIN_C1, HIGH);
}

void reset_code(int a, int b, int c, int d, int e) {
  send_request(0x75, a, b, c, d, e);
}

void try_code(int a, int b, int c, int d, int e) {
  send_request(0x71, a, b, c, d, e);
}

void setup() {
  // Exemplo: definir código 1-2-3-4-5 e tentar abrir
  reset_code(1, 2, 3, 4, 5);
  delay(1000);
  try_code(1, 2, 3, 4, 5);
}

void loop() {
  // Não faz nada no loop
}
