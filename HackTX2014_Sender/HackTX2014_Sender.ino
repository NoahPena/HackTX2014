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
     vw_send(1, 1);
     vw_wait_tx(); 
  } else {
     vw_send(0, 1);
     vw_wait_tx(); 
  }
  
  delay(500);
}
