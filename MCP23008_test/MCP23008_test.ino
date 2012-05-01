#include <Wire.h>
#include <AdafruitMCP23008.h>

// Test i/o expansion. 
// Checks all 8 pins of MCP23008 as outputs then as inputs
// Does not yet test pull ups or interrupts

// Connect pin #1 of the expander to Analog 5 (i2c clock)
// Connect pin #2 of the expander to Analog 4 (i2c data)
// Connect pins #3, 4 and 5 of the expander to ground (address selection)
// Connect pin #6 and 18 of the expander to 5V (power and reset disable)
// Connect pin #9 of the expander to ground (common ground)

// MCP channels #0 to #7 (pins 10-17) connected to Arduino digital pins 5 to 12

// If you add an LED to pin PASS_PIN, it will light up while the tests are passing. 
// If any test fails, it will turn off until the next time the tests are run and FAIL_PIN will turn on

AdafruitMCP23008 mcp;

const int arduinoLowPin = 5;
const int pinCount = 8;
const int PASS_PIN = 2;
const int FAIL_PIN = 3;

void setup() { 
  Serial.begin(9600); 
  mcp.begin();      // use default address 0
  pinMode(PASS_PIN, OUTPUT);
  pinMode(FAIL_PIN, OUTPUT);
}

boolean checkEqual(int pin, int expected, int actual, char * description) {
     if (expected != actual) {
       Serial.print("> FAIL> ");
       Serial.print(description);
       Serial.print(" Pin ");
       Serial.print(pin);
       Serial.print(" was ");       
       Serial.println(actual ? "HIGH" : "LOW");
       return false;
     }
     return true;
}

void setMcpPinsMode(int mode) {
  for (int i = 0; i < pinCount; i++) {
    mcp.pinMode(i, mode);
  }
}

int arduinoPin(int pin) {
  return arduinoLowPin + pin;
}

void setArduinoPinsMode(int mode) {
  for (int i = 0; i < pinCount; i++) {
    pinMode(arduinoPin(i), mode);
  }
}

void writeArduino(int arduinoPin, int signal) {
    digitalWrite(arduinoPin, signal);
    delay(1);
}

void writeMcp(int pin, int signal) {
    mcp.digitalWrite(pin, signal);
    delay(1);
}

void loop() {
  boolean testsPassing = true;
  Serial.println("MCP to Arduino"); 
  setMcpPinsMode(OUTPUT);
  setArduinoPinsMode(INPUT);
  
  for (int i = 0; i < pinCount; i++) {
    const int arduinoPinI = arduinoPin(i);
    writeMcp(i, LOW);
    testsPassing = testsPassing && checkEqual(i, LOW, digitalRead(arduinoPinI), "mcp -> ard");
    
    writeMcp(i, HIGH);
    testsPassing = testsPassing && checkEqual(i, HIGH, digitalRead(arduinoPinI), "mcp -> ard");
  }
  
  Serial.println("Arduino to MCP"); 
  setMcpPinsMode(INPUT);
  setArduinoPinsMode(OUTPUT);
  
  for (int i = 0; i < pinCount; i++) {
    const int arduinoPinI = arduinoPin(i);
    writeArduino(arduinoPinI, LOW);
    testsPassing = testsPassing && checkEqual(i, LOW, mcp.digitalRead(i), "ard -> mcp");

    writeArduino(arduinoPinI, HIGH);
    testsPassing = testsPassing && checkEqual(i, HIGH, mcp.digitalRead(i), "ard -> mcp");
  }
  
  digitalWrite(PASS_PIN, testsPassing ? HIGH  : LOW);
  digitalWrite(FAIL_PIN, testsPassing ? LOW : HIGH);
  delay(500);
}

