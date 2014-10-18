#include <VirtualWire.h>

int transmitPin = 7;
int fsrPin = A0;
int value = 0;

int threshold = 0;

void setup()
{
  Serial.begin(9600);
  
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(transmitPin);
  vw_setup(2000);
  
}

void loop()
{
  value = analogRead(fsrPin);
  
  if(value > threshold)
  {
     vw_send((uint8_t*)1, 1);
     vw_wait_tx(); 
    Serial.print("Send 1");
  } else {
     vw_send((uint8_t*)0, 1);
     vw_wait_tx(); 
     Serial.print("Send 0");
  }
  
  delay(500);
}
