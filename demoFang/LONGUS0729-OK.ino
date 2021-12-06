
//Vehicle Inspect 
#include <SPI.h>
#include <RH_NRF24.h>
#define UPTIME 6000 //开机时间ms,6秒
#define DOPP 33
#define RM_EN 26
#define RM_DE 25
RH_NRF24 nrf24(5,4);
void setup() 
{   pinMode(RM_EN,OUTPUT);
pinMode(RM_DE,OUTPUT);
pinMode(DOPP,INPUT);
  Serial.begin(115200);
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 10),250Kbps, 0dBm
  if (!nrf24.setChannel(10))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    

}
long tempus=0;
bool etat =0; 
void loop()
{
    delay(10);
    if(digitalRead(DOPP)<1){
      
        tempus=millis();
        etat=1;
        digitalWrite(RM_EN,1);
        delay(500);
        digitalWrite(RM_EN,0);
        Serial.println("Vehicle passing...............");
        uint8_t data[] = "ALARM";
        nrf24.send(data, sizeof(data));
        nrf24.waitPacketSent();
    }  
    if(((millis()-tempus)>UPTIME)&&etat==1){
      digitalWrite(RM_DE,1);
      etat=0;
      Serial.println("Alarm expired!!!!!");
      delay(500);
      digitalWrite(RM_DE,0);
  }
}
