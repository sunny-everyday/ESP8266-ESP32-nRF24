#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>

#include <SPI.h>
#include <RH_NRF24.h>
#include "SPIFFS.h"
#include <FS.h>   //Include File System Headers

const char* input1;
const char* input0;
// Set web server port number to 80
WiFiServer server(80);

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

String ssid,password;
String m_ssid, m_sn;//从文件数据中提取的WIFI热点名称和密码
int firOfColon,secOfColon;//第一个符号的位置,第二个符号的位置
int m1,m2,m3,m4,m5;
uint8_t data[14];

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define CHANNEL_MIN    100

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin
#define SERIAL_COMMUNICATION_CONTROL_PIN 5 // Transmission set pin
#define RS485_TX_PIN_VALUE HIGH
#define RS485_RX_PIN_VALUE LOW

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 2);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional



void setup() {
 
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
  // Connect to Wi-Fi network with SSID and password

  pinMode(SERIAL_COMMUNICATION_CONTROL_PIN, OUTPUT);
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE);

  Serial2.begin(9600);//, SERIAL_8N1, RXD2, TXD2);   // set the data rate
  //read wifiname and password:
  File myFile;

  

  if (!SPIFFS.begin()) {
    Serial.println("Start SPIFFS Failed!please check Arduino Download Config.");
    return;
  }
  Serial.println("File System Initialized");
  myFile = SPIFFS.open("/IP.txt", "r");//文件名前的反斜杠不能少
  if (myFile) {
    Serial.println("Reading IP.txt...");
    while (myFile.available()) {
      //读取文件输出
      file_str += (char)myFile.read(); //循环读取文件转换成String类型
     //Serial.write(myFile.read());//循环文件输出到串口
     }
     myFile.close();
//    Serial.println(file_str);//串口输出已转换成String类型的文件内容
    firOfColon = file_str.indexOf(':');//从字符串开始位置查找":"位置
    secOfColon = file_str.lastIndexOf(':');//从字符串尾部查找":"位置
 
  int  posOfCr = file_str.indexOf('\r');

    ssid = file_str.substring(firOfColon + 1, posOfCr);
    ssid.trim();//自动清除字符串中最前面和最后面的空白数据。不做清除字符串会包含有'\n'
    Serial.println(ssid);
    password = file_str.substring(secOfColon + 1);
    password.trim();//保险起见，自动清除字符串中最前面和最后面的空白数据。
    Serial.println(password);
  //  myFile.close();
   delay(10);
  } else {
    Serial.println("Open File Failed.");
  }
 


    // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  const char* c1 = ssid.c_str();
  const char* c2 = password.c_str();
  Serial.print("Connecting to ");
  Serial.print(c1);   
  Serial.print(",");  
  Serial.println(c2);
  
   WiFi.begin(c1,c2);

  String hostname = "ESP32server";
//WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
WiFi.setHostname(hostname.c_str()); //define hostname
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");Serial.println(WiFi.macAddress());
  server.begin();
//*************************************
 if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }


   File file = SPIFFS.open("/data.txt");
  if(!file){
    Serial.println("data.txt Failed to open file for reading");
    return;
  }
  
  Serial.println("data.txt File Content:");
  while(file.available()){
    
   Serial.write(file.read());

  }
  file.close();

//
  
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
							uint8_t data[14] = {red,yellow,blue,0,0,0,0,0,0,0,0,0,2,0} ;
							sendDatato433(data);
							delay(2000);
							sendDatato2G4(data);
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
							  if(wbegin > 0 ){
								String Trastime = header.substring(colorbegin+2, sbegin);
								i_Tras = Trastime.toInt();
								i_Ontime = i_Offtime = 0;
								Serial.println(String("Trastime = ") + i_Tras);
								Serial.println(String("wbegin = ") + wbegin);
							}

							for(int h=0;h<1;h++){
								  uint8_t data[14] ={red,yellow,blue,0,0,i_Tras,0,0,0,0,0,0,3,0} ;//buf[13]=3是三色灯命令
								  sendDatato433(data);
								  delay(2000);
								  sendDatato2G4(data);
							}
							break;
						}
						else
						{
							Serial.println("the comand is reset or sleep or check."); 
							if(header.indexOf("reset") >= 0){//重启
								for(int h=0;h<1;h++){
								  uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,8,0} ;//buf[13]=8是报警灯重启命令
								  sendDatato433(data);
								  delay(2000);
								  sendDatato2G4(data);
								}
						    }
							 if (header.indexOf("check") >= 0) {
							  for(int h=0;h<1;h++){
								uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,1,0} ;//buf[13]=1是报警灯点名命令
								sendDatato433(data);
								delay(2000);
								sendDatato2G4(data);
							  }
							}
							if (header.indexOf("sleep") >= 0) {
							  for(int h=0;h<1;h++){
								uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,9,0} ;//buf[13]=9是报警灯休眠命令
								sendDatato433(data);
								delay(2000);
								sendDatato2G4(data);
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
						for(int h=0;h<1;h++){
							  uint8_t data[14] ={red,yellow,blue,i_Ontime/10,i_Offtime/10,0,0,0,0,0,0,0,2,0} ;//buf[13]=2是单色灯命令
							  sendDatato433(data);
							  delay(2000);
							  sendDatato2G4(data);
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
void sendSYN(uint8_t *data){
	
	
	uint8_t SYN1[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],1} ;
	uint8_t SYN2[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],2} ;
	uint8_t SYN3[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],3} ;
	uint8_t SYN4[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],4} ;
	uint8_t SYN5[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],5} ;
	uint8_t SYN6[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],6} ;
	uint8_t SYN7[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],7} ;
	uint8_t SYN8[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],8} ;
	uint8_t SYN9[14] ={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],9} ;
	uint8_t SYN10[14]={data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],10} ;
	
	nrf24.send(SYN1, sizeof(SYN1));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN2, sizeof(SYN2));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN3, sizeof(SYN3));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN4, sizeof(SYN4));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN5, sizeof(SYN5));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN6, sizeof(SYN6));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN7, sizeof(SYN7));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN8, sizeof(SYN8));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN9, sizeof(SYN9));
	nrf24.waitPacketSent();
	delay(10);
	
	nrf24.send(SYN10, sizeof(SYN10));
	nrf24.waitPacketSent();
	delay(10);
	
}
void sendDatato2G4(uint8_t *data){
	sendSYN(data);
	//nrf24.send(data, sizeof(data));
	//nrf24.waitPacketSent();
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
	}

}
