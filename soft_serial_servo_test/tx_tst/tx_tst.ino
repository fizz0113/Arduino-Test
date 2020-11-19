#include <SoftwareSerial.h>
#define SS_RX 10
#define SS_TX 11
SoftwareSerial mySerial(SS_RX, SS_TX);

int data1,data2;

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  mySerial.write(66);
  mySerial.write(53);
  delay(1000);
}
