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
bool red;
bool grn;
bool blu;
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
  if (!nrf24.setChannel(50))
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
 
    //Initialize File System
//    WiFi.softAP(m_ssid, m_sn);
//    IPAddress IP = WiFi.softAPIP();
//    Serial.print("AP IP address: ");
//    Serial.println(IP);




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


void loop()
{
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
 //   Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)    {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
	  
        char c = client.read();             // read a byte, then
    //   Serial.write(c);                    // print it out the serial monitor
        header += c;
         String input1=header;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
       //    Serial.write(c);
            
    //        client.println("HTTP/1.1 200 OK");
    //        client.println("Content-type:text/html");
    //        client.println("Connection: close");
    //        client.println();
            
            // turns the GPIOs on and off

//
  if ((header.indexOf("GET /26/on") >= 0)||(header.indexOf("GET /26/off") >= 0)||(header.indexOf("GET /25/on") >= 0)||(header.indexOf("GET /25/off") >= 0)||(header.indexOf("GET /27/on") >= 0)||(header.indexOf("GET /27/off") >= 0)) { 
            if (header.indexOf("GET /26/on") >= 0) { //选择1
              Serial.println("GPIO 26 on");
              output26State = "on";
              grn=1;
              digitalWrite(output26, HIGH);

            }//选择1
            else if (header.indexOf("GET /26/off") >= 0) { //选择2
              Serial.println("GPIO 26 off");
              output26State = "off";
               grn=0;
              digitalWrite(output26, LOW);

            }//选择2
            else if (header.indexOf("GET /27/on") >= 0) { //选择3
              Serial.println("GPIO 27 on");
              output27State = "on";
              red=1;
              digitalWrite(output27, HIGH);

            }//选择3
            else if (header.indexOf("GET /27/off") >= 0) { //选择4
              Serial.println("GPIO 27 off");
              output27State = "off";
              red=0;
              digitalWrite(output27, LOW);

            }//选择4
            else if (header.indexOf("GET /25/on") >= 0) { //选择5
              Serial.println("GPIO 25 on");
              output25State = "on";
              blu=1;
              digitalWrite(output25, HIGH);

            } //选择5
            else if (header.indexOf("GET /25/off") >= 0) { //选择6
              Serial.println("GPIO 25 off");
              output25State = "off";
              blu=0;
              digitalWrite(output25, LOW);

            }//选择6结束

for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
            
  }
   
 else if((header.indexOf("GET /I")) >= 0||(header.indexOf("GET /J")) >= 0){  
              
              if (header.indexOf("GET /I") >= 0) { //选择7
              Serial.print("I,J=");
             
          if(i<20){i=i+1;
         }
         else
         {
          i=1;
          }

           Serial.print("I,J=");
 Serial.print(dur*i);
  Serial.print(",");
  Serial.println(itv*j);
     }//选择7
      else if (header.indexOf("GET /J") >= 0) { //选择8
              Serial.print("I,J=");

          if(j<20){j=j+1;
         }
         else
         {
          j=0;
          } 
           Serial.print("I,J=");
 Serial.print(dur*i);
 Serial.print(",");
  Serial.println(itv*j);


for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,23,59,0} ;
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
    }
    }//选择8
   }




//check ID 点名检查报警灯是否正常
    firOfColon = header.indexOf('=');//从字符串开始位置查找":"位置
    secOfColon = header.indexOf('H');//从字符串尾部查找":"位置
   String id = header.substring(firOfColon+1,secOfColon);
    id.trim();//自动清除字符串中最前面和最后面的空白数据。
   Serial.print("CheckID= ");
    Serial.println(id);
//Serial.println(header.substring(9,15));


if(header.substring(9,15)=="input1"){//选择11
bufx = "";
//Serial.println(header.substring(9,15));
m_sn = id.substring(0,5);

//Serial.print("ID=");Serial.println(m_sn.toInt());
 msn1=id.substring(0,1);
 msn2=id.substring(1,2);
 msn3=id.substring(2,3);
 msn4=id.substring(3,4);
 msn5=id.substring(4,5);

   m1=msn1.toInt();
    m2=msn2.toInt();
      m3=msn3.toInt();
       m4=msn4.toInt();
        m5=msn5.toInt();

 //   Serial.print(m1); 
 //   Serial.print(m2);
 //   Serial.print(m3);
 //   Serial.print(m4);
 //   Serial.println(m5);   


//*****

 //循环开始
     for(int h=0;h<1;h++){//循环点名多次               
    uint8_t data[14] = {14,13,12,11,10,9,8,7,m1,m2,m3,m4,m5,1};
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();
}
for(int h=0;h<14;h++)
{
  Serial.print(data[h]);
Serial.print(",");
}
 Serial.println(" ");   
 // 接收
              uint8_t revdata[RH_NRF24_MAX_MESSAGE_LEN];
              revdata[13] = 0; //保证数据有变化 不受上次数据影响
              uint8_t len = sizeof(revdata);
              nrf24.waitAvailableTimeout(200);

              if (nrf24.recv(revdata, &len))
              {
                 Serial.println(len);
              }
              Serial.print("=");
              if (revdata[13] == 2)
              {
                buf8 = revdata[8] * 10000;
                buf9 = revdata[9] * 1000;
                buf10 = revdata[10] * 100;
                buf11 = revdata[11] * 10;
                buf12 = revdata[12];
                bufx = String(buf8 + buf9 + buf10 + buf11 + buf12);
                Serial.print(bufx);
                Serial.println(" OK");
              }
              else
              {
                Serial.println("no response for input1");
              }           


 //  }//循环结束

//******
     

for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
  

//  Serial.println(header);
}//选择11

