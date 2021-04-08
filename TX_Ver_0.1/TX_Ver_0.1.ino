/*--------------------
	Flyp & Klyk.
	
	Self-sustaining system used to call office staff on demand.
	
	This code is for the button or transmitter. 
	
	Dependencies:
	- RF24                    (https://github.com/nRF24/RF24)
	- RF24Network             (https://github.com/nRF24/RF24Network)
	- BatteryVoltageReader    (https://github.com/RalphBacon/Arduino-Battery-Monitor)

	*** Edit variables present in file: "variables.h" ***

  --------------------*/

#include "includes.h"   // Header file for libraries and variables

void setup() {
  
  BVR.begin();
  SPI.begin();    
  radio.begin();    
  radio.setPALevel(RF24_PA_HIGH);   
  network.begin(node_address);
  network.setup_watchdog(wdt_2s);   // Setup of watchdog for sleep time of 2 seconds.
  network.multicastLevel(2);        // Setting multicastLevel as 2 for transmitters as to not wake in case of multicast.
  network.multicastRelay = true;    // Enabling multicast relay.

  attachInterrupt(digitalPinToInterrupt(button_pin), button_ISR, FALLING);    // Setting up interrupt (falling) on digital pin 3. 

  pinMode(led_pin, OUTPUT);   // Setting up LED for indication.

  // Setting up message/payload to be sent.
  strlcpy(message.buffer, text, sizeof(message.buffer));
  message.freq = frequency;   
  message.bat_volt = BVR.readVCC();
  
}

void loop() {
  network.update();   // Needs to be called frequently to keep the network active.

  if( button_pressed ) {  
    button_pressed = false;  
    message.bat_volt = BVR.readVCC();   // Updating battery voltage before sending.

    bool success = send_C();    // To send message to display.
    blink_led(success);     // To show status of transmission.
    
    // Putting transmitter to sleep to reduce power consumption.
    if ( millis() - sleep_timer > awake_time ) {
      sleep_timer = millis();
      delay(50);
      radio.stopListening();
      network.sleepNode(5, NRF_interrupt_pin);      // sleepNode(cycles, interrupt-pin)
    }
  }
}
