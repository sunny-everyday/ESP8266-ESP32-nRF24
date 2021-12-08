
#include <SPI.h>

#define DEFAULT_CHANNEL 200 // 默认通道
#define INTERVAL_TIME 5000  // 发送亮灯信号间隔时间
#define LIGHT_TIME 6000  // 亮灯时长
#define FREQUENCE 115200    // 波特率

#define DELAY_TIME 500 // 延迟解决信号干扰

#define CE_PIN 2
#define CSN_PIN 4

#define DOPP 33


#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin
#define UPTIME 6000 //开机时间ms,6秒

long tempus = 0;        // 用来保存程序启动至此刻的时间，作为雷达传递信号周期的依据
long index_counter = 0; // 用来确认信号丢失情况
bool etat =0; 
bool openstate = false;
void setup()
{
  initial(); // 设备与程序初始化
}

// 初始化整个开发板
void initial()
{
  //pinMode(DOPP, OUTPUT);
  Serial.begin(FREQUENCE);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   // set the data rate
}

void loop()
{
    delay(1000);
    if( openstate == false){
        Serial.println("Here car coming，start alarming");
        tempus=millis();
        {openLight();openstate = true;}
		    
    }
    else
    {
      if(((millis()-tempus)>UPTIME) && openstate == true){
        closeLight();
        openstate = false;
        Serial.println("stop alarming");
      }
    }
    
}

void openLight() {
 Serial2.print("open-light-and-alarm");
 Serial.println("open-light-and-alarm");
}

void closeLight() {
 Serial2.print("clse-light-and-alarm");
 Serial.println("clse-light-and-alarm");
}
