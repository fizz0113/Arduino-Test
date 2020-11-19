#include <Servo.h>

Servo myservo;  // 建立一個 servo 物件，最多可建立 12個 servo

int pos = 0;    // 設定 Servo 位置的變數

void setup() {
  myservo.attach(9);  // 將 servo 物件連接到 pin 9
}

void loop() {
    // 正轉 180度
    for (pos = 0; pos <= 170; pos += 1) // 從 0 度旋轉到 180 度，每次 1 度 
    {
      myservo.write(pos);               // 告訴 servo 走到 'pos' 的位置
      delay(15);                        // 等待 15ms 讓 servo 走到指定位置
    }

    // 反轉 180度
    for (pos = 169; pos >= 0; pos -= 1) // 從 180 度旋轉到 0 度，每次 1 度 
    {
      myservo.write(pos);               // 告訴 servo 走到 'pos' 的位置

      delay(15);                        // 等待 15ms 讓 servo 走到指定位置
    }
}



