// Format of receivESPAsyncWebServered packet: command[6]={Red_Intensity/1023,Green_Intensity/1023,Blue_Intensity/1023,Blink_Duration/ms,Blink_Interval/ms,h,m,s,h-begin,m-begin,h-end,m-end};
// 接收数据包格式: command[6]={红色亮度/1023,绿色亮度/1023,蓝色亮度/1023,闪烁时长/毫秒,间隔时长/毫秒,h,m,s,h-begin,m-begin,h-end,m-end}
#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include "SPIFFS.h"
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <FS.h>   
#endif
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <RF24.h>
//#include <TimeLib.h>

uint8_t data[14];
uint8_t data1[14];      

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 //单位秒
#define TIME_SLEEP 60    //单位分钟，每次休眠1小时
float voltage=0;
#ifdef ESP32
#define rouge 25
#define verde 26
#define azure 27
#else
#define rouge 2
#define verde 4
#define azure 5

#define control 15
#endif

// Singleton instance of the radio driver

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = 
int sensorValue = 0;  // value read from the pot
float temp=0;
int red = 0; //red value 0-1023
int blue = 0; //blue
int yellow = 0; //yellow
int duration; //blink duration
int interval; //blink interval
int fois; //downlink timestamp to distinguish commands
int tmpBuf = 0; //a buffer variable to differentiate between commands
int ComHor;
int ComMin;
int ArsHor;
int ArsMin;
int Ahour;
int Aminute;
int Asecond;
int bufA = -2;
int FFmount;
int bian;
int a, b, c, d, e, f;//ID
int a1,b1,c1,d1,e1,f1;//sleep time
long t = 0;
bool state = 0;
long t_v = 0;

#ifdef ESP32
RH_NRF24 nrf24(2, 4); //CE,CSN
#else
RH_NRF24 nrf24(0, 15); //CE,CSN//原来CE接16
#endif
void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);
#ifdef ESP32
//btStop();
#else

#endif

  pinMode(rouge, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azure, OUTPUT);
  
  //pinMode(control, OUTPUT);
  //-SPIFFS**********************
  File myFile;
  String file_str;//从文件读取的数据转成String类型
  String id, id1, id2, id3, id4, id5;
  int firOfColon;//第一个冒号的位置

  Serial.begin(115200);
  Serial.println();
  if (!SPIFFS.begin()) {
    Serial.println("Start SPIFFS Failed!please check Arduino Download Config.");
    return;
  }
  Serial.println("");
  Serial.println("File System Initialized");
  myFile = SPIFFS.open("/data.txt", "r");//文件名前的反斜杠不能少
  if (myFile) {
    Serial.println("Reading data.txt...");
    while (myFile.available()) {
      //读取文件输出
      file_str += (char)myFile.read(); //循环读取文件转换成String类型
      //Serial.write(myFile.read());//循环文件输出到串口
    }
    id = file_str;
	id.trim();
	int channel = id.toInt();
    Serial.print("ID=");
    Serial.println(id);//串口输出已转换成String类型的文件内容
    //自动清除字符串中最前面和最后面的空白数据。不做清除字符串会包含有'\n'

    myFile.close();
	
	if (!nrf24.init())
    Serial.println("init failed");

	if (!nrf24.setChannel(channel))
	Serial.println("setChannel failed");
	if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
	Serial.println("setRF failed");
  } else {
    Serial.println("Open File Failed.");
  }
  //-*******************************spiffs end***********
}


void loop()
{

// ************************************************************电池电压检测结束。
  if (nrf24.available())
  {
  
    uint8_t buf[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.print("receive:" + buf[8] + buf[9] + buf[10] + buf[11] + buf[12]);
      //      NRF24::printBuffer("request: ", buf, len);
      red = buf[0];
      blue = buf[1];
      yellow = buf[2];
      duration = buf[3]; //refresh frequent
      interval = buf[4]; //refresh frequent
      fois = buf[5];
      Ahour = buf[6];
      Aminute = buf[7];
      Asecond = buf[8];
      ComHor = buf[9];
      ComMin = buf[10];
      ArsHor = buf[11];
      ArsMin = buf[12];
      FFmount = buf[13];
      //上报自己是否正常程序，下面是检查是否收到 {14,13,12,11,10,9,8,7,6,5,4,3,2,1}，如果最后一个值是1，说明基站要求上报自己是否正常。下面是程序段，收到这个值就发送2。

      if ((FFmount == 1) && (ArsMin == e) && (ArsHor == d) && (ComMin == c) && (ComHor == b) && (Asecond == a))
      {

        //循环开始
        for (int h = 0; h < 5; h++)
        {

          //      uint8_t data1[14];
          //= {2, 2, 2, 2, 2, 2, 2, 2, a, b, c, d, e, 2};

          data1[0] = 2;
          data1[1] = 2;
          data1[2] = 2;
          data1[3] = 2;
          data1[4] = 2;
          data1[5] = 2;
          data1[6] = 2;
          data1[7] = 2;
          data1[8] = a;
          data1[9] = b;
          data1[10] = c;
          data1[11] = d;
          data1[12] = e;
          data1[13] = 2;

          //delay(100);
          nrf24.send(data1, sizeof(data1));
          nrf24.waitPacketSent();
          //delay(20);
        }

        Serial.print("send");

        for (int h = 8; h < 13; h++)
        {
          Serial.print(data1[h]);
          //   Serial.print(",");
        }

        //循环结束
        Serial.println(" OK!");
        FFmount = 0;
        buf[13] = 0;
      }  
        //开灯
     if (yellow == 1)
     {
       Serial.print("on light");
     }
     if (yellow == 0)
     {
       Serial.print("off light");    
     }
    if(yellow == 1)
    digitalWrite(azure, 1);
    //关灯
    if(yellow == 0)
    digitalWrite(azure, 0);   

	}
	else
	{
	  Serial.println("recv failed");
	}
  }
  

}
 
