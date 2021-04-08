// Setup of radio.
void setup_radio() {
  SPI.begin();  // For SPI communication.
  radio.begin();  // Initialization of nRF24L01 module.
  radio.setPALevel(RF24_PA_HIGH); 
  network.begin( node_address); // Setting the node address.
  network.multicastRelay = true;  // Enabling multicast.

  if (00 != node_address) {
    network.multicastLevel(1);
  }
}

// Setup of LCD (16x2 LCD Display with I2C Backpack). Also displays the splash screen with custom text.
void setup_lcd(char *text, int delay_time) {
  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.createChar(0, horizontal_line);
  lcd.createChar(1, vertical_line);
  lcd.home(); // Setting the cursor at home position.
  lcd.setCursor( ((16-strlen(text))/2)+1, 0); // Adjusting cursor position such that Custom text is in center.
  lcd.print(text);
  char* product_name = "Flyp & Klyk!";
  lcd.setCursor( ((16-strlen(product_name))/2)+1, 1);
  lcd.print(product_name);
  delay(delay_time);
  lcd.clear();
  lcd.setBacklight(LOW);
  lcd.home();
}

// Callback of lcd_update Task. For visual conformation that the code is running as expected.
void lcd_update_callback() {
  lcd.setCursor( 15, 1);
  if ( line == horizontal ) {
    lcd.write(byte(1));
    line = vertical;
  }
  else {
    lcd.write(byte(0));
    line = horizontal;
  }
  lcd.home();
}

// Callback of network_update Task.
void network_update_callback() {
  network.update(); // Needs to be executed regularly to keep the network alive.
}

// Function to check for message. This function handles two types of messages: Text messages, Acknowledgement messages.
void check_message() {
  while( network.available() ) {
    RF24NetworkHeader header;
    network.peek(header);
    
    // Switching based on type of message.
    switch (header.type) {
      case 'C':
        network.read(header,&message, sizeof(message_t));
        message_update.enable();
        message_update.restart();
        break;

      case 'A':
        network.read(header, 0, 0);
        message_update.disable();
        break;

      default:
        network.read(header, 0, 0);
        break;
    };
  }
}

// Callback when message_update Task is enabled.
bool message_update_enable() {
  
  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print(message.buffer);
  
  // Displays Low battery warning.
  if ( message.bat_volt < 200 && message.bat_volt != 0 ){
    lcd.setCursor(0, 1);
    lcd.print("Low Bat:");
    lcd.print(message.bat_volt);
  }
  
  attachInterrupt(digitalPinToInterrupt(button_pin), message_update_ISR ,FALLING);
  return true;
}

// Callback for message_update.
void message_update_callback() {
  if ( (message_update.getRunCounter() == 2) || (message_update.getRunCounter() == 3) ) {
    play_buzzer.restart();
  }
}

// Callbackwhen message_update Task is disabled.
void message_update_disable() {
  detachInterrupt(digitalPinToInterrupt(button_pin));
  noTone(buzzer_pin);
  lcd.clear();
  lcd.setBacklight(LOW);
  lcd_update.forceNextIteration();
  
}

// Interrupt Service Routine attached to digital pin 4.
void message_update_ISR() {
  disable_message_update = true;
}

// Callback for play_buzzer Task.
void play_buzzer_callback() {
  switch (play_buzzer.getRunCounter()) {
    case 1:
      tone( buzzer_pin, message.freq + 10, 500);
      break;

    case 2:
      tone( buzzer_pin, message.freq - 20, 500);
      break;

    case 3:
      tone( buzzer_pin, message.freq - 10, 500);
      break;

    case 4:
      tone( buzzer_pin, message.freq, 500);
      break;

    default:
      break;
  }
}
