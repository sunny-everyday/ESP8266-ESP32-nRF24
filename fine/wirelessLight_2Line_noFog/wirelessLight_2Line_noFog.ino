
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

#define A2OUT 2
#define A1OUT 5


// Singleton instance of the radio driver

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = 
int sensorValue = 0;  // value read from the pot
float temp=0;
int red = 0; //red value 0-1023
int yel = 0; //yellow
int blu = 0; //blue
int duration; //blink duration
int interval; //blink interval
int tmpBuf = 0; //a buffer variable to differentiate between commands
int ComHor;
int ComMin;
int ArsHor;
int ArsMin;
int Ahour;
int Aminute;
int Asecond;
int bufA = -2;
int FFmount;//命令码
int SynNum = 0;
int bian;
int sleepH;
int a, b, c, d, e, f;//ID
int a1,b1,c1,d1,e1,f1;//sleep time
long t = 0;
long t1 = 0;
long t2 = 0;
long t3 = 0;
bool state = 0;
long t_v = 0;
int A2,A1;


RH_NRF24 nrf24(0, 4); //CE,CSN//原来CE接16


void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  pinMode(A2OUT, OUTPUT);
  pinMode(A1OUT, OUTPUT);
  TurnRed();
  delay(2000);
  TurnYell();
  delay(2000);
  TurnBlue();
  delay(2000);
  offLight();
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

    a = id.substring(0, 1).toInt();
    b = id.substring(1, 2).toInt();
    c = id.substring(2, 3).toInt();
    d = id.substring(3, 4).toInt();
    e = id.substring(4, 5).toInt();
    myFile.close();
  } else {
    Serial.println("Open File Failed.");
  }
  //-*******************************spiffs end***********
}

int firstL = 0;  //1 红色 2黄色 4蓝色 0 灭
int secondL = 0;
int thirdL = 0;
int fois = 0;
bool receivetureinfo = false;
int receivesynNum = 0;
int lastsynInfo  = 0;
void loop()
{

  // ************************************************************电池电压检测结束。
  if (nrf24.available())
  {
  
    uint8_t buf[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      Serial.println(String("receive:") + buf[0] + buf[1] + buf[2] + buf[3]+ buf[4] +  buf[5]+  buf[6] + buf[7] + buf[13]);
      //      NRF24::printBuffer("request: ", buf, len);
      red = buf[0];
      yel = buf[1];
      blu = buf[2];
      duration = buf[3]; //refresh frequent
      interval = buf[4]; //refresh frequent
      fois = buf[5];   //周期
      sleepH = buf[6];
	  
	  if(buf[7] == 2 || buf[8] == 2 || buf[9] == 2 || buf[10] == 2||  buf[11] == 2 || buf[12] == 2)
	  {FFmount = 2;}  
      
	  if(buf[7] == 1 || buf[8] == 1 || buf[9] == 1 || buf[10] == 1||  buf[11] == 1 || buf[12] == 1)
	  {FFmount = 1;} 
  
	  if(buf[7] == 9 || buf[8] == 9 || buf[9] == 9 || buf[10] == 9||  buf[11] == 9 || buf[12] == 9)
	  {FFmount = 9;} 
  
  	  if(buf[7] == 8 || buf[8] == 8 || buf[9] == 8 || buf[10] == 8||  buf[11] == 8 || buf[12] == 8)
	  {FFmount = 8;} 
  
	  SynNum = buf[13];
      //上报自己是否正常程序，下面是检查是否收到 {14,13,12,11,10,9,8,7,6,5,4,3,2,1}，如果最后一个值是1，说明基站要求上报自己是否正常。下面是程序段，收到这个值就发送2。
	  //非雾区没有三色灯
			
		if (FFmount == 2)  //单色灯
		{
		  if(red == 1)
			{firstL = 1;secondL =0;thirdL=0;Serial.println("red");TurnRed();}
		  if(yel == 1)
			{firstL = 2;secondL =0;thirdL=0;Serial.println("yell");TurnYell();}
		  if(blu == 1)
			{firstL = 4;secondL =0;thirdL=0;Serial.println("blue");TurnBlue();}
		  if(red == 0 && yel == 0 && blu == 0)
			{firstL = 0;secondL =0;thirdL=0;Serial.println("offLight");offLight() ;} 
		}
		if (FFmount == 1)  //check
		{
			if(red ==a &&  yel == b && blu == c && duration == d && interval == e)
			{
				//循环开始
				for (int h = 0; h < 2; h++)
				{

				  data1[0] = a;
				  data1[1] = b;
				  data1[2] = c;
				  data1[3] = d;
				  data1[4] = e;
				  
				  data1[5] = data1[6] = data1[7] = data1[8] = data1[9] = data1[10] = data1[11] = data1[12] = 5;
				  nrf24.send(data1, sizeof(data1));
				  nrf24.waitPacketSent();
				  delay(200);
				}

				Serial.print("send");
				for (int h = 0; h < 13; h++)
				{
				  Serial.print(data1[h]);
				  Serial.print(",");
				}

				//循环结束
				Serial.println(" OK!");
				FFmount = 0;

			}
		}
			  
		if (FFmount == 9) //休眠。。。
		{
		  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
		  Serial.println("Going to sleep now");
		  delay(1000);
		  ESP.deepSleep(60e6 * 60 * sleepH);  //1s * 60 * H
		  Serial.flush();

		  Serial.println("This will never be printed");
		  // delay(e*(duration+interval));
		  // Serial.println("休眠 OK...");
		} //end 休眠

		if (FFmount == 8) //重启。。。
		{
			Serial.println("Restarting in 2 seconds");
			delay(2000);
			ESP.restart();
		} //end 重启结束

  	}
  	else
  	{
  	  Serial.println("recv failed");
  	}

  }

}

void TurnRed()
{
  //Serial.println("red");
  A2 = 0; A1=0;
  digitalWrite(A2OUT, A2);
  digitalWrite(A1OUT, A1);
}//红灯
void TurnYell()
{
  //Serial.println("yel");
  A2 = 1; A1=1;
  digitalWrite(A2OUT, A2);
  digitalWrite(A1OUT, A1);
}//黄灯
void TurnBlue()
{
  //Serial.println("blue");
  A2 = 0; A1=1;
  digitalWrite(A2OUT, A2);
  digitalWrite(A1OUT, A1);
}//蓝灯
void offLight() //灭灯
{
  A2 = 1; A1=0;
  digitalWrite(A2OUT, A2);
  digitalWrite(A1OUT, A1);
}
