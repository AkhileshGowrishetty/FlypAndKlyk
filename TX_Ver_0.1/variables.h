const uint16_t node_address = 011;   // Edit to set new address.

const char text[] = "TEXT";   // Enter text to be displayed between quotes.
const int frequency = 262;    // Frequency of buzzer tone.

extern long InternalReferenceVoltage = 1095L;   // Check BatteryVoltageMonitor Repository/Video on how to get this value. 

/* ----------
    Edit the BatteryVoltageReader.cpp.
      - Find the variable "InternalReferenceVoltage". It will be a constant. 
      - Remove the word "const" before long so that we can edit the InternalReferenceVoltage here.
   ---------- */
