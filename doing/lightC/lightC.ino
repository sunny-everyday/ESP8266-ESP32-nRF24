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

#define rouge 2
#define verde 4
#define azure 5
#define control 5

// Singleton instance of the radio driver

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = 
int sensorValue = 0;  // value read from the pot
float temp=0;
int red = 0; //red value 0-1023
int grn = 0; //green
int blu = 0; //blue
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

bool ONsendok = false;
bool OFFsendok = false;

#ifdef ESP32
RH_NRF24 nrf24(2, 4); //CE,CSN
#else
RH_NRF24 nrf24(2, 4); //CE,CSN//0,15
#endif
void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  pinMode(control, INPUT);
  

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
  Serial.println(channel);//串口输出已转换成String类型的文件内容

    myFile.close();
	
	if (!nrf24.init())
	  Serial.println("init failed");
	//read channel from configfile
	if (!nrf24.setChannel(channel))
		Serial.println("setChannel failed");
	if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
		Serial.println("setRF failed");
  Serial.println("nrf24 init OK");
  } else {
    Serial.println("Open File Failed.");
  }
  //-*******************************spiffs end***********
}


void loop()
{

 
  int val = digitalRead(control); 
	if(val == HIGH && ONsendok == false)
	{
		Serial.print("get the signal from 8266S \r\n");
		//if (nrf24.available())
    for(int h=0;h<2;h++){
  		uint8_t data[14] ={0,0,1,200,0,0,6,0,0,0,0,25,60,0} ;
  		nrf24.send(data, sizeof(data));
  		nrf24.waitPacketSent();
  		Serial.print("send the signal.");
      delay(1);
    }
		ONsendok = true;
    OFFsendok = false;
	}
  if(val == LOW && OFFsendok == false)
	{
      for(int h=0;h<2;h++){
      uint8_t data[14] ={0,0,0,200,0,0,6,0,0,0,0,25,60,0} ;
  		nrf24.send(data, sizeof(data));
  		nrf24.waitPacketSent();
  		Serial.print("send the signal.");
      delay(1);
      }
      OFFsendok = true;
      ONsendok = false;
    }
	

}
 
