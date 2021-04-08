// Interrupt Service Routine for button interrupt.
void button_ISR() {
  button_pressed = true;
  wasInterrupted = true;
  sleep_cycles_remaining = 0;
}

// Function to send message to display.
bool send_C() {
  uint16_t to_node = 00;
  RF24NetworkHeader header( to_node, 'C');
  
  bool tx_status = network.multicast(header, &message, sizeof(message), 1);
  tx_status &= network.write(header, &message, sizeof(message));
  delay(50);
  
  return tx_status;
}

// Function to show status of transmission.
void blink_led(bool success)
{
  if (success)
  {
    for (byte i = 0; i < 3; i++)
    {
      digitalWrite(led_pin, HIGH);
      delay(50);
      digitalWrite(led_pin, LOW);
      delay(100);
    }
  } 
  else
  {
    digitalWrite(led_pin, HIGH);
    delay(50);
    digitalWrite(led_pin, LOW);
  }
}
