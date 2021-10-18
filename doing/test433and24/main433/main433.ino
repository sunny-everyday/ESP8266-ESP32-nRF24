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
 
void setup()  {
  Serial.begin(9600);
 
  pinMode(SERIAL_COMMUNICATION_CONTROL_PIN, OUTPUT);
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE);
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   // set the data rate
 
  delay(500);
 
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE); // Now trasmit
  
  
    
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
 
void loop() {
	digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE);  // Disable RS485 Read
	char data_1[14] ={1,0,0,0,0,1,0,0,3,0,0,5,1,1} ;
	Serial.println("send data_1 to nrf24");
  uint8_t uint8_1[14] ={1,0,0,0,0,1,0,0,3,0,0,5,1,1} ;
	nrf24.send(uint8_1, sizeof(uint8_1));
	nrf24.waitPacketSent();

	Serial.println("Send data_1 to 433!");
	Serial2.print(String(data_1)); // Send message

 
	delay(1000);
	
	char data_2[14] ={1,0,1,0,0,1,0,0,3,0,0,5,1,1} ;
  uint8_t uint8_2[14] ={1,0,1,1,0,1,0,0,3,0,0,5,1,1} ;
	Serial.println("send data_2 to nrf24");
	nrf24.send(uint8_2, sizeof(uint8_2));
	nrf24.waitPacketSent();
	Serial.println("Send data_2 to 433!");
	Serial2.print(String(data_2)); // Send message
	
	delay(1000);
}
