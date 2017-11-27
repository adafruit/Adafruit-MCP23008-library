#include <Wire.h>
#include "Adafruit_MCP23008.h"

Adafruit_MCP23008 mcp;

void setup() {
  Serial.begin(9600);

  mcp.begin();      // use default address 0

  // Set the output pin to HIGH when an interrupt had occured
  mcp.setInterruptOutPinMode(MCP23008_INT_OUT_HIGH);

  mcp.pinMode(0, INPUT);

  // Enable interrupts for pin 0 of type RISING
  mcp.enableInterrupt(0, RISING);
}


void loop() {
  uint8_t validInterrupts = mcp.readInterrupts();

  if (validInterrupts) {
    // Go through the pins
    for (int pin=0; pin < 8; pin++) {
      // If the bit for the pin in validInterrupts is set then we have an interrupt on it
      if ((validInterrupts >> pin) & 1) {
        Serial.print("Interrupt on pin: ");
        Serial.println(pin);
      }
    }
  }

  delay(1);
}
