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
#include <.h>
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
#endif

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
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(50))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");



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
    Serial.print("ID=");
    Serial.println(id);//串口输出已转换成String类型的文件内容
    id.trim();//自动清除字符串中最前面和最后面的空白数据。不做清除字符串会包含有'\n'

    id1 = id.substring(id.length() - 5, id.length() - 4);
    id2 = id.substring(id.length() - 4, id.length() - 3);
    id3 = id.substring(id.length() - 3, id.length() - 2);
    id4 = id.substring(id.length() - 2, id.length() - 1);
    id5 = id.substring(firOfColon + 4, '\r');

    a = id1.toInt();
    b = id2.toInt();
    c = id3.toInt();
    d = id4.toInt();
    e = id5.toInt();
    myFile.close();
  } else {
    Serial.println("Open File Failed.");
  }
  //-*******************************spiffs end***********
}


void loop()
{
		//  *************************************************下面为电池电压测量，低于某值就休眠一小时，然后重启。
		// read the analog in value
		//      t_v = millis();
	 if((millis()-t_v)>=10000)//10秒该时间间隔输出一次电压值
	 {
	  sensorValue = analogRead(analogInPin);
	   temp = 4.2*(sensorValue*(5.5675)/4095);
		//  Serial.print("sensor = ");
		//  Serial.print(sensorValue);  
	  Serial.print("Battery Volt = ");
	  Serial.print(temp);
	    Serial.println("V");
	  t_v = millis();
	  if (temp < voltage){    //小于变量Voltage的值（初定3.4V）就让其休眠，每次休眠1小时
	    digitalWrite(rouge, 0);
	    digitalWrite(verde, 0);
	    digitalWrite(azure, 0);
	 #ifdef ESP32
	  Serial.println("Setup ESP32 to sleep for  " + String(TIME_SLEEP) +  " minutes");
	 Serial.println("Going to sleep now");
	  esp_sleep_enable_timer_wakeup(TIME_SLEEP * uS_TO_S_FACTOR * 60);
	  delay(1000);
	  Serial.flush(); 
	  esp_deep_sleep_start();
	  Serial.println("This will never be printed");
	#else
	  Serial.println("Setup ESP8266 to sleep for " + String(TIME_SLEEP) +  " minutes");
	  Serial.println("Going to sleep now");
	  ESP.deepSleep(TIME_SLEEP * uS_TO_S_FACTOR * 60); 
	  Serial.println("This will never be printed");
	#endif
	 }
	}
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
      grn = buf[1];
      blu = buf[2];
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
      
      else if (FFmount == 5)
	    { //滚动闪烁。。。
	      FFmount = 0;
	      buf[13] = 0;
	      delay(e * (duration + interval));
	    } //end 滚动闪烁

	    else if (FFmount == 9) //休眠。。。
	    {
	      FFmount = 0;
	      buf[13] = 0;
	#ifdef ESP32
	      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
	#endif
	      Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
	      Serial.println("Going to sleep now");
	      delay(1000);
	      Serial.flush();
	#ifdef ESP32
	      esp_deep_sleep_start();
	#endif
	      Serial.println("This will never be printed");
	      // delay(e*(duration+interval));
	      // Serial.println("休眠 OK...");
	    } //end 休眠

	    else if (FFmount == 8) //重启。。。
	    {
	      FFmount = 0;
	      buf[13] = 0;
	      digitalWrite(rouge, 0);
	      digitalWrite(verde, 0);
	      digitalWrite(azure, 0);
	      Serial.println("Restarting in 2 seconds");
	      delay(2000);
	      ESP.restart();
	      //   Serial.println("重启 OK...");
	    } //end 重启结束

	    }
	    else
	    {
	      Serial.println("recv failed");
	    }

    
  }
 

   rayon();

}
 
void rayon() {
  if (state == 1) {
    digitalWrite(rouge, red);
    digitalWrite(verde, grn);
    digitalWrite(azure, blu);
    if ((millis() - t) >= (10 * duration)) {
      state = 0;
      t = millis();
    }
  }
  if (state == 0) {
    digitalWrite(rouge, 0);
    digitalWrite(verde, 0);
    digitalWrite(azure, 0);
    if ((millis() - t) >= (10 * interval)) {
      state = 1;
      t = millis();
    }
  }

}
