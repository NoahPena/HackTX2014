
/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 03/17/2013 : Charles-Henri Hallard (http://hallard.me)
              Modified to use with Arduipi board http://hallard.me/arduipi
						  Changed to use modified bcm2835 and RF24 library 

 */

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */


#include <stdio.h>
#include <RF24.h>
#include <SPI.h>

//
// Hardware configuration
//

// CE Pin, CSN Pin, SPI Speed
//RF24 radio(11, 12);  //teensy 2.0++
RF24 radio(9, 10);  //teensy 3,0
//RF24 radio(48,49);  //arduino Mega


int motor = 4;
int lastTen[10];
int counter = 0;
int counterNothing = 0;


//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;


void setup() {
  //pinMode(20,OUTPUT);//Teensy++ 2.0 SS
  //pinMode(53,OUTPUT); //Arduino mega SS
  // Role
  role = role_pong_back;

  //
  // Print preamble:
  //
  //delay(10000);
  Serial.begin(115200);
  //printf_begin();
  Serial.println("RF24/examples/pongtest/\n");
  
  Serial.print("ROLE: ");
  Serial.println(role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

Serial.println("Setting up channel");
  // optionally, reduce the payload size.  seems to
  // improve reliability
	radio.setPayloadSize(8);
	radio.setChannel(0x4c);
  radio.setPALevel(RF24_PA_MAX);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
  Serial.println("Setting up pipes");
  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //
  Serial.println("Start listening");
  radio.startListening();
  //
  // Dump the configuration of the rf unit for debugging
  //
  //radio.printDetails();
  Serial.println("End of setup process. Starting loop");
  
  pinMode(motor, OUTPUT);
}

void loop(){
  //
  // Ping out role.  Repeatedly send the current time
  //
		if (role == role_ping_out)
		{
			// First, stop listening so we can talk.
			radio.stopListening();

			// Take the time, and send it.  This will block until complete
			unsigned long time = millis();
			Serial.print("Now sending ");
                        Serial.println(time);
			bool ok = radio.write( &time, sizeof(unsigned long) );
			
			if (ok)
				Serial.println("ok...");
			else
				Serial.println("failed.\n");

			// Now, continue listening
			radio.startListening();

			// Wait here until we get a response, or timeout (250ms)
			unsigned long started_waiting_at = millis();
			bool timeout = false;
			while ( ! radio.available() && ! timeout ) {
					// by bcatalin » Thu Feb 14, 2013 11:26 am
					delay(5); //add a small delay to let radio.available to check payload
				if (millis() - started_waiting_at > 200 )
					timeout = true;
			}


			// Describe the results
			if ( timeout )
			{
				Serial.println("Failed, response timed out.\n");
			}
			else
			{
				// Grab the response, compare, and send to debugging spew
				unsigned long got_time;
				radio.read( &got_time, sizeof(unsigned long) );

				// Spew it
                                Serial.print("Got response");
                               Serial.println(got_time);
                               Serial.print("round-trip delay: ");
                               Serial.println(millis()-got_time);
			}

			// Try again 1s later
			delay(1000);
			
			
		}

		//
		// Pong back role.  Receive each packet, dump it out, and send it back
		//

		if ( role == role_pong_back )
		{
                      if(counter == 10)
                      {
                          counter = 0;
                      } 
             
			// if there is data ready
			//printf("Check available...\n");
			if ( radio.available() )
			{
                            digitalWrite(motor, LOW);
                              counterNothing = 0;
                              
                             
				// Dump the payloads until we've gotten everything
				unsigned long got_time;
				bool done = false;
				
				while (!done)
				{
					// Fetch the payload, and see if this was the last one.
					done = radio.read( &got_time, sizeof(unsigned long) );
                                        lastTen[counter] = got_time;
                                        counter++;
					// Spew it
                                        Serial.print("Got payload(");
                                        Serial.print(sizeof(unsigned long));
                                        Serial.print(") ");
                                        Serial.print(got_time);
					// Delay just a little bit to let the other unit
					// make the transition to receiver
					delay(20);
				}

				// First, stop listening so we can talk
				radio.stopListening();

				// Send the final one back.
				radio.write( &got_time, sizeof(unsigned long) );

				// Now, resume listening so we catch the next packets.
				radio.startListening();
			} else {
                              counterNothing++;
                              Serial.println("Nothing");
                              
                              if(counterNothing >= 5)
                              {
                                 int amount = (lastTen[0] + lastTen[1] + lastTen[2] + lastTen[3] + lastTen[4] + lastTen[5] + lastTen[6] + lastTen[7] + lastTen[8] + lastTen[9])/2; 
                                     Serial.println(amount);
                                     if(amount > (255/2))
                                     {
                                         digitalWrite(motor, HIGH);
                                     }
                              }
                          }
		}

    delay(1200);
	} // forever loop



