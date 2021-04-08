// Libraries used.
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>  // LiquidCrystal Library for driving LCD Display.
#include <TaskScheduler.h>
#include "variables.h"

// Pins for nRF24L01+ modules.
#define CE 7
#define CS 8

#define button_pin 3
#define buzzer_pin 9

// Prototypes for functions.
void setup_radio();
void setup_lcd(char *text, int delay_time);
void lcd_update_callback();       // Callback for lcd_update Task.
void network_update_callback();   // Callback for network_update Task.
void message_update_callback();   // Callback for message_update Task.
bool message_update_enable();     // Callback when message_update Task is enabled.
void message_update_disable();    // Callback when message_update Task is disabled.
void play_buzzer_callback();      // Callback for play_message Task.
void message_update_ISR();        // Interrupt Service Routine for interrupt on digital pin 3.
void check_message();             // Function to check new message.

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
RF24 radio(7, 8);  // CE & CS pins to use (Using 7,8 on Uno,Nano,attiny841)
RF24Network network(radio);

Scheduler ts;   // Scheduler

// Task to update and relay messages among network.
Task network_update ( TASK_IMMEDIATE, TASK_FOREVER, &network_update_callback, &ts, true);

// Task to refresh line position in right corner of LCD display.
Task lcd_update ( 500 * TASK_MILLISECOND, TASK_FOREVER, &lcd_update_callback, &ts, true);

/*
  Task when enabled attaches interrupt on digital pin 3. Prints text from received message.
  A tone is played in last two iterations. Tone is handled by play_buzzer Task.
  When Task is disabled, tone is stopped and interrupt is dettached.
*/
Task message_update ( 7 * TASK_SECOND, 5, &message_update_callback,  &ts, false, &message_update_enable , &message_update_disable);
// This Task handles playing buzzer.
Task play_buzzer ( 1 * TASK_SECOND, 4, &play_buzzer_callback, &ts, false);

// Custom characters for lcd. Maximum 8 characters can be created.
byte horizontal_line[] = {
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000
};

byte vertical_line[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

enum line_position {
  horizontal,
  vertical
};

// Structure of received payload.
typedef struct
{
  char buffer[17];
  int freq = 0;
  unsigned int bat_volt;
} message_t;

volatile bool disable_message_update = false;

message_t message;
line_position line = vertical;
