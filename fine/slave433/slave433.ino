#include "Arduino.h"
#include <RH_NRF24.h>

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin

//SoftwareSerial RS485Serial(RXPin, TXPin); // RX, TX
RH_NRF24 nrf24(2,4);//CE,CSN
int byteSend;
bool isDataReceived = false;
bool send0 = false;
void setup()  {
  Serial.begin(9600);
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   // set the data rate
 
  delay(500);
    
  if (!nrf24.init())
	Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2)
  if (!nrf24.setChannel(100))
    Serial.println("setChannel failed");
  //if (!nrf24.setRF(RH_NRF24::DataRate250Kbps, RH_NRF24::TransmitPower0dBm))
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");


	//pinMode(output25, OUTPUT);
	//digitalWrite(output25, LOW);
	Serial.println("init finished.");

}
int beginflag = 0;
int h = 0;
bool truestart = false;
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
void loop() {

	byte buf[15];
	uint8_t data[14];
 if (Serial2.available()){
      //dataReceived
       // get the new byte:
      char inChar = (char)Serial2.read();
    if(truestart == false)
    {
  	  if (inChar == 0x99)
  	  {
    		Serial.print("Data begin from 0x99");
    		beginflag = 1;
  	  }	
  	  if (inChar== 0x88 || inChar == 0x77 || inChar == 0x66 || inChar == 0x55  )
  	  {
    		Serial.println("Data begin for 88 77 66 55 ");
    		beginflag = beginflag + 1;
        if(inChar == 0x55)
        {
          truestart = true;
          h = 0;
        }
  	  }	
    }
    else
    {
       data[h] = inChar;
       h++;
    }
 
    if(h >=14)
    {
	  delay(2000);	
      Serial.println("send data to nrf24");
	  sendSYN(data);
      //nrf24.send(data, 14); 
      for (int i = 0; i < 14; i++)
      {
        Serial.print(data[i]);
        Serial.print(",");
      }
      //nrf24.waitPacketSent();
      h  = 0; 
      beginflag = 0;
      truestart = false;
    }
  }
 
}
