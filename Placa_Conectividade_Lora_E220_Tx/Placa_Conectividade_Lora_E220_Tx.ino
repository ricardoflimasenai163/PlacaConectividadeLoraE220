#include "Arduino.h"
#include "LoRa_E220.h"
int cont = 0;

LoRa_E220 e220ttl(&Serial2, 2, 10, 11); //  RX AUX M0 M1

void setup() {
  Serial.begin(9600);
  delay(5000);

  // INICIA O MÓDULO LORA E220
  e220ttl.begin();

  // ENVIA UM DADO DE TESTE
  Serial.println("ENVIANDO UMA MENSAGEM DE TESTE");
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
  // Verfica se o dado foi enviado com sucesso
  Serial.println(rs.getResponseDescription());
}

void loop() {

  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, String(cont++, DEC));
  // Verfica se o dado foi enviado com sucesso
  Serial.println(rs.getResponseDescription());
  delay(2000);
}

