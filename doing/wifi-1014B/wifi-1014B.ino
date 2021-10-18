/*********
  fangtao
baojinden-jizhanchengxu
*********/

#include <Arduino.h>
//  #ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
//#else
//  #include <ESP8266WiFi.h>
//  #include <ESPAsyncTCP.h>
//#endif
#include <ESPAsyncWebServer.h>
//#include <TinyGPS++.h>
//TinyGPSPlus gps;
//#define GPS_BAUD 9600
//#include <TimeLib.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include "SPIFFS.h"
#include <FS.h>   //Include File System Headers
//***
//#include <RF24.h>
//***
// Replace with your network credentials


//const char* ssid = "203-1";
//const char* password = "njustXP2017";

//const char* ssid = "TP-LINK_502";
//const char* password ="88102110";

//const char* ssid = "CMCC3";
//const char* password ="84817200";
const char* input1;
const char* input0;
// Set web server port number to 80
WiFiServer server(80);
//AsyncWebServer server(80);
// Variable to store the HTTP request
String header;

RH_NRF24 nrf24(2,4);//CE,CSN
uint8_t red;
uint8_t yellow;
uint8_t blu;
int Tempmant;
const int dur=2;
const int itv=2;
int ComHor;
int ComMin;
int ArsHor;
int ArsMin;
int fois=0;
int FFmount;
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
String ssid,password;
String m_ssid, m_sn;//从文件数据中提取的WIFI热点名称和密码
int firOfColon,secOfColon;//第一个符号的位置,第二个符号的位置

int x=0;
String  msn1,msn2,msn3,msn4,msn5;
int m1,m2,m3,m4,m5;
uint8_t data[14];

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
String output25State = "off";
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 25;
const int output25 = 27;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



// Set your Static IP address
IPAddress local_IP(192, 168, 110, 200);
// Set your Gateway IP address
IPAddress gateway(192, 168, 110, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional



void setup() {
 
  Serial.begin(115200);
  
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(output25, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  digitalWrite(output25, LOW);
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(100))
    Serial.println("setChannel failed");
  //if (!nrf24.setRF(RH_NRF24::DataRate250Kbps, RH_NRF24::TransmitPower0dBm))
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
  // Connect to Wi-Fi network with SSID and password

//read wifiname and password:
  File myFile;
//  String file_str;//从文件读取的数据转成String类型
//  String ssid, password;//从文件数据中提取的WIFI热点名称和密码
//  int firOfColon, secOfColon;//第一个冒号的位置,第二个冒号的位置
  

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
 //   Serial.print("firOfColon=");
 //   Serial.println(firOfColon);
 //   Serial.print("secOfColon=");
 //   Serial.println(secOfColon);
 
  int  posOfCr = file_str.indexOf('\r');

    ssid = file_str.substring(firOfColon + 1, posOfCr);
    ssid.trim();//自动清除字符串中最前面和最后面的空白数据。不做清除字符串会包含有'\n'
    password = file_str.substring(secOfColon + 1);
    password.trim();//保险起见，自动清除字符串中最前面和最后面的空白数据。

  //  myFile.close();
   delay(10);
  } else {
    Serial.println("Open File Failed.");
  }
 


    // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
//    Serial.print("ssid=");
//    Serial.print(ssid);
//    Serial.print("password=");
//    Serial.println(password);
  const char* c1 = ssid.c_str();
  const char* c2 = password.c_str();
  Serial.print("Connecting to ");
  Serial.print(c1);   Serial.print(",");  Serial.println(c2);
  
   WiFi.begin(c1,c2);
//  WiFi.begin("CMCC3", "84817200");
//  WiFi.softAP(ssid,password);  
//  IPAddress local_IP(192, 168, 110, 200);
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

//xiashu baojinden ID
   File file = SPIFFS.open("/data.txt");
  if(!file){
    Serial.println("data.txt Failed to open file for reading");
    return;
  }
  
  Serial.println("data.txt File Content:");
  while(file.available()){
    
   Serial.write(file.read());
//   Serial.println(file.read());
  }
  file.close();

//
  
}

