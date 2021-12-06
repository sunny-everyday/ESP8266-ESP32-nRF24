
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>   

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

#define rouge 2
#define verde 4
#define azure 5


// Singleton instance of the radio driver

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = 
int sensorValue = 0;  // value read from the pot
float temp=0;
int red = 0; //red value 0-1023
int yel = 0; //yellow
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
int sleepH;
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


  pinMode(rouge, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azure, OUTPUT);

  Serial.println("red");
  digitalWrite(rouge, 1);
  digitalWrite(verde, 0);
  digitalWrite(azure, 0);
  delay(2000);

  Serial.println("yell");
  digitalWrite(rouge, 0);
  digitalWrite(verde, 1);
  digitalWrite(azure, 0);
  delay(2000);
  
  Serial.println("blue");
  digitalWrite(rouge, 0);
  digitalWrite(verde, 0);
  digitalWrite(azure, 1);
  delay(2000);
  
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(100))
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

int firstL = 0;
int secondL = 0;
int thirdL = 0;
void loop()
{
	 
  // ************************************************************电池电压检测结束。
  if (nrf24.available())
  {
  
    uint8_t buf[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.println(String("receive:") + buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5]);
      //      NRF24::printBuffer("request: ", buf, len);
      red = buf[0];
      yel = buf[1];
      blu = buf[2];
      duration = buf[3]; //refresh frequent
      interval = buf[4]; //refresh frequent
      fois = buf[5];
      sleepH = buf[12];
      FFmount = buf[13];
      //上报自己是否正常程序，下面是检查是否收到 {14,13,12,11,10,9,8,7,6,5,4,3,2,1}，如果最后一个值是1，说明基站要求上报自己是否正常。下面是程序段，收到这个值就发送2。
		
	  if (FFmount == 2)  //set
	  {
		if(red + yel + blu == 6 && duration == interval && duration == 0)
		{
			if(red == 1)
				firstL = rouge;
			else if (yel == 1)
				firstL = verde;
			else  if(blu == 1)
				firstL = azure;
			else
			{}
		
		
			if(red == 2)
				secondL = rouge;
			else if (yel == 2)
				secondL = verde;
			else  if(blu == 2)
				secondL = azure;
			else
			{}
		
			if(red == 3)
				thirdL = rouge;
			else if (yel == 3)
				thirdL = verde;
			else  if(blu == 3)
				thirdL = azure;
			else
			{}
			
		}
	  }
      if (FFmount == 1)  //check
      {
        //循环开始
        for (int h = 0; h < 5; h++)
        {

          data1[8] = a;
          data1[9] = b;
          data1[10] = c;
          data1[11] = d;
          data1[12] = e;
          data1[13] = 2;

          //delay(100);
          //nrf24.send(data1, sizeof(data1));
          //nrf24.waitPacketSent();
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
      
		else if (FFmount == 9) //休眠。。。
		{
		  FFmount = 0;
		  buf[13] = 0;
		 
		  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
		  Serial.println("Going to sleep now");
		  delay(1000);
		  ESP.deepSleep(30e7);  //300s 5分钟
		  Serial.flush();

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

  if(duration == 0 && interval == 0 && red + yel + blu != 0)
  {
	if (state == 0) {
	digitalWrite(firstL, 1);
	digitalWrite(secondL, 0);
	digitalWrite(thirdL, 0);
	if ((millis() - t) >= (10 * fois)) {
		  state = 1;
		  t = millis();
		}
	}
	if (state == 1) {
	digitalWrite(firstL, 0);
	digitalWrite(secondL, 1);
	digitalWrite(thirdL, 0);
	if ((millis() - t) >= (10 * fois)) {
		  state = 2;
		  t = millis();
		}
	}
	if (state == 2) {
	digitalWrite(firstL, 0);
	digitalWrite(secondL, 0);
	digitalWrite(thirdL, 1);
	if ((millis() - t) >= (10 * fois)) {
		  state = 0;
		  t = millis();
		}
	}
	  
  }
  else
  {
	if (state == 1) {
	digitalWrite(rouge, red);
	digitalWrite(verde, yel);
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
}
