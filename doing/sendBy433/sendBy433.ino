#include "Arduino.h"

#define RXD2        16  // Serial2 Receive pin
#define TXD2        17  // Serial2 Transmit pin
 
//RS485 control
#define SERIAL_COMMUNICATION_CONTROL_PIN 5 // Transmission set pin
#define RS485_TX_PIN_VALUE HIGH
#define RS485_RX_PIN_VALUE LOW
 
//SoftwareSerial RS485Serial(RXPin, TXPin); // RX, TX
 
int byteSend;
 
void setup()  {
  Serial.begin(9600);
 
  pinMode(SERIAL_COMMUNICATION_CONTROL_PIN, OUTPUT);
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_RX_PIN_VALUE);
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);   // set the data rate
 
  delay(500);
 
  digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE); // Now trasmit

}
 
void loop() {
      digitalWrite(SERIAL_COMMUNICATION_CONTROL_PIN, RS485_TX_PIN_VALUE);  // Disable RS485 Transmit
      Serial.println("Send data!");
      Serial2.print("Hello world!"); // Send message
      if (Serial2.available())
      {
            Serial.println(Serial2.readString());
      }
      delay(1000);
}
