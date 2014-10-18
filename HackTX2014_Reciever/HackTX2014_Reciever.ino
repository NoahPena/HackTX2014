#include <VirtualWire.h>

int receivePin = 4;
int motor = 3;
uint8_t lastTen[10];
int counter = 0;
int counterNothing = 0;

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
  Serial.println("*******");
  Serial.println("");
  Serial.print("Counter: ");
  Serial.println(counter);
  Serial.print("CounterNothing: ");
  Serial.println(counterNothing);
  Serial.println("********");
  Serial.println("");
  
  
  uint8_t buf[1];
  uint8_t buflen = 1;
  if(counter == 10)
  {
   counter = 0; 
  }
  
  if(vw_get_message(buf, &buflen))
  {
    digitalWrite(motor, LOW);
    counterNothing = 0;
    Serial.println(buf[0]);
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
    counterNothing++;
    Serial.println("Nothing");
    
    if(counterNothing >= 5)
    {
     int amount = (lastTen[0] + lastTen[1] + lastTen[2] + lastTen[3] + lastTen[4] + lastTen[5] + lastTen[6] + lastTen[7] + lastTen[8] + lastTen[9])/2; 
     if(amount < (255/2))
     {
         digitalWrite(motor, HIGH);
     }
    }
  }
  
  delay(1000);
}
