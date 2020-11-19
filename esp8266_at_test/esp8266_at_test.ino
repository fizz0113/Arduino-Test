#include <SoftwareSerial.h>
#define _baudrate   115200
#define _rxpin      4
#define _txpin      5
SoftwareSerial esp8266( _rxpin, _txpin ); // RX, TX

void setup() {
  Serial.begin( _baudrate );
  esp8266.begin( _baudrate );
  esp8266.println("AT");
}
// AT+UART_CUR=115200,8,1,0,0

void loop(){
  if (esp8266.available()){
    Serial.write(esp8266.read());
  }

  if (Serial.available()){
    esp8266.write(Serial.read());
  }
  delayMicroseconds(30);
}



