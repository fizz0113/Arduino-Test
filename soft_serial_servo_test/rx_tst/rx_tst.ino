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
  if(mySerial.available()>1){
      data1=mySerial.read();
      data2=mySerial.read();
      mySerial.print("Data 1  ");
      mySerial.println(data1);
      mySerial.print("Data 2  ");
      mySerial.println(data2);
      mySerial.println();
    }
}
