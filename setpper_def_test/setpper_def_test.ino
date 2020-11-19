#include <Stepper.h>
#define STEPS 50  //定義步進馬達每圈的步數

//steps:代表馬達轉完一圈需要多少步數。如果馬達上有標示每步的度數，
//將360除以這個角度，就可以得到所需要的步數(例如：360/3.6=100)。(int)

Stepper stepper(STEPS, 11, 10, 9, 8);

void setup()
{
stepper.setSpeed(80);     // 將馬達的速度設定成140RPM 最大  150~160
}

void loop()
{
    stepper.step(STEPS/2);//正半圈
    delay(500);
    stepper.step(0-STEPS/2);//反半圈
    delay(500);
    stepper.step(STEPS);//正1圈
    delay(500);
    stepper.step(0-STEPS);//反1圈
    delay(500);
    stepper.step(STEPS*1.5);//正1圈半
    delay(500);
    stepper.step(0-STEPS*1.5);//反1圈半
    delay(500);
}
