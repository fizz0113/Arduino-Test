#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include <Wire.h>

/*  LCD原連接方式
#include <LiquidCrystal_SR.h>
LiquidCrystal_SR lcd(8,4,TWO_WIRE);
                   | |
                   | \-- Clock Pin
                   \---- Data/Enable Pin
*/


/*-------------------------------------------------------------*/
/*  旋轉編碼器 */
#define RotA          A0;
#define RotB          A1;
#define RotP          2;
/*-------------------------------------------------------------*/
/*  RGB_LED */
#define R_pin         3;
#define G_pin         5;
#define B_pin         9;
/*-------------------------------------------------------------*/
/*  超音波 */
#define Trig_Pin      A2;
#define Echo_Pin      4;
float Distance//距離
/*-------------------------------------------------------------*/
/*  DHT11溫室度感測器 */
#define DHT_Pin       A3;
dht11 DHT11;
float temp,humi;//溫溼度
/*-------------------------------------------------------------*/
/*  DS1307計時器 */
/*  DS1307 計時器位置 0x68 */
#define DS1307_I2C_ADDRESS 0x68
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;//時間
/*-------------------------------------------------------------*/
/*  數位小喇叭 */
#define speaker       8;
char toneName[]="CDEFGAB"; //音符表

//unsigned int val1[]={262,277,294,311,330,349,370,392,412,440,466,494};
//unsigned int val2[]={523,554,587,622,659,698,740,784,831,880,932,988};
//unsigned int val3[]={1046,1109,1175,1245,1318,1397,1480,1568,1661,1760,1865,1976};
unsigned int frequency[8]={523,587,659,694,784,880,988};//音符頻率表
char beeTone[]="GEEFDDCDEFGGGGEEFDDCEGGEDDDDDEFEEEEEFGGEEFDDCEGGC";
char starTone[]="CCGGAAGFFEEDDCGGFFEEDGGFFEEDCCGGAAGFFEEDDC";

byte beeBeat[]={1,1,2,1,1,2,1,1,1,1,1,1,2, //小蜜蜂節拍
                 1,1,2,1,1,2,1,1,1,1,4,
                1,1,1,1,1,1,2,1,1,1,1,1,1,2,
                 1,1,2,1,1,2,1,1,1,1,4};

byte starBeat[]={1,1,1,1,1,1,2,1,1,1,1,1,1,2, //小星星節拍
                 1,1,1,1,1,1,2,1,1,1,1,1,1,2,
                 1,1,1,1,1,1,2,1,1,1,1,1,1,2};

const int beeLen=sizeof(beeTone); //小蜜蜂音符總數
const int starLen=sizeof(starTone); //小星星音符總數
unsigned long tempo=180;
/*-------------------------------------------------------------*/
/*  LCD小螢幕 */
LiquidCrystal_I2C lcd(0x27,16,2);// (LCD位置, 每行字元, 行數)
#define K 11;//顯示行數 11

/*  LCD自製字元宣告 */
byte ch[7][8]={
  {B00000,B01110,B00000,B01011,B00000,B01011,B01100,B00110},//民
  {B00000,B01100,B00000,B01000,B00100,B01000,B00100,B00000},//國
  {B00000,B10101,B00000,B00100,B00100,B01110,B00000,B11111},//西
  {B00000,B11111,B00000,B11011,B11011,B10010,B01000,B11111},//元
  {B11011,B10000,B01101,B10001,B10101,B00000,B11101,B11111},//年
  {B10000,B10110,B10000,B10110,B10000,B10110,B01100,B11111},//月
  {B11111,B00000,B01110,B00000,B01110,B00000,B11111,B11111},//日
}

/*  LCD螢幕顯示清單 */
String MENU[] = {
  "A RGBLED Display",
  "B UltraSonic    ",
  "C Temperatire  F".setCharAt(14, (0xDF)),
  "D Humidity      ",
  "E Student ID RLS",
  "F 23-58 UDC     ",
  "G Music Loop    ",
  "H Voice : OFF   ",
  "I DISP01        ",
  "J DISP02        ",
  "K DISP SET      "
};

/*  當清單上被按下 */
volatile boolean A_FLAG,B_FLAG,C_FLAG,D_FLAG,E_FLAG,F_FLAG,G_FLAG,H_FLAG,I_FLAG,J_FLAG,K_FLAG;
/*-------------------------------------------------------------*/

