#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include "printf.h"

RF24 radio(9,10);

//int transmitPin = 7;
int fsrPin = A0;
int value = 0;

int threshold = 0;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup()
{
  Serial.begin(9600);
  
  radio.begin();
  
  radio.setRetries(15, 15);
  
  radio.setPayloadSize(8);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  
  radio.startListening();
  
}

void loop()
{
  value = analogRead(fsrPin);
  
  if(value > threshold)
  {
     sendValue(1);
     Serial.print("Send 1");
  } else {
     sendValue(0); 
     Serial.print("Send 0");
  }
  
  delay(500);
}

void sendValue(int value)
{
 radio.stopListening();
 radio.write(&value, sizeof(int));

 radio.startListening(); 
}