String idoff=header.substring(14,15);
idoff.trim();//自动清除字符串中最前面和最后面的空白数据。
//int idon=idoff.toInt();
//Serial.print("idon=");Serial.println(idon);
if(idoff == "2"){
  //选择亮灯时间

  Serial.println(header.substring(14,15));
   Serial.println("on time "+id);
   id20=id.toInt();
   id21=id21;
 Serial.print("id20="); Serial.println(id20); 
 Serial.print("id21="); Serial.println(id21);
 for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,id20,id21,fois,6,0,0,0,0,23,59,0} ;
i = int(id20 / (10 * dur));
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
}
 else if (idoff == "3")
 {//选择关灯时间

   Serial.println(header.substring(14,15));
   Serial.println("off time "+id);
  id20=id20;
  id21=id.toInt();
 Serial.print("id20="); Serial.println(id20); 
 Serial.print("id21="); Serial.println(id21);
 for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,id20,id21,fois,6,0,0,0,0,23,59,0} ;
j = int(id21 / (10 * itv));
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
}
 else if (idoff == "0")//****************************************************Sleep start
 {//选择休眠时间
                 
   Serial.println(header.substring(14,15));
//   Serial.println("Sleep time "+id);
int id30=id.toInt();
int a=floor(id30/10000);
int b=floor((id30-a*10000)/1000);
int c=floor((id30-a*10000-b*1000)/100);
int d=floor((id30-a*10000-b*1000-c*100)/10);
int e=id30-a*10000-b*1000-c*100-d*10;
 Serial.print("休眠时间（Min）="); Serial.println(id30);
 for(int h=0;h<2;h++){
uint8_t data[14] ={red,grn,blu,100,100,fois,6,0,a,b,c,d,e,7} ;;//buf[13]=7是自定时间报警灯休眠命令
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
data[13]=0;
}
Serial.println("start SLEEP.........");

}