/*  清單及旋轉編碼器 */
byte MENUPTR,CW_CNT,CCW_CNT,Rot_Flag;
byte SET_MENUPTR,SET_CW_CNT,SET_CCW_CNT,SET_Rot_Flag;
byte OLD=0;
byte SET_OLD=0;

volatile boolean sw=0,isrsw=0;
int chk,chg=0;
int cur_x=0;
int cur_y=0;


//LED-----------------------------------------------------------
void RL(int x){
     analogWrite(R_pin,x);
    }
void GL(int x){
     analogWrite(G_pin,x);
    }
void BL(int x){
    analogWrite(B_pin,x);
    }
void OFFL(){
  RL(255);BL(255);GL(255);
}

void showNumber(int value,int colPosition){
  int i;
  lcd.setCursor(colPosition,0);
  for(i=0;i<=2;i++)
     lcd.write(byte(number[value][i]));
  lcd.setCursor(colPosition,1);
  for(i=3;i<=5;i++)
     lcd.write(byte(number[value][i]));
 }
//Ping-----------------------------------------------------------
float Ping() {
  digitalWrite(Trig_Pin, HIGH);   // 觸發腳設定成高電位
  delayMicroseconds(5);           // 持續 5 微秒
  digitalWrite(Trig_Pin, LOW);    // 觸發腳設定成低電位
  return pulseIn(Echo_Pin, HIGH); // 傳回高脈衝的持續時間
}

