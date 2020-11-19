/*  Include ------------------------------------------------------------------------------------------*/
#include "DFRobot_EC.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>

/*  Define  ------------------------------------------------------------------------------------------*/
#define ARDUINO_BAUD            9600          // arduino串列阜通訊鮑瑞
#define XBEE_RX                 9             // XBEE RX
#define XBEE_TX                 10            // XBEE TX
#define EC_PIN                  A1            // EC感測器腳位
#define TRIG_PIN                6             // 超音波感測-發射
#define ECHO_PIN                7             // 超音波感測-接收

#define RELAY_WATER             2             // 純水抽水馬達繼電器腳位
#define RELAY_FERTILIZER        3             // 肥料抽水馬達繼電器腳位
#define RELAY_3                 4             // 繼電器3腳位
#define RELAY_4                 5             // 繼電器4腳位

#define MIX_WATER_STAGE_H       10            // 混合桶高水位
#define MIX_WATER_STAGE_L       20            // 混和桶低水位
DFRobot_EC ec;
SoftwareSerial conn(XBEE_RX, XBEE_TX);

/*  Var     ------------------------------------------------------------------------------------------*/
int i, j, k;                                  // 迴圈用
unsigned long time_twoH_ms,
              time_fivH_ms,
              time_one_sec,
              time_three_sec,
              time_five_sec,
              time_ten_sec;

double ecVal;                                 // 濃度值
double ultraVal;                              // 水位距離數值 (超音波感測)

double goal_ec=-1;                            // 指定濃度值

volatile boolean flag_relay_water,
                  flag_relay_fertilizer;

volatile byte duration_relay_water,
              duration_relay_fertilizer;

/*  Func    ------------------------------------------------------------------------------------------*/

void Relay_PRO()
{
    digitalWrite(RELAY_WATER, !flag_relay_water);
    digitalWrite(RELAY_FERTILIZER, !flag_relay_fertilizer);
}

void EC_PRO(double temperature=25)
{
    double vol_ec = analogRead(EC_PIN)/1024.0*5000;                 // ec感測器感測電壓
    ec.calibration(vol_ec, temperature);                            // ec值校正
    double tmp = 0;                                                 // 儲存多次感測值
    for(i=0; i<5; i++)
    {
        tmp += ec.readEC(vol_ec, temperature);
    }
    ecVal = tmp/5;                                                  // 將多次感測值取均值 (避免凸波)
}

void Ultra_PRO()
{
    digitalWrite(TRIG_PIN, HIGH);                                                     // 觸發腳設定成高電位
    delayMicroseconds(5);                                                             // 持續5微秒
    digitalWrite(TRIG_PIN, LOW);                                                      // 觸發腳設定成低電位
    double ping = pulseIn(ECHO_PIN, HIGH);                                            // 儲存多次高脈衝的持續時間
    ultraVal = 5 < (ping / 58) && (ping / 58) < 500 ? (ping / 58) : ultraVal;         // 把多次高脈衝時間值換算成公分單位
}

void Pumping_Down_Counter()
{
    if( duration_relay_water>0 ) duration_relay_water-=1;
    else { flag_relay_water=false;duration_relay_water=0; }

    if( duration_relay_fertilizer>0 ) duration_relay_fertilizer-=1;
    else { flag_relay_fertilizer=false;duration_relay_fertilizer=0; }
}

void Mix_PRO()
{
    /* 水位判斷基準 */
    bool isUnderHigh = MIX_WATER_STAGE_H < ultraVal;
    bool isOverLow = ultraVal <= MIX_WATER_STAGE_L;
    bool isPumping = isUnderHigh && isOverLow;

    /* 濃度判斷基準 */
    bool isPumpingWater = goal_ec != -1 && goal_ec < ecVal;
    bool isPumpingFertilizer = goal_ec != -1 && !isPumpingWater;

    if(isPumping && isPumpingWater) { flag_relay_water=true; duration_relay_water=10; }
    else if(isPumping && isPumpingFertilizer) { flag_relay_fertilizer=true; duration_relay_fertilizer=10; }
}

void ConnectWithMaster()
{
    if(conn.available()>1)
    {
        conn.read(); // 水位?
        goal_ec = conn.read(); //目標濃度
    }
}

void setup()
{
    /* 串列阜 */
    Serial.begin(ARDUINO_BAUD);
    conn.begin(ARDUINO_BAUD);

    /* 濃度感測 */
    ec.begin();

    /* 超音波感測 */
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    /* 繼電器控制 */
    pinMode(RELAY_WATER, OUTPUT);
    pinMode(RELAY_FERTILIZER, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    pinMode(RELAY_4, OUTPUT);

    /* 初始時間 */
    time_twoH_ms=time_fivH_ms
      =time_one_sec=time_three_sec
      =time_five_sec=time_ten_sec=millis();
}

void loop()
{
    Mix_PRO();
    Relay_PRO();
    ConnectWithMaster();
    Serial.println(String("濃度: ")+ecVal+"\t距離: "+ultraVal);

    /* 需計時--------------------------------------------------------------------------------------------- */
    if( millis() - time_twoH_ms >= 200U )
    {
        // 間隔200ms執行
        
        time_twoH_ms = millis();
    }

    if( millis() - time_fivH_ms >= 500U )
    {
        // 間隔500ms執行
        Pumping_Down_Counter();
        time_fivH_ms = millis();
    }
    
    if( millis() - time_one_sec >= 1000U )
    {
        // 間隔1s執行
        EC_PRO();
        Ultra_PRO();
        time_one_sec = millis();
    }

    if( millis() - time_three_sec >= 3000U )
    {
        // 間隔3s執行
        
        time_three_sec = millis();
    }

    if( millis() - time_five_sec >= 5000U )
    {
        // 間隔5s執行
        
        time_five_sec = millis();
    }

    if( millis() - time_ten_sec >= 10000U )
    {
        // 間隔10s執行
        
        time_ten_sec = millis();
    }
}