//*****************************************************wake up


 //选择9 check all----//从这里开始是检查报警灯是否正常的程序，这个网页服务器有check按钮，按这个按钮，则nrf24l01模块发送{14,13,12,11,10,9,8,7,a,b,c,d,e,1} ;   
  if (header.indexOf("GET /check") >= 0) {
    bufx = "";
    Serial.println("check");   

     if (!SPIFFS.begin()) {
    Serial.println("Start SPIFFS Failed!please check Arduino Download Config.");
    return;
  }
  Serial.println("File System Initialized");
   myFile = SPIFFS.open("/data.txt", "r");//文件名前的反斜杠不能少
  if (myFile) {
    Serial.println("Reading data.txt...");
    while (myFile.available()) {
      //读取文件输出
      file_str += (char)myFile.read(); //循环读取文件转换成String类型
     }
     int len = file_str.length();
while(len>=1){
     len = file_str.length();
    
  //  m_sn.trim();//保险起见，自动清除字符串中最前面和最后面的空白数据。

//    Serial.println("呼叫。。。"); 
   int vieuxPos = 0;   

m_sn = file_str.substring(0,5);
Serial.print("ID= "); Serial.println(m_sn.toInt()); 

 msn1=file_str.substring(0,1);
 msn2=file_str.substring(1,2);
 msn3=file_str.substring(2,3);
 msn4=file_str.substring(3,4);
 msn5=file_str.substring(4,5);

   m1=msn1.toInt();
    m2=msn2.toInt();
      m3=msn3.toInt();
       m4=msn4.toInt();
        m5=msn5.toInt();

//    Serial.print(m1); 
//    Serial.print(m2);
//    Serial.print(m3);
//    Serial.print(m4);
//    Serial.println(m5);   


//*****

 //循环开始
                    
    uint8_t data[14] = {14,13,12,11,10,9,8,7,m1,m2,m3,m4,m5,1};
    nrf24.send(data, sizeof(data));
    nrf24.waitPacketSent();

for(int h=0;h<14;h++)
{
  Serial.print(data[h]);
Serial.print(",");
}
 Serial.println(" ");   
 data[13]=0;
                // 接收
                uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
                uint8_t len = RH_NRF24_MAX_MESSAGE_LEN;
                nrf24.waitAvailableTimeout(200);
                if (nrf24.recv(buf, &len))
                {
                     Serial.print("=");
                    if (buf[13] == 2)
                    {
                      buf8 = buf[8] * 10000;
                      buf9 = buf[9] * 1000;
                      buf10 = buf[10] * 100;
                      buf11 = buf[11] * 10;
                      buf12 = buf[12];
                      bufx = bufx + " " +  String(buf8 + buf9 + buf10 + buf11 + buf12);
                      Serial.print(bufx);
                      Serial.println(" OK");
                    }
                  }
                  else
                  {
                    Serial.println("no response");
                  }
  delay(2000);
 //  }//循环结束

//******
      firOfColon = file_str.indexOf("\n");//从字符串开始位置查找“回车”位置
      String resultat = file_str.substring(vieuxPos,firOfColon+1);
      file_str = file_str.substring(firOfColon+1);
         len = file_str.length();
         vieuxPos=firOfColon+1;
    }
    myFile.close();
    } 
    else {
    Serial.println("Open File Failed.");
   }
for(int h=0;h<5;h++){
uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
  }
  
//****************************8//选择9

if(header.indexOf("water") >= 0){//滚动显示
 //  dur=20;
 //  itv=30;
 //  red=1;
 //  grn=0;
 //  blu=0;

 for(int h=0;h<2;h++){
   uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,22,58,5} ;//buf[13]=5是滚动闪烁命令
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
data[13]=0;
Serial.print("Send water data：");
 // for (int h = 0;h<14;h++){
//    Serial.print( uint8_t data[14]);
//  }
Serial.print(red);
Serial.print(",");
Serial.print(grn);
Serial.print(",");
Serial.print(blu);
Serial.print(",");
Serial.print(dur*i);
Serial.print(",");
Serial.print(itv*j);
Serial.print(",");
Serial.print("0,6,0,0,0,0,22,58,");
Serial.println("5");

}//选择10结束


//--------------------------------------------
else if(header.indexOf("reset") >= 0){//重启
  for(int h=0;h<1;h++){
   uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,22,60,8} ;//buf[13]=8是报警灯重启命令
nrf24.send(data, sizeof(data));
nrf24.waitPacketSent();
}
Serial.print("Send reset data：");
 //for (int h = 0;h<14;h++){
 //   Serial.print( uint8_t data[14]);
 // }
 data[13]=0;
Serial.println("start RESET.........");

}//重启结束



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
            
            // Web Page Heading
            client.println("<body><h3>NJUST ALARM-LAMP CONTROL</h3>");
    //      client.println("<div>");    

   //    client.println("<p>");       
   //     client.println("</p>");          
   //       client.println("<p><a href=\"/sleep\"><button class=\"button\">SLEEP</button></a></p>");

   //       client.println("</div>");  
       
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
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON_</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</div>");
            client.println("<div>");
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Red " + output27State + "</p >");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON_</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</div>");
            
            // Display current state, and ON/OFF buttons for GPIO 25  
            client.println("<div>");
            client.println("<p>Yellow " + output25State + "</p>");
            // If the output25State is off, it displays the ON button       
            if (output25State=="off") {
              client.println("<p><a href=\"/25/on\"><button class=\"button\">ON_</button></a></p>");
            } else {
              client.println("<p><a href=\"/25/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("</div>");

            // Display current state, and INT buttons for pinglu  
         
         client.println("<p>");
   //  displays the FRE  button   
       client.println("<div>");    
       client.println(i*dur*10);  
    client.println("<p>");       
     client.println("</p>");          
       client.println("<p><a href=\"/I\"><button class=\"button\">ON time</button></a></p>");

       client.println("</div>");  
       client.println("<div>");         
       client.println(j*itv*10);
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


//
//******************
//
  //   client.print(bufx);
  //        client.print("OK");        
            
            // The HTTP response ends with another blank line
            client.println();


// Serial.println(output26State);
// Serial.println(grn);
// Serial.println(output27State);
// Serial.println(red);
// Serial.println(output25State);
// Serial.println(blu);
  fois = millis();


// uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,23,59,0} ;
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
        }

      }

    }
    // Clear the header variable
    
    header = "";
    // Close the connection
    client.stop();
  //  Serial.println("Client disconnected.");

  }
 
// uint8_t data[14] ={red,grn,blu,dur*i,itv*j,fois,6,0,0,0,0,23,59,0} ;
// nrf24.send(data, sizeof(data));
// nrf24.waitPacketSent();

}

      
