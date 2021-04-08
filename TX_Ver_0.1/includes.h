// Libraries used.
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <BatteryVoltageReader.h>
#include "variables.h"

// Pins for nRF24L01+ modules.
#define CE 7
#define CS 8
#define NRF_interrupt_pin 0

#define led_pin 9
#define button_pin 3

// Prototypes for functions.
void button_ISR();
bool send_C();
void blink_led(bool success);

// Structure of payload.
typedef struct {
  char buffer[17];
  int freq = 0;
  unsigned int bat_volt;
} message_t;

message_t message;

RF24 radio( CE, CS);
RF24Network network(radio);

BatteryVoltageReader BVR;

volatile bool button_pressed = false;

// Variables from RF24Network library. Used to prevent sleep after wakeup due to button interrupt.
extern volatile bool wasInterrupted;
extern volatile byte sleep_cycles_remaining;

//This is for sleep mode. It is not really required, as users could just use the number 0 through 10
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;
unsigned long awake_time = 500;  // How long in ms the radio will stay awake after leaving sleep mode
unsigned long sleep_timer = 0;
