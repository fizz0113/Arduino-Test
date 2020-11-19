#include <SoftwareSerial.h>
#include "DHT.h"

#define _baudrate   9600
#define DHT_Pin     A3

DHT dht(DHT_Pin, DHT22);
SoftwareSerial mySerial(4,5);

float temp,humi;

void setup(){
  Serial.begin(_baudrate);
  mySerial.begin(_baudrate);
  dht.begin();
}

void loop(){
   temp=dht.readTemperature();
   humi=dht.readHumidity();
   Serial.print("temp=");
   Serial.print(temp);
   Serial.print("\thumi=");
   Serial.println(humi);
   mySerial.write((int)temp);
   mySerial.write((int)humi);
   delay(2000);
}
