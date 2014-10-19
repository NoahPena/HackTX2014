#include <stdio.h>
#include <RF24.h>
#include <SPI.h>

int receivePin = 4;
int motor = 9;
uint8_t lastTen[10];
int counter = 0;
int counterNothing = 0;

RF24 radio(9, 10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup()
{
  Serial.begin(9600);
//  Serial.println("cool");

  radio.begin();
  
  radio.setRetries(15,15);
  
  radio.setPayloadSize(8);
  radio.setChannel(0x4c);
  radio.setPALevel(RF24_PA_MAX);
  
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  
  radio.startListening();
  
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
  
  if(radio.available())
  {
    digitalWrite(motor, LOW);
    counterNothing = 0;
    
    bool done = false;
    unsigned long got_time;
    
    while(!done)
    {
       done = radio.read(&got_time, sizeof(unsigned long)); 
       
       delay(20);
    }
    
    radio.stopListening();
    
    radio.write(&got_time, sizeof(unsigned long));
    
    radio.startListening();
    
    Serial.println(got_time);
    lastTen[counter] = got_time;
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
