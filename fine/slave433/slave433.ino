#include "Arduino.h"
#include <RH_NRF24.h>

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin
 
//RS485 control
#define SERIAL_COMMUNICATION_CONTROL_PIN 5 // Transmission set pin
#define RS485_TX_PIN_VALUE HIGH
#define RS485_RX_PIN_VALUE LOW
 
//SoftwareSerial RS485Serial(RXPin, TXPin); // RX, TX
RH_NRF24 nrf24(2,4);//CE,CSN
int byteSend;
bool isDataReceived = false;
bool send0 = false;
void setup()  {
  Serial.begin(9600);
 
  pinMode(SERIAL_COMMUNICATION_CONTROL_PIN, OUTPUT);
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE);
  
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
void loop() {
	digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE);  //  RS485 Read

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
      Serial.println("send data to nrf24");
      nrf24.send(data, 14); 
      for (int i = 0; i < 14; i++)
      {
        Serial.print(data[i]);
        Serial.print(",");
      }
      nrf24.waitPacketSent();
      h  = 0; 
      beginflag = 0;
      truestart = false;
    }
  }
 
}
