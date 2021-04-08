/*--------------------
	Flyp & Klyk.
	
	Self-sustaining system used to call office staff on demand.
	
	This code is for the button or transmitter. 
	
	Dependencies:
	- RF24                 (https://github.com/nRF24/RF24)
	- RF24Network          (https://github.com/nRF24/RF24Network)
	- LiquidCrystal_I2C    (https://github.com/fmalpartida/New-LiquidCrystal)
  - TaskScheduler        (https://github.com/arkhipenko/TaskScheduler)
  
	*** Edit variables present in file: "variables.h" ***

  --------------------*/
  
#include "includes.h"   // Header file for libraries and variables.

void setup() {

  pinMode(button_pin, INPUT);
  
  pinMode(buzzer_pin, OUTPUT);
  
  setup_radio();

  setup_lcd("TEXT", 2000);    // Setup of LCD with Custom text ( Length of text must be less than or equal to 16 characters).
  
}

void loop() {
  ts.execute();   

  check_message();
  
  if ( disable_message_update ) {
    message_update.disable();
    disable_message_update = false;
    
    uint16_t to_node = 00;
    RF24NetworkHeader header(to_node, 'A');
    bool send_ack = true;
    
    if ( node_address == 00 ) {
      network.multicast(header, &send_ack, sizeof(send_ack), 1);
    }
    else {
      network.multicast(header, &send_ack, sizeof(send_ack), 1);
      network.write(header, &send_ack, sizeof(send_ack));
    }
    
  }
}