//playtone-----------------------------------------------------------
void playTone(char toneNo,byte beatNo) //播放音符。
{
 unsigned long duration=beatNo*60000; //計算一拍時間（毫秒）。
 int sc_time=10;
   for(int j=0;j<7;j++){
    ROT_SCAN(sc_time);
    if(G_FLAG==1 | H_FLAG==1){
     if(toneNo==toneName[j]){//查音符表。
         tone(speaker,frequency[j]); //播放音符。
         ROT_SCAN(duration*10000000); //目前音符的節拍。
         noTone(speaker); //關閉聲音。
      }
    }
    else return;
   }
}
//-----------------------------------------------------------
void getDateDs1307(byte *second,
                   byte *minute,
                   byte *hour,
                   byte *dayOfWeek,
                   byte *dayOfMonth,
                   byte *month,
                   byte *year)
{
    // Reset the register pointer
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
    // A few of these need masks because certain bits are control bits
  *second     = bcdToDec(Wire.read() & 0x7f);
  *minute     = bcdToDec(Wire.read());
  *hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}
  // Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return ( (val/10*16) + (val%10) );
}
  // Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return ( (val/16*10) + (val%16) );
}
//-------------------------------------------------------
void MENU_DISP(void) {
  A_FLAG=0;B_FLAG=0; C_FLAG=0; D_FLAG=0; E_FLAG=0; F_FLAG=0;G_FLAG=0;H_FLAG=0;I_FLAG=0;J_FLAG=0;K_FLAG=0; 
  lcd.clear ();
  lcd.print(MENU[MENUPTR%K]);
  lcd.setCursor (0, 1);
  lcd.print(MENU[(MENUPTR+1)%K]);
}
void ROT_LEFT(void) {
  CW_CNT=0; CCW_CNT+=1;
  if (CCW_CNT==2) {
    CCW_CNT=0;
    if (MENUPTR==0) MENUPTR=(K-1);
    else MENUPTR-=1;
    voice_chose(sw,1);
    MENU_DISP();
  }
}
void ROT_RIGHT(void) {
  CCW_CNT=0; CW_CNT+=1;
  if (CW_CNT==2) {
    CW_CNT=0;
    if (MENUPTR==(K-1)) MENUPTR=0;
    else MENUPTR+=1;
    voice_chose(sw,2);
    MENU_DISP();
  }
}
void ROT_SCAN(unsigned int DelN) {
  boolean val_A, val_B, val_P;
  byte  NEW;
WAITN:
  DelN--;
  val_A = digitalRead(RotA);
  val_B = digitalRead(RotB);
  if (val_B==1) NEW=2;
  else NEW=0;
  NEW+=val_A;
  Rot_Flag=NEW^OLD;
  if (Rot_Flag!=0) {
    OLD=NEW;
    if ((Rot_Flag&2)==2) {
      if ((NEW==1)|(NEW==2)) ROT_RIGHT();
      if ((NEW==0)|(NEW==3)) ROT_LEFT();
    } 
    else {
      if ((NEW==0)|(NEW==3)) ROT_RIGHT();
      if ((NEW==1)|(NEW==2)) ROT_LEFT();
    }
  }
  if (DelN != 0) goto WAITN;
}
void ISR_0() {
  switch (MENUPTR) {
    case 0: A_FLAG=1; break;
    case 1: B_FLAG=1; break;
    case 2: C_FLAG=1; break;
    case 3: D_FLAG=1; break;
    case 4: E_FLAG=1; break;
    case 5: F_FLAG=1; break;
    case 6: G_FLAG=1; break;
    case 7: H_FLAG=1; break;
    case 8: I_FLAG=1; break;
    case 9: J_FLAG=1; break;
    case 10: K_FLAG=1; break;
    default: break;
  }
  isrsw=1;
}
void RGB_PRO() {
    int i,y=50,sc_time=10,dl_time=100000;

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          RL(0);
          lcd.setCursor(0,1);
          lcd.print("RedLED            ");
          }
          else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          GL(0);
          lcd.setCursor(0,1);
          lcd.print("GreenLED          ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          BL(0);
          lcd.setCursor(0,1);
          lcd.print("BlueLED           ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          RL(0);GL(0);
          lcd.setCursor(0,1);
          lcd.print("YellowLED         ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          RL(0);BL(0);
          lcd.setCursor(0,1);
          lcd.print("MagentaLED        ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          BL(0);GL(0);
          lcd.setCursor(0,1);
          lcd.print("CyanLED           ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);

      for(i=0;i<y;i++)
      {
        ROT_SCAN(sc_time);
        if(A_FLAG==1){
          RL(0);GL(0);BL(0);
          lcd.setCursor(0,1);
          lcd.print("WhiteLED          ");
        }
        else {OFFL();return;}
      }
      OFFL();ROT_SCAN(dl_time);
}
void Ultra_PRO() {
     Distance = Ping() / 58;         // 把高脈衝時間值換算成公分單位
 lcd.setCursor(0,1);
  if (Distance < 1000) {
    lcd.print("Distance=");
    if(Distance>100){
      lcd.print(int(Distance));
      lcd.print("cm    ");
    }
    else{
      lcd.print(Distance);
      lcd.print("cm  ");
    }
    ROT_SCAN(1000000);
  }
}
void TEMP_PRO() {
  chk = DHT11.read(A3);
  temp = DHT11.temperature;
//  int c=temp;
  int f=temp*9/5+32;
  if(chk==0){
    lcd.setCursor(0,1);
    lcd.print("temp=");
    lcd.print(f);
//    lcd.print(c);
    lcd.print((char) 0xDF);
    lcd.print("F      ");
//    lcd.print("C      ");
    ROT_SCAN(1000000);
  }
}
void Humi_PRO(){
  chk = DHT11.read(A3);
  humi = DHT11.humidity;
  if(chk==0){
    lcd.setCursor(0,1);
    lcd.print("Humi=");
    lcd.print(humi);
    lcd.print("%       ");
    ROT_SCAN(1000000);
  }
}
void UD_CNT() {
ud:
  lcd.setCursor(0,1);
  lcd.print("                 ");
  for(int i=23;i<59;i++){
    ROT_SCAN(1000);
    if(F_FLAG==1){
      lcd.setCursor(0,1);
      lcd.print(i);
      ROT_SCAN(100000);
    }
    else return;
  }
goto ud;
}
void RL_shift() {
  int i,x=9,dl_time=100000,sc_time=10000;

  for(i=0;i<x;i++){
    ROT_SCAN(sc_time);
    if(E_FLAG==0) return;
    else{
    lcd.setCursor(0,1);
    lcd.print("                  ");
    lcd.setCursor(i,1);
    lcd.print("9A417018");
    ROT_SCAN(dl_time);
    }
  }
    for(i=x-2;i>0;i--){
      ROT_SCAN(sc_time);
      if(E_FLAG==0) return;
      else{
      lcd.setCursor(0,1);
      lcd.print("                  ");
      lcd.setCursor(i,1);
      lcd.print("9A417018");
      ROT_SCAN(dl_time);
      }
  }
}
void music_loop(){
  int sc_time=100,dl_time=10000000;
m_l:
//  if(x==1){
     for(int i=0;i<beeLen;i++){//播放「小蜜蜂」樂曲。
      ROT_SCAN(sc_time);
        if(G_FLAG==1){
          playTone(beeTone[i],beeBeat[i]);
        }
        else {
          noTone(speaker);
          return;
        }
     }
    ROT_SCAN(dl_time);
//  }
//  else if(x==2){
     for(int i=0;i<starLen;i++) {//播放「小星星」樂曲。
      ROT_SCAN(sc_time);
        if(G_FLAG==1){
          playTone(starTone[i],starBeat[i]);
        }
        else {
          noTone(speaker);
          return;
        }
     }
     ROT_SCAN(dl_time);
//  }
goto m_l;
//  else return;
}
void voice_sw(){
  if(sw==0){
    sw=1;
    MENU[K-1]="Voice : ON       ";
  }
  else{
    sw=0;
    MENU[K-1]="Voice : OFF      ";
  }
  MENU_DISP();
}
void voice_chose(volatile boolean x,int y){
int val2[]={523,554,587,622,659,698,740,784,831,880,932,988};
  if(x==1){
    if(y==1){
//      if(chg==12) chg=0;
//      else chg+=1;
      tone(speaker,val2[5]);
      delay(50);
      noTone(speaker);
    }
    else if(y==2){
//      if(chg==0) chg=12;
//      else chg-=1;
      tone(speaker,val2[2]);
      delay(50);
      noTone(speaker);
    }
    else{
      tone(speaker,1976);
      delay(50);
      noTone(speaker);
      isrsw=0;
    }
  }
  else noTone(speaker);return;
}
void DISP_01(){
  lcd.home();
  lcd.write(2);
  lcd.write(3);
  lcd.print(year);
  lcd.write(4);
  if(month<10)lcd.print(0);
  lcd.print(month);
  lcd.write(5);
  if(dayOfMonth<10)lcd.print(0);
  lcd.print(dayOfMonth);
  lcd.write(6);
  lcd.print("   ");
  lcd.setCursor(0,1);
  lcd.print("    ");
  if(hour<10) lcd.print(0);
  lcd.print(hour);
  lcd.print(":");
  if(minute<10) lcd.print(0);
  lcd.print(minute);
  lcd.print(":");
  if(second<10) lcd.print(0);
  lcd.print(second);
  lcd.print(" ");
  switch(dayOfWeek){
    case 1:lcd.print("MON");break;
    case 2:lcd.print("TUE");break;
    case 3:lcd.print("WED");break;
    case 4:lcd.print("THU");break;
    case 5:lcd.print("FRI");break;
    case 6:lcd.print("SAT");break;
    case 7:lcd.print("SUN");break;
  }
}
void DISP_02(){
  lcd.home();
  lcd.write(0);
  lcd.write(1);
  lcd.print(year);
  lcd.write(4);
  if(month<10)lcd.print(0);
  lcd.print(month);
  lcd.write(5);
  if(dayOfMonth<10)lcd.print(0);
  lcd.print(dayOfMonth);
  lcd.write(6);
  lcd.print("   ");
  switch(dayOfWeek){
    case 1:lcd.print("MON");break;
    case 2:lcd.print("TUE");break;
    case 3:lcd.print("WED");break;
    case 4:lcd.print("THU");break;
    case 5:lcd.print("FRI");break;
    case 6:lcd.print("SAT");break;
    case 7:lcd.print("SUN");break;
  }
  lcd.setCursor(0,1);
  lcd.print("    ");
  if(hour<10) lcd.print(0);
  lcd.print(hour);
  lcd.print(":");
  if(minute<10) lcd.print(0);
  lcd.print(minute);
  lcd.print(":");
  if(second<10) lcd.print(0);
  lcd.print(second);
  lcd.print("    ");
}
void DISP_SET(){
  lcd.home();
  lcd.print(year);
  lcd.print("/");
  if(month<10)lcd.print(0);
  lcd.print(month);
  lcd.print("/");
  if(dayOfMonth<10)lcd.print(0);
  lcd.print(dayOfMonth);
  lcd.print("week:");
  lcd.print(dayOfWeek);
  lcd.setCursor(0,1);
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(second);
  lcd.print("    ");
  lcd.print("OK");
  lcd.print("  ");
  lcd.home();
  lcd.cursor();
  lcd.setCursor(cur_y,cur_x);
  SET_ROT_SCAN(100000);
  if(digitalRead(RotP)==LOW){
      if(cur_y==0){
        if(cur_x==2){
          year+=10;//////////////
        }
        if(cur_x==3)year+=1;
        if(cur_x==5){
//          if(month>=10)month-=10;
//          else 
          month+=10;
        }
        if(cur_x==6)month+=1;
        if(cur_x==8){
//          if(dayOfMonth>=30)dayOfMonth-=30;
//          else 
          dayOfMonth+=10;
        }
        if(cur_x==9) dayOfMonth+=1;
        if(cur_x==15) dayOfWeek+=1;
      }
      if(cur_y==1){
        if(cur_x==0) hour+=10;
        if(cur_x==1) hour+=1;
        if(cur_x==3) minute+=10;
        if(cur_x==4) minute+=1;
        if(cur_x==6) second+=10;
        if(cur_x==7) second+=1;
        if(cur_x==12|cur_x==13){
          Wire.beginTransmission(DS1307_I2C_ADDRESS);
          Wire.write(0);
          Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
          Wire.write(decToBcd(minute));
          Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                           // bit 6 (also need to change readDateDs1307)
          Wire.write(decToBcd(dayOfWeek));
          Wire.write(decToBcd(dayOfMonth));
          Wire.write(decToBcd(month));
          Wire.write(decToBcd(year));
          Wire.endTransmission();
          MENU_DISP();
        }
      }
  }
}
void SET_ROT_SCAN(unsigned int DelN) {
  boolean val_A, val_B, val_P;
  byte  NEW;
WAITN:
  DelN--;
  val_A = digitalRead(RotA);
  val_B = digitalRead(RotB);
  if (val_B==1) NEW=2;
  else NEW=0;
  NEW+=val_A;
  SET_Rot_Flag=NEW^SET_OLD;
  if (SET_Rot_Flag!=0) {
    SET_OLD=NEW;
    if ((SET_Rot_Flag&2)==2) {
      if ((NEW==1)|(NEW==2)) SET_ROT_RIGHT();
      if ((NEW==0)|(NEW==3)) SET_ROT_LEFT();
    }
    else {
      if ((NEW==0)|(NEW==3)) SET_ROT_RIGHT();
      if ((NEW==1)|(NEW==2)) SET_ROT_LEFT();
    }
  }
  if (DelN != 0) goto WAITN;
}

void SET_ROT_RIGHT(void) {
  SET_CCW_CNT=0; SET_CW_CNT+=1;
  if (SET_CW_CNT==2) {
    SET_CW_CNT=0;
    if (cur_y==0 && cur_x==15) {cur_x=0;cur_y=1;}
    if (cur_y==1 && cur_x==15) {cur_x=0;cur_y=0;}
    else cur_x+=1;
    voice_chose(sw,2);
    DISP_SET();
  }
}
void SET_ROT_LEFT(void) {
  SET_CW_CNT=0; SET_CCW_CNT+=1;
  if (SET_CCW_CNT==2) {
    SET_CCW_CNT=0;
    if (cur_y==0 && cur_x==0) {cur_x=15;cur_y=1;}
    if (cur_y==1 && cur_x==0) {cur_x=15;cur_y=0;}
    else cur_x-=1;
    voice_chose(sw,1);
    DISP_SET();
  }
}


void setup() {
  Serial.begin(9600);
  attachInterrupt(0, ISR_0, FALLING);
  pinMode(RotA, INPUT);
  pinMode(RotB, INPUT);
  pinMode(RotP, INPUT);
  pinMode(Trig_Pin, OUTPUT);  // 觸發腳設定成「輸出」
  pinMode(Echo_Pin, INPUT);   // 接收腳設定成「輸入」
  lcd.init();                    // initialize the LCD
  for(int i=0;i<7;i++){
    lcd.createChar(i,ch[i]);
  }
//  lcd.begin(20,4);
  MENU_DISP();
}
void loop() {
  ROT_SCAN(1000);
  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  if(isrsw==1)voice_chose(sw,3);
  if (A_FLAG==1)RGB_PRO();
  else if (B_FLAG==1)Ultra_PRO() ;
  else if (C_FLAG==1)TEMP_PRO();
  else if (D_FLAG==1)Humi_PRO();
  else if (E_FLAG==1)RL_shift();
  else if (F_FLAG==1)UD_CNT();
  else if (G_FLAG==1)music_loop();
  else if (H_FLAG==1)voice_sw();
  else if (I_FLAG==1)DISP_01();
  else if (J_FLAG==1)DISP_02();
  else if (K_FLAG==1)DISP_SET();
}