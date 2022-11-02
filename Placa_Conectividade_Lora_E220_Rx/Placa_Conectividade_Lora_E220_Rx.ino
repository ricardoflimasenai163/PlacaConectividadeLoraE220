
#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "owl.h"
// DisplaySSD1306_128x64_SPI display(RST, { BUS number, CS pin, DC pin, FREQ (0 means default), CLK pin, MOSI pin});
DisplaySSD1306_128x64_SPI display(20, { -1, 16, 17, 0, 18, 19}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPS.h>
#define ss Serial1
String Rssi,latitude,longitude;
LoRa_E220 e220ttl(&Serial2, 2, 10, 11); //  RX AUX M0 M1
TinyGPS gps;


void setup()
{
  // CONFIGURA PINOS PARA LED RGB
  pinMode(26, OUTPUT); // LED RGB - PINO VERMELHO
  pinMode(22, OUTPUT); // LED RGB - PINO VERDE
  pinMode(21, OUTPUT); // LED RGB - PINO AZUL
  analogWrite(26, 0);
  analogWrite(22, 255);
  analogWrite(21, 0);
  Serial.begin(9600);
  delay(5000);

  // INICIA COMUNICAÇÃO SERIAL RADIO LORA E220
  e220ttl.begin();
  Serial.println("Start receiving!");

  // CONFIGURA DISPLAY OLED
  display.setFixedFont( ssd1306xled_font6x8 );
  display.begin();
  display.clear();
  display.printFixed(0,  8, "FEEC UNICAMP", STYLE_NORMAL);

  // INICIA SERIAL DO GPS
  ss.begin(9600);
  Serial.println(TinyGPS::library_version());
  Serial.println();

}

void loop()
{
  //################################################################
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    latitude =  String(flat,5);
    Serial.print(latitude);
    Serial.print(" LON=");
    longitude =  String(flon,5);
    Serial.print(longitude);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.println(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }

  //################################################################

  if (e220ttl.available() > 1) {
    Serial.println("Message received!");
    ResponseContainer rc = e220ttl.receiveMessageRSSI();

    if (rc.status.code != 1) {
      Serial.println(rc.status.getResponseDescription());
    }
    else {
      String dado = (rc.data).substring(3);
      Rssi = String((-(256 - rc.rssi)), DEC);
      Serial.println("RSSI: " + Rssi);
      dado = "Pacote n: " + dado;
      Serial.println(dado + '\n');

      //########## CONVERTE STRING EM VETOR CHAR >> RSSI
      int str_len = Rssi.length() + 1;
      char valor_rssi[str_len];
      Rssi.toCharArray(valor_rssi, str_len);

      //########## CONVERTE STRING EM VETOR CHAR >> DADO RECEBIDO
      int str_len1 = dado.length() + 1;
      char dado_recebido[str_len1];
      dado.toCharArray(dado_recebido, str_len1);

      //#################################

            //########## CONVERTE STRING EM VETOR CHAR >> LATITUDE
      str_len1 = latitude.length() + 1;
      char valor_lat[str_len1];
      latitude.toCharArray(valor_lat, str_len1);

      //#################################

            //########## CONVERTE STRING EM VETOR CHAR >> LONGITUDE
      str_len1 = longitude.length() + 1;
      char valor_long[str_len1];
      longitude.toCharArray(valor_long, str_len1);

      //#################################

      display.clear();
      display.setFixedFont( ssd1306xled_font8x16 );
      display.printFixed(0,  0, dado_recebido, STYLE_NORMAL);
      display.printFixed(0,  20, "RSSI: ", STYLE_NORMAL);
      display.printFixed(50,  20, valor_rssi, STYLE_NORMAL);
      display.printFixed(0,  35, "LAT: ", STYLE_NORMAL);
      display.printFixed(50,  35, valor_lat, STYLE_NORMAL);
      display.printFixed(0,  55, "LONG: ", STYLE_NORMAL);
      display.printFixed(50,  55, valor_long, STYLE_NORMAL);

    }
  }
}
