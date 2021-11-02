#include <WiFi.h>

#include <ESPmDNS.h>

//#include <WebServer.h>

#include <WiFiMulti.h>

#include <ESPAsyncWebServer.h>

#include <RH_NRF24.h>

#include "SPIFFS.h"

const char* AP_SSID  = "ESP32_Config"; //热点名称

const char* AP_PASS  = "12345678";  //密码


WiFiServer server(80);

WiFiMulti wifiMulti;


uint8_t resr_count_down = 120;//重启倒计时s

TimerHandle_t xTimer_rest;

void restCallback(TimerHandle_t xTimer );

const char* input1;
const char* input0;
String header;
RH_NRF24 nrf24(2,4);//CE,CSN
int red;  //27
int blue; //26
int yellow;  //25
int Tempmant;
const int dur=2;
const int itv=2;

int i=1;
int j=0;
String buf = "";
int len;
int buf8,buf9,buf10,buf11,buf12;
String bufx = "";
int id20=200;
int id21=200;
File myFile;
String file_str;//从文件读取的数据转成String类型
int x=0;
int firOfColon,secOfColon;//第一个符号的位置,第二个符号的位置
String  m_sn,msn1,msn2,msn3,msn4,msn5;
int m1,m2,m3,m4,m5;
uint8_t data[14];

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
String output25State = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define CHANNEL_MIN    1

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin

