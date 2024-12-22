#include <IRremote.h>

int IR_RECEIVE_PIN = 10;
int ledPin=8;            //LED -digital 3
boolean ledState=LOW;         //ledState用来存储LED的状态


void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(ledPin,OUTPUT);         //设置LED为输出状态
  
}

void loop() {
   if(IrReceiver.decode()){
   Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

   if(IrReceiver.decodedIRData.decodedRawData == 0xFF00BF00){       
        ledState=!ledState;

       digitalWrite(ledPin,ledState);
    }
   IrReceiver.resume();
  }
}

