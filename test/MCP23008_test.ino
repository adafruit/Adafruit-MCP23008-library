#include <Wire.h>
#include "Adafruit_MCP23008.h"

// Test i/o expansion. 
// Checks all 8 pins of MCP23008 as outputs then as inputs
// Does not yet test pull ups or interrupts

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// MCP channels #0 to #7 (pins 10-17) connected to Arduino digital pins 5 to 12

Adafruit_MCP23008 mcp;

int arduinoLowPin = 5;
int pinCount = 8;

void setup() { 
  Serial.begin(9600); 
  mcp.begin();      // use default address 0
}

void checkEqual(int pin, int expected, int actual, char * description) {
     if (expected != actual) {
       Serial.print("> FAIL> ");
       Serial.print(description);
       Serial.print(" Pin ");
       Serial.print(pin);
       Serial.print(" was ");       
       Serial.println(actual ? "HIGH" : "LOW");
     }
}

void setMcpPinsMode(int mode) {
  for (int i = 0; i < pinCount; i++) {
    mcp.pinMode(i, mode);
  }
}

void setArduinoPinsMode(int mode) {
  for (int i = 0; i < pinCount; i++) {
    pinMode(arduinoLowPin + i, mode);
  }
}

void writeArduino(int pin, int signal) {
    digitalWrite(arduinoLowPin + pin, signal);
    delay(1);
}

void writeMcp(int pin, int signal) {
    mcp.digitalWrite(pin, signal);
    delay(1);
}

void loop() {
  Serial.println("MCP to Arduino"); 
  setMcpPinsMode(OUTPUT);
  setArduinoPinsMode(INPUT);
  
  for (int i = 0; i < pinCount; i++) {
    writeMcp(i, LOW);
    checkEqual(i, LOW, digitalRead(arduinoLowPin + i), "mcp -> ard");
    
    writeMcp(i, HIGH);
    checkEqual(i, HIGH, digitalRead(arduinoLowPin + i), "mcp -> ard");
  }
  
  Serial.println("Arduino to MCP"); 
  setMcpPinsMode(INPUT);
  setArduinoPinsMode(OUTPUT);
  
  for (int i = 0; i < pinCount; i++) {
    writeArduino(i, LOW);
    checkEqual(i, LOW, mcp.digitalRead(i), "ard -> mcp");

    writeArduino(i, HIGH);
    checkEqual(i, HIGH, mcp.digitalRead(i), "ard -> mcp");
  }
  delay(1000);
}