#define SERIAL_COMMUNICATION_CONTROL_PIN 5 // Transmission set pin
#define RS485_TX_PIN_VALUE HIGH
#define RS485_RX_PIN_VALUE LOW

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);//配置为AP模式

  boolean result = WiFi.softAP(AP_SSID, AP_PASS);//开启WIFI热点

  if (result)
  {

	IPAddress myIP = WiFi.softAPIP();

	//打印相关信息

	Serial.println("");

	Serial.print("Soft-AP IP address = ");

	Serial.println(myIP);

	Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());

	Serial.println("waiting ...");


	xTimer_rest = xTimerCreate("xTimer_rest", 1000 / portTICK_PERIOD_MS, pdTRUE, ( void * ) 0, 	restCallback);

	xTimerStart( xTimer_rest, 0 );  //开启定时器

	} 
	else 
	{  //开启热点失败

		Serial.println("WiFiAP Failed");

		delay(3000);

		ESP.restart();  //复位esp32

	}


	if (MDNS.begin("esp32")) {

		Serial.println("MDNS responder started");

	}

	server.begin();

	Serial.begin(115200);

	// Initialize the output variables as outputs


	if (!nrf24.init())
	Serial.println("init failed");
	// Defaults after init are 2.402 GHz (channel 2)
	if (!nrf24.setChannel(CHANNEL_MIN))
	Serial.println("setChannel failed");
	//if (!nrf24.setRF(RH_NRF24::DataRate250Kbps, RH_NRF24::TransmitPower0dBm))
	if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
	Serial.println("setRF failed");

	pinMode(SERIAL_COMMUNICATION_CONTROL_PIN, OUTPUT);
	digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE);

	Serial2.begin(9600);//, SERIAL_8N1, RXD2, TXD2);   // set the data rate

	delay(500);

}
int i_Ontime = 0;
int i_Offtime = 0;
int  i_Tras = 0;
void loop()
{
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
 //   Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)    
    {  // loop while the client's connected
		currentTime = millis();
		if (client.available()) 
		{             // if there's bytes to read from the client,
	  
			char c = client.read();             // read a byte, then
			//   Serial.write(c);                    // print it out the serial monitor
			header += c;
			String input1=header;
			if (c == '\n') {                    // if the byte is a newline character
 		      //192.168.12.1/o=y=200s=200e=  192.168.12.1/o=rybw=200s=   192.168.12.1/f= 192.168.12.1/o=reset  192.168.12.1/o=sleep  192.168.12.1/o=check
				if (currentLine.length() == 0) 
				{

					if (header.indexOf("f=") > 0){
						red = yellow = blue = 0;
						//开关时间 和 错灯间隔
						for(int h=0;h<1;h++){
							uint8_t data[14] ={red,yellow,blue,0,0,0,0,0,0,0,0,0,0,2} ;
							nrf24.send(data, sizeof(data));
							nrf24.waitPacketSent();
							sendDatato433(data);
						}
					}
					else if (header.indexOf("o=") > 0){ 
						int colorbegin=0;int sbegin=0;int ebegin=0;int wbegin=0;	
						sbegin = header.indexOf("s=");
						ebegin = header.indexOf("e=");
						
						if(header.indexOf("y=") > 0 && sbegin > 0 && ebegin > 0){
							Serial.println("yellow light.");
							colorbegin = header.indexOf("y=");
							yellow = 1;
							red = blue = 0;
						}
						else if (header.indexOf("b=") > 0 && sbegin > 0 && ebegin > 0){
							Serial.println("blue light.");
							colorbegin = header.indexOf("b=");
							blue = 1;
							red = yellow = 0;
						}
						else if (header.indexOf("r=") > 0 && sbegin > 0 && ebegin > 0){
							Serial.println("red light.");
							colorbegin = header.indexOf("r=");
							red = 1;
							yellow = blue = 0;
						}
						else if (header.indexOf("w=") > 0 && sbegin > 0 && ebegin > 0 &&  header.indexOf("s=")> 0){
							Serial.println("w light.");
							colorbegin = wbegin = header.indexOf("w=");
							sbegin  = header.indexOf("s=");
              if(header.indexOf("ryb") > 0)
              {
                red = 1;
                yellow = 2;
                blue = 3;   //固定红黄蓝顺序
              }
              if(header.indexOf("rby") > 0)
              {
                red = 1;
                yellow = 3;
                blue = 2;   //固定红黄蓝顺序
              }
              if(header.indexOf("bry") > 0)
              {
                red = 2;
                yellow = 3;
                blue = 1;   //固定红黄蓝顺序
              }
              if(header.indexOf("byr") > 0)
              {
                red = 3;
                yellow = 2;
                blue = 1;   //固定红黄蓝顺序
              }
              if(header.indexOf("ybr") > 0)
              {
                red = 3;
                yellow = 1;
                blue = 2;   //固定红黄蓝顺序
              }
              if(header.indexOf("yrb") > 0)
              {
                red = 2;
                yellow = 1;
                blue = 3;   //固定红黄蓝顺序
              }
						}
						else{
							Serial.println("the comand is reset or sleep or check."); 
							if(header.indexOf("reset") >= 0){//重启
                for(int h=0;h<1;h++){
                  uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,0,8} ;//buf[13]=8是报警灯重启命令
                  nrf24.send(data, sizeof(data));
                  nrf24.waitPacketSent();
                  sendDatato433(data);
                }
             }
             if (header.indexOf("check") >= 0) {
              for(int h=0;h<1;h++){
                uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,0,1} ;//buf[13]=1是报警灯点名命令
                nrf24.send(data, sizeof(data));
                nrf24.waitPacketSent();
                sendDatato433(data);
              }
          }
            if (header.indexOf("sleep") >= 0) {
              for(int h=0;h<1;h++){
                uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,0,9} ;//buf[13]=9是报警灯休眠命令
                nrf24.send(data, sizeof(data));
                nrf24.waitPacketSent();
                sendDatato433(data);
              }
              }
              break;
						}
						if(colorbegin  > 0 ){
							String Ontime = header.substring(colorbegin+2, sbegin);
							i_Ontime = Ontime.toInt();
							String Offtime = header.substring(sbegin+2, ebegin);
							i_Offtime = Offtime.toInt();
							Serial.println(String("ontime = ") + i_Ontime + String(",offtime = ") + i_Offtime );
						}
						if(wbegin > 0 ){
							String Trastime = header.substring(colorbegin+2, sbegin);
							i_Tras = Trastime.toInt();
							i_Ontime = i_Offtime = 0;
							Serial.println(String("Trastime = ") + i_Tras);
							Serial.println(String("wbegin = ") + wbegin);
						}
						//开关时间 和 错灯间隔
						for(int h=0;h<1;h++){
							uint8_t data[14] ={red,yellow,blue,i_Ontime/10,i_Offtime/10,i_Tras/10,0,0,0,0,0,0,0,2} ;
							nrf24.send(data, sizeof(data));
							nrf24.waitPacketSent();
							sendDatato433(data);
						}
					}
					else { //选择3
						Serial.println("no command can handle error");
					}//选择3
            
          // Break out of the while loop
          break;
				} 
				else 
				  
				{ // if you got a newline, then clear currentLine
					currentLine = "";
				}
			}
			else if (c != '\r') {  
			// if you got anything else but a carriage return character,
			  currentLine += c;      // add it to the end of the currentLine
			}

		}

	}
    // Clear the header variable
    
    header = "";
    // Close the connection
    client.stop();
  //  Serial.println("Client disconnected.");

  }
 

}

void sendDatato433(uint8_t *data){
	Serial2.write(0x99); // Send message
	Serial2.write(0x88); // Send message
	Serial2.write(0x77); // Send message
	Serial2.write(0x66); // Send message
	Serial2.write(0x55); // Send message
	for(int h=0;h<14;h++)
	{
		Serial2.write(data[h]); // Send message
    Serial.println(data[h]);
		//delay(10);	
	}

}

void restCallback(TimerHandle_t xTimer ) {  //长时间不访问WIFI Config 将复位设备

  resr_count_down --;

  //Serial.print("resr_count_down: ");

  //Serial.println(resr_count_down);

  if (resr_count_down < 1) {

	//ESP.restart();
	resr_count_down = 120;
	Serial.print("2 mins");

  }

}
