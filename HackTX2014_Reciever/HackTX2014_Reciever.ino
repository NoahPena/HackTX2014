#include <VirtualWire.h>

int receivePin = 4;
int motor = 3;
uint8_t lastTen[10];
int counter = 0;

void setup()
{
  Serial.begin(9600);
  
  vw_set_rx_pin(receivePin);
  vw_set_ptt_inverted(true);
  vw_setup(2000);
  
  vw_rx_start();
  
  pinMode(motor, OUTPUT);
  
}

void loop()
{
  uint8_t buf[1];
  uint8_t buflen = 1;
  
  if(vw_get_message(buf, &buflen))
  {
    lastTen[counter] = buf[0];
    counter++;
    if(buf[0] == 0)
    {
     //Do Nothing 
    }
    
    if(buf[0] == 1)
    {
     //Do Something 
    }
  } else {
    
  }
}
