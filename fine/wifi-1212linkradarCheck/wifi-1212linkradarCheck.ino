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


String ID_str;//从文件读取的数据转成String类型
int x=0;

String ssid,password,localip,gateway;
String m_ssid, m_sn;//从文件数据中提取的WIFI热点名称和密码
int firOfColon,secOfColon;//第一个符号的位置,第二个符号的位置

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

#define CHANNEL_MIN    100

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin

#define LIG       15 

//// Set your Static IP address
//IPAddress local_IP(192, 168, 1, 2);
//// Set your Gateway IP address
//IPAddress gateway(192, 168, 1, 1);
// Set your Static IP address
IPAddress local_IP(32, 182, 233, 6);
// Set your Gateway IP address
IPAddress gateWay(32, 182, 233, 1);
IPAddress subnet(255, 255, 255, 128);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

uint8_t data[14] ;
bool TurnONfromC = false;
bool TurnOFFfromC = false;
void getinfofromtxt(String file_str)
{
  String stringOne = file_str;
  int firstClosingBracket = stringOne.indexOf(':');
  Serial.println("The index of : in the string " + stringOne + " is " + firstClosingBracket);

  int secondOpeningBracket = firstClosingBracket + 1;
  int secondClosingBracket = stringOne.indexOf(':', secondOpeningBracket);
  Serial.println("The index of  the second : in the string " + stringOne + " is " + secondClosingBracket);


  int thirdOpeningBracket = secondClosingBracket + 1;
  int thirdClosingBracket = stringOne.indexOf(':', thirdOpeningBracket);
  Serial.println("The index of  the third : in the string " + stringOne + " is " + thirdClosingBracket);
  
  
  int lastOpeningBracket = file_str.lastIndexOf(':');//从字符串尾部查找":"位置
  // you can also use indexOf() to search for Strings:
  int  posOf1r = file_str.indexOf('\r');
  
  int secondrBegin = posOf1r + 1;
  int posOf2r = stringOne.indexOf('\r', secondrBegin);
  Serial.println("The index of  the second \rin the string " + stringOne + " is " + secondClosingBracket);


  int thirdrBegin = posOf2r + 1;
  int posOf3r = stringOne.indexOf('\r', thirdrBegin);
  Serial.println("The index of  the third \r in the string " + stringOne + " is " + thirdClosingBracket);
  
  
	ssid = file_str.substring(firstClosingBracket + 1, posOf1r);
	ssid.trim();//自动清除字符串中最前面和最后面的空白数据。不做清除字符串会包含有'\n'
	Serial.println(ssid);
	password = file_str.substring(secondClosingBracket + 1,posOf2r);
	password.trim();//保险起见，自动清除字符串中最前面和最后面的空白数据。
	Serial.println(password);
	
	localip = file_str.substring(thirdClosingBracket + 1, posOf3r);
	localip.trim();
	Serial.println(localip);
    gateway = file_str.substring(lastOpeningBracket+ 1);
	gateway.trim();
	Serial.println(gateway);

  bool x= local_IP.fromString(localip);
  Serial.println(x);
  bool y= gateWay.fromString(gateway);
  Serial.println(y);

}
void setup() {
 
  Serial.begin(115200);
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   // set the data rate
  
  
  pinMode(LIG, OUTPUT);
  digitalWrite(LIG, 1);

  delay(2000);
  digitalWrite(LIG, 0);
	// Initialize the output variables as outputs
   //TurnONfromC = true;

	if (!nrf24.init())
	Serial.println("init failed");
	// Defaults after init are 2.402 GHz (channel 2)
	if (!nrf24.setChannel(CHANNEL_MIN))
	Serial.println("setChannel failed");
	//if (!nrf24.setRF(RH_NRF24::DataRate250Kbps, RH_NRF24::TransmitPower0dBm))
	if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
	Serial.println("setRF failed");
  // Connect to Wi-Fi network with SSID and password


  Serial2.begin(9600);//, SERIAL_8N1, RXD2, TXD2);   // set the data rate
  //read wifiname and password:
  File myFile;

  

  if (!SPIFFS.begin()) {
    Serial.println("Start SPIFFS Failed!please check Arduino Download Config.");
    return;
  }
  Serial.println("File System Initialized");
  myFile = SPIFFS.open("/IP.txt", "r");//文件名前的反斜杠不能少
  String file_str;//从文件读取的数据转成String类型
  if (myFile) {
    Serial.println("Reading IP.txt...");
    while (myFile.available()) {
      //读取文件输出
      file_str += (char)myFile.read(); //循环读取文件转换成String类型
     //Serial.write(myFile.read());//循环文件输出到串口
    }
    myFile.close();
	getinfofromtxt( file_str);
  } else {
    Serial.println("Open File Failed.");
  }


    // Configures static IP address
  if (!WiFi.config(local_IP, gateWay, subnet, primaryDNS, secondaryDNS)) {
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

  
//
  
}
int i_Ontime = 0;
int i_Offtime = 0;
int  i_Tras = 0;
unsigned short rev;
char temp;
bool TurnONfromR = false;
bool TurnOFFfromR = false;


void data_analyse(char temp)
{
  if(temp == 'o')
  {Serial.println("alarm begin");TurnONfromR = true;digitalWrite(LIG, 1);}
  if(temp == 'c')
  {Serial.println("alarm end");TurnOFFfromR = true;digitalWrite(LIG, 0);}
}

const char* host = "15r34407v7.imwork.net"; //需要访问的域名
const int httpsPort = 37170;  // 需要访问的端口
const String url = "/test";   // 需要访问的地址
String test_ssid="abc,,,defg,,,hijk,,,lmno";

void SendHTTPGet(WiFiClient client)
{
	
	 String postRequest =(String)("GET ") + url + "/ HTTP/1.1\r\n" +  
     "Content-Type: text/html;charset=utf-8\r\n" +  
     "Host: " + host + "\r\n" + 
     "User-Agent: BuildFailureDetectorESP8266\r\n" +
     "Connection: Keep Alive\r\n\r\n";  
   Serial.println(postRequest);  
   client.print(postRequest);  // 发送HTTP请求
}

void SendHTTP200(WiFiClient client)
{
	
   String postRequest =(String)("HTTP/1.1 200 OK\r\n\r\n");  
   Serial.println(postRequest);  
   client.print(postRequest);  // 发送HTTP请求
}
void testsplit(String ID_str)
{
	
	for(int index = 0; index < 10;index++)
	{
		String part01 = Split(ID_str,",,,",index);
		Serial.println(part01);
		if(part01 != "")
		{
			for(int h=0;h<1;h++){
				int a = part01.substring(0,1).toInt();
				int b = part01.substring(1,2).toInt();
				int c = part01.substring(2,3).toInt();
				int d = part01.substring(3,4).toInt();
				int e = part01.substring(4,5).toInt();
				uint8_t data[14] ={a,b,c,d,e,1,1,1,1,1,1,1,1,1} ;//buf[13]=1是报警灯点名命令
				sendDatato2G4(data);
			}	
			
			 // 接收
				  uint8_t len = sizeof(data);
				nrf24.waitAvailableTimeout(100);
				//nrf24.setModeRx(); 
				//uint8_t clearRxBuf();
				//uint8_t flushRx();
				//nrf24.waitAvailable(); 

				  if (nrf24.recv(data, &len))
				  {
				  for (int h = 0;h<14;h++){
					Serial.print(data[h]);
					 Serial.print(",");
					   }
				  }
				 //   nrf24.setModeTx(); 
				  Serial.print("!!!!!!");
				  if(data[5]==5 || data[6] == 5 || data[7] ==5 || data[8] == 5 || data[9] == 5|| data[10] ==5 || data[11] ==5 ||data[12]==5){

					Serial.print(part01);    
					Serial.println(" ok"); 
				    delay(1000);
				  }
				  else{
					Serial.println("Erreur Attenant Reception");
				   
				  }
		   
		}	 
		else
		{
			return;
		}			
	}

	
}

String Split(String str,String fen,int index)
{
 int weizhi;
 String temps[str.length()];
 int i=0;
 do
 {
    weizhi = str.indexOf(fen);
    if(weizhi != -1)
    {
      temps[i] =  str.substring(0,weizhi);
      str = str.substring(weizhi+fen.length(),str.length());
      i++;
      }
      else {
        if(str.length()>0)
        temps[i] = str;
      }
 }
  while(weizhi>=0);

  if(index>i)return "-1";
  return temps[index];
}
void getIDandSendCMD()
{
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
    
    ID_str += (char)(file.read());

  }
  file.close();
  
  testsplit(ID_str);
	
}
void loop()
{
  //接收雷达信号
   rev = Serial2.available();
   if(rev != 0)
   {  
      temp = Serial2.read();   //读取一个数据并且将它从缓存区删除
      Serial.println(temp);    //
	  if(TurnONfromC)
	  {data_analyse(temp);}    //
    }
 
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
						//关灯
						TurnOFFfromC  = true;
						TurnONfromC = false;
						SendHTTP200(client);
					}
					else if (header.indexOf("o=") > 0){ 
						TurnONfromC = true;
						SendHTTP200(client);
						
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
						
						else
						{
							Serial.println("the comand is reset or sleep or check."); 
							if(header.indexOf("reset") >= 0){//重启
								for(int h=0;h<1;h++){
								  uint8_t data[14] ={0,0,0,0,0,0,8,8,8,8,8,8,8,0} ;//buf[13]=8是报警灯重启命令
									sendDatato2G4(data);
								}
						    }
							 if (header.indexOf("check") >= 0) {
								 
								getIDandSendCMD(); 
							  
							}
							if (header.indexOf("sleep") >= 0) {
							  for(int h=0;h<1;h++){
								uint8_t data[14] ={0,0,0,0,0,0,5,9,9,9,9,9,9,0} ;//buf[13]=9是报警灯休眠命令
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
							uint8_t Tempdata[14] ={red,yellow,blue,i_Ontime/10,i_Offtime/10,0,2,2,2,2,2,2,2,0} ;//buf[13]=2是单色灯命令
							memcpy(data, Tempdata,14);
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
  
  if(TurnONfromC)
  {
	if(TurnONfromR)
	{
		for(int h=0;h<3;h++){
		sendDatato2G4(data);
		nrf24.waitPacketSent();
		delay(10);
		}
		TurnONfromR = false;
		Serial.println("TurnON by Radia");
	}
	if(TurnOFFfromR)
	{
	  uint8_t data[14] ={0,0,0,0,0,0,2,2,2,2,2,2,2,0} ;//buf[13]=2是报警灯休眠命令
		for(int h=0;h<3;h++){
		  nrf24.send(data, 14);
		  nrf24.waitPacketSent();
		  delay(10);
		}
		TurnOFFfromR = false;
		
		Serial.println("TurnOFF by Radia");
	}
  }
   if(TurnOFFfromC)
   {
	   uint8_t data[14] ={0,0,0,0,0,0,2,2,2,2,2,2,2,0} ;//buf[13]=2是报警灯休眠命令
		for(int h=0;h<3;h++){
		  nrf24.send(data, 14);
		  nrf24.waitPacketSent();
		  delay(10);
		}
		TurnOFFfromR = false;
		Serial.println("TurnOFF by Control");
	   TurnOFFfromC = false;
   }
 
}

void sendDatato2G4(uint8_t *data){

  for(int h=0;h<14;h++)
  {
    Serial.print(data[h]);  
  }
	nrf24.send(data, 14);
	nrf24.waitPacketSent();
        Serial.println("send OK ");  
}