int i_Ontime;
int i_Offtime;
int i_Tras;
void loop()
{
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime){  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
	  
        char c = client.read();             // read a byte, then
		//Serial.write(c);                    // print it out the serial monitor
        header += c;
        String input1=header;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
		  //"192.168.12.1/o=y=200s=200e="
		  //"192.168.12.1/o=rylw=200s=200e=2t="
          if (currentLine.length() == 0) {

			if (header.indexOf("f=") > 0){
				red = yellow = blu = 0;
			}
            else if (header.indexOf("o=") > 0){ 
				int colorbegin=0;int sbegin=0;int ebegin=0;int wbegin=0;int tbegin=0;	
				sbegin = header.indexOf("s=");
				ebegin = header.indexOf("e=");
				if(header.indexOf("y=") > 0 && sbegin > 0 && ebegin > 0){
          Serial.println("yellow light.");
					colorbegin = header.indexOf("y=");
					yellow = 1;
					red = blu = 0;
				}
				else if (header.indexOf("b=") > 0 && sbegin > 0 && ebegin > 0){
          Serial.println("blue light.");
					colorbegin = header.indexOf("b=");
					blu = 1;
					red = yellow = 0;
				}
				else if (header.indexOf("r=") > 0 && sbegin > 0 && ebegin > 0){
          Serial.println("red light.");
					colorbegin = header.indexOf("r=");
					red = 1;
					yellow = blu = 0;
			 }
				else if (header.indexOf("w=") > 0 && sbegin > 0 && ebegin > 0 &&  header.indexOf("t=")> 0){
          Serial.println("w light.");
					colorbegin = wbegin = header.indexOf("w=");
					tbegin  = header.indexOf("t=");
					red = 1;
					yellow = 2;
					blu = 3;
				}
				else{
					Serial.println("the comand is wrong."); 
					//retrun err to http server
				}
				if(colorbegin  > 0 ){
					String Ontime = header.substring(colorbegin+2, sbegin);
					i_Ontime = Ontime.toInt();
					String Offtime = header.substring(sbegin+2, ebegin);
					i_Offtime = Offtime.toInt();
					Serial.println(String("ontime = ") + i_Ontime + String(",offtime = ") + i_Offtime );
				}
				if(wbegin > 0 ){
					String Trastime = header.substring(ebegin+2, tbegin);
					i_Tras = Trastime.toInt();
					Serial.println(String("Trastime = ") + i_Tras);
					Serial.println(String("wbegin = ") + wbegin);
				}
				//开关时间 和 错灯间隔
				for(int h=0;h<1;h++){
					uint8_t data[14] ={red,yellow,blu,i_Ontime/10,i_Offtime/10,i_Tras,0,0,0,0,0,0,0,1} ;
					nrf24.send(data, sizeof(data));
					nrf24.waitPacketSent();
				}
      }
			else if(header.indexOf("reset") >= 0){//重启
				for(int h=0;h<1;h++){
					uint8_t data[14] ={0,0,0,0,0,0,0,0,0,0,0,0,0,8} ;//buf[13]=8是报警灯重启命令
					nrf24.send(data, sizeof(data));
					nrf24.waitPacketSent();
				}
			}
			else if (header.indexOf("check") >= 0) {
			}
			else if (header.indexOf("sleep") >= 0) {
			}
			else { //选择3
			  Serial.println("no command can handle error");
			}//选择3
      
    digitalWrite(output26, red>0);
    digitalWrite(output27, blu>0);
    digitalWrite(output25, yellow>0);
	    // Display the HTML web page
		client.println("<!DOCTYPE html><html>");
		client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		client.println("<link rel=\"icon\" href=\"data:,\">");
		// CSS to style the on/off buttons 
		// Feel free to change the background-color and font-size attributes to fit your preferences
		client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		client.println("div { display: inline-block; margin: 0px auto; text-align: center;}");
		client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 8px 40px;");
		client.println("text-decoration: none; font-size: 10px; margin: 0px; cursor: pointer;}");
		client.println(".button2 {background-color: #FF0000;}</style></head>");

		client.println("<body><h3>NJUST ALARM-LAMP CONTROL</h3>");

		client.println("<div>");     
		client.println("</form><br>");
		client.println("<form action='/get'>");
		client.println("Sleep time: <input type='text' name='input0'>");
		client.println("<input type='submit' value='Submit'>");
		client.println("</form>");
		client.println("</div>");  


		client.println("<p><a href=\"/reset\"><button class=\"button\">RESET</button></a></p>");

		client.println("</form><br>");

		// Display current state, and ON/OFF buttons for GPIO 26 
		client.println("<div>");
		client.println("<p>Blue " + output26State + "</p >");
		// If the output26State is off, it displays the ON button       
		if (red==0) {
		client.println("<p><a href=\"/26/on\"><button class=\"button\">ON_</button></a></p>");
		} else {
		client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
		} 
		client.println("</div>");
		client.println("<div>");
		// Display current state, and ON/OFF buttons for GPIO 27  
		client.println("<p>Red " + output27State + "</p >");
		// If the output27State is off, it displays the ON button       
		if (blu==0) {
		client.println("<p><a href=\"/27/on\"><button class=\"button\">ON_</button></a></p>");
		} else {
		client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
		}
		client.println("</div>");

		// Display current state, and ON/OFF buttons for GPIO 25  
		client.println("<div>");
		client.println("<p>Yellow " + output25State + "</p>");
		// If the output25State is off, it displays the ON button       
		if (yellow==0) {
		client.println("<p><a href=\"/25/on\"><button class=\"button\">ON_</button></a></p>");
		} else {
		client.println("<p><a href=\"/25/off\"><button class=\"button button2\">OFF</button></a></p>");
		} 

		client.println("</div>");

		// Display current state, and INT buttons for pinglu  

		client.println("<p>");
		//  displays the FRE  button   
		client.println("<div>");    
		client.println(i_Ontime);  
		client.println("<p>");       
		client.println("</p>");          
		client.println("<p><a href=\"/I\"><button class=\"button\">ON time</button></a></p>");

		client.println("</div>");  
		client.println("<div>");         
		client.println(i_Offtime);
		client.println("<p><a href=\"/J\"><button class=\"button\">OFF time</button></a></p>");
		client.println("</div>"); 
		//gaibian pinglv
		client.println("<form action='/get'>");
		client.println("on time: <input type='text' name='input2'>");
		client.println("<input type='submit' value='Submit'>");
		client.println("</form><br>");
		client.println("<form action='/get'>");
		client.println("off time: <input type='text' name='input3'>");
		client.println("<input type='submit' value='Submit'>");
		client.println("</form><br>");

		client.println("<p><a href=\"/water\"><button class=\"button\">Flowing Water</button></a></p>");
		client.println("<p><a href=\"/check\"><button class=\"button\">Check all</button></a></p>");
		client.println("<form action='/get'>");
		client.println("checkID <input type='text' name='input1'>");
		client.println("<input type='submit' value='Submit'>");
		client.println("</form><br>");
		//       client.println("<body><h6></h6>");          
		client.println("</body></html>");
		//  Serial.println(header);
		if (bufx != "")
		{
			client.print(bufx);
			client.println("OK");
			bufx = "";
		}
		else
		{
			client.println("");
		}   

		// The HTTP response ends with another blank line
		client.println();


		fois = millis();
		// uint8_t data[14] ={red,yellow,blu,dur*i,itv*j,fois,6,0,0,0,0,23,59,0} ;
		// nrf24.send(data, sizeof(data));
		// nrf24.waitPacketSent();

		//********************            
		// Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  
        // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          Serial.print(c);
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

      
