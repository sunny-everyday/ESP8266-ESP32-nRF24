#include <WiFi.h>

#include <ESPmDNS.h>

//#include <WebServer.h>

#include <WiFiMulti.h>

#include <ESPAsyncWebServer.h>

#include <RH_NRF24.h>

#include "SPIFFS.h"

const char* AP_SSID  = "ESP32_Config"; //热点名称

const char* AP_PASS  = "12345678";  //密码

#define ROOT_HTML  "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"icon\" href=\"data:,\"> <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;} div { display: inline-block; margin: 0px auto; text-align: center;} .button { background-color: #4CAF50; border: none; color: white; padding: 8px 40px; text-decoration: none; font-size: 10px; margin: 0px; cursor: pointer;} .button2 {background-color: #FF0000;}</style></head> <body><h3>NJUST ALARM-LAMP CONTROL</h3> <div>      </form><br> <form action='/get'> Sleep time: <input type='text' name='input0'> <input type='submit' value='Submit'> </form> </div>   <p><a href=\"/reset\"><button class=\"button\">RESET</button></a></p> </form><br> <div> <p>Blue</p > <p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p> </div> <div> <p>Red</p > <p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p> </div> <div> <p>Yellow</p> <p><a href=\"/25/off\"><button class=\"button button2\">OFF</button></a></p> </div> <p> <div>     <p>        </p>           <p><a href=\"/I\"><button class=\"button\">ON time</button></a></p> </div>   <div>          <p><a href=\"/J\"><button class=\"button\">OFF time</button></a></p> </div>  <form action='/get'> on time: <input type='text' name='input2'> <input type='submit' value='Submit'> </form><br> <form action='/get'> off time: <input type='text' name='input3'> <input type='submit' value='Submit'> </form><br> <p><a href=\"/water\"><button class=\"button\">Flowing Water</button></a></p> <p><a href=\"/check\"><button class=\"button\">Check all</button></a></p> <form action='/get'> checkID <input type='text' name='input1'> <input type='submit' value='Submit'> </form><br> </body></html>"

WiFiServer server(80);

WiFiMulti wifiMulti;


uint8_t resr_count_down = 120;//重启倒计时s

TimerHandle_t xTimer_rest;

void restCallback(TimerHandle_t xTimer );

const char* input1;
const char* input0;
String header;
RH_NRF24 nrf24(2,4);//CE,CSN
bool red;  //27
bool blue; //26
bool yellow;  //25
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
int x=0;
int firOfColon,secOfColon;//第一个符号的位置,第二个符号的位置
String  m_sn,msn1,msn2,msn3,msn4,msn5;
int m1,m2,m3,m4,m5;
uint8_t data[14];

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
String output25State = "off";
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
const int output25 = 25;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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


	xTimer_rest = xTimerCreate("xTimer_rest", 1000 / portTICK_PERIOD_MS, pdTRUE, ( void * ) 0, restCallback);

	xTimerStart( xTimer_rest, 0 );  //开启定时器


  } else {  //开启热点失败

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
              blue=1;
              digitalWrite(output26, HIGH);

            }//选择1
            else if (header.indexOf("GET /26/off") >= 0) { //选择2
              Serial.println("GPIO 26 off");
              output26State = "off";
               blue=0;
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
              yellow=1;
              digitalWrite(output25, HIGH);

            } //选择5
            else if (header.indexOf("GET /25/off") >= 0) { //选择6
              Serial.println("GPIO 25 off");
              output25State = "off";
              yellow=0;
              digitalWrite(output25, LOW);

            }//选择6结束

			for(int h=0;h<5;h++){
			uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
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
		uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,23,59,0} ;
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
//    Serial.print("CheckID= ");
//    Serial.println(id);
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
	  uint8_t len = sizeof(data);
	nrf24.waitAvailableTimeout(10);

  if (nrf24.recv(data, &len))
 {
 // for (int h = 0;h<14;h++){
 //   Serial.print(data[h]);
 //    Serial.print(",");
 //     }
}
 //    Serial.print("!!!!!!");
  if(data[13]==2){
   buf8=data[8]*10000;
   buf9=data[9]*1000;
   buf10=data[10]*100;
   buf11=data[11]*10;
   buf12=data[12];
   bufx=String(buf8+buf9+buf10+buf11+buf12);
    Serial.print(bufx);    
    Serial.println(" OK");

	data[8]=0;
	data[9]=0;
	data[10]=0;
	data[11]=0;
	data[12]=0;
  //delay(100);
  }
  else{
    Serial.println("Error Attenant Reception");
    //bufx=0;
     }

 //  }//循环结束

//******
     

	for(int h=0;h<5;h++){
	uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
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
	uint8_t data[14] ={red,blue,yellow,id20,id21,fois,6,0,0,0,0,23,59,0} ;
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
	uint8_t data[14] ={red,blue,yellow,id20,id21,fois,6,0,0,0,0,23,59,0} ;
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
	uint8_t data[14] ={red,blue,yellow,100,100,fois,6,0,a,b,c,d,e,7} ;;//buf[13]=7是自定时间报警灯休眠命令
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
  uint8_t len = sizeof(data);
	nrf24.waitAvailableTimeout(10);

  if (nrf24.recv(data, &len))
 {
  for (int h = 0;h<14;h++){
    Serial.print(data[h]);
     Serial.print(",");
       }
}
 //   nrf24.setModeTx(); 
     Serial.print("!!!!!!");
  if(data[13]==2){
   buf8=data[8]*10000;
   buf9=data[9]*1000;
   buf10=data[10]*100;
   buf11=data[11]*10;
   buf12=data[12];
   bufx=bufx + String(buf8+buf9+buf10+buf11+buf12);
    Serial.print(bufx);    
    Serial.println(" ok");
         // client.print(bufx);
         //  client.println("OK");   
           
  
  }
  else{
 //   Serial.println("Erreur Attenant Reception");
   // bufx=0;
   
  }
  delay(500);
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
	uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,25,60,0} ;
	nrf24.send(data, sizeof(data));
	nrf24.waitPacketSent();
	}
  }
  
//****************************8//选择9

if(header.indexOf("water") >= 0){//滚动显示

	 for(int h=0;h<2;h++){
	   uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,22,58,5} ;//buf[13]=5是滚动闪烁命令
	nrf24.send(data, sizeof(data));
	nrf24.waitPacketSent();
	}
	data[13]=0;
	Serial.print("Send water data：");

	Serial.print(red);
	Serial.print(",");
	Serial.print(blue);
	Serial.print(",");
	Serial.print(yellow);
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
	   uint8_t data[14] ={red,blue,yellow,dur*i,itv*j,fois,6,0,0,0,0,22,60,8} ;//buf[13]=8是报警灯重启命令
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

    fois = millis();



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